#include "DefHeader.h"
#include "WICImageProcessor.h"

bool common::image::WICImageProcessor::wicToMemory(const WIC_META& inputMeta, void* targetMemory, UINT targetSize) noexcept {
    // Check meta
    if (inputMeta.fromMemory && (!inputMeta.Memory.ptrData || !inputMeta.Memory.size)) {
        return false;
    }
    if (!inputMeta.fromMemory && (inputMeta.fileHandle == INVALID_HANDLE_VALUE || inputMeta.fileHandle == NULL)) {
        return false;
    }
    
    // Check target
    if (!targetMemory || targetSize < inputMeta.stride * inputMeta.height) {
        return false;
    }
    
    // Create WIC Factory
    ScopedComPointer<IWICImagingFactory> ptrFactory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ptrFactory.to())))) {
        return false;
    }

    // Open decoder
    ScopedComPointer<IWICBitmapDecoder> ptrDecoder;
    if (inputMeta.fromMemory) {
        // Create stream
        ScopedComPointer<IWICStream> ptrStream;
        if (FAILED(ptrFactory->CreateStream(ptrStream.to()))) {
            return false;
        }

        // Init stream
        if (FAILED(ptrStream->InitializeFromMemory((BYTE*)inputMeta.Memory.ptrData, (DWORD)inputMeta.Memory.size))) {
            return false;
        }

        // Open from memory
        if (FAILED(ptrFactory->CreateDecoderFromStream(ptrStream, NULL, WICDecodeMetadataCacheOnLoad, ptrDecoder.to()))) {
            return false;
        }
    }
    else {
        // Open from File
        if (FAILED(ptrFactory->CreateDecoderFromFileHandle((ULONG_PTR)inputMeta.fileHandle, NULL, WICDecodeMetadataCacheOnLoad, ptrDecoder.to()))) {
            return false;
        }
    }

    // Get frame
    ScopedComPointer<IWICBitmapFrameDecode> ptrFrame;
    if (FAILED(ptrDecoder->GetFrame(0, ptrFrame.to()))) {
        return false;
    }

    // Check if direct copy is avalible
    if (inputMeta.nativePixelFormat == inputMeta.targetPixelFormat) {
        // Build copy rect
        WICRect copyRect;
        copyRect.X = 0;
        copyRect.Y = 0;
        copyRect.Width = inputMeta.width;
        copyRect.Height = inputMeta.height;
        
        // Copy pixels
        if (FAILED(ptrFrame->CopyPixels(&copyRect, inputMeta.stride, targetSize, (BYTE*)targetMemory))) {
            return false;
        }
    }
    // Conversion required
    else {
        // Create converter
        ScopedComPointer<IWICFormatConverter> ptrFormatConverter;
        if (FAILED(ptrFactory->CreateFormatConverter(ptrFormatConverter.to()))) {
            return false;
        }

        // Check if conversion is right
        BOOL conversionAvailible = FALSE;
        if (FAILED(ptrFormatConverter->CanConvert(inputMeta.nativePixelFormat, inputMeta.targetPixelFormat, &conversionAvailible)) || !conversionAvailible) {
            return false;
        }

        // Init converter
        if (FAILED(ptrFormatConverter->Initialize(ptrFrame, inputMeta.targetPixelFormat, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom))) {
            return false;
        }

        // Copy output
        WICRect rcDest;
        rcDest.X = 0;
        rcDest.Y = 0;
        rcDest.Width = inputMeta.width;
        rcDest.Height = inputMeta.height;
        if (FAILED(ptrFormatConverter->CopyPixels(&rcDest, targetSize / inputMeta.height, targetSize, (BYTE*)targetMemory))) {
            return false;
        }
    }

    // Passed --> true
    return true;
}
