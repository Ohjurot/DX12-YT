#include "DefHeader.h"
#include "WICMeta.h"

// [GUID] --> DXGI
const GUID common::image::WicIO::guid_to_dxgi_SOURCE[CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_DXGI] = {
    // 64 BPP
    GUID_WICPixelFormat64bppRGBA,   // R16G16B16A16

    // 32 BPP
    GUID_WICPixelFormat32bppBGR,    // B8G8R8X8
    GUID_WICPixelFormat32bppBGRA,   // B8G8R8A8
    GUID_WICPixelFormat32bppRGBA,   // R8G8B8A8
};
// GUID --> [DXGI]
const DXGI_FORMAT common::image::WicIO::guid_to_dxgi_TARGET[CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_DXGI] = {
    // 64 BPP
    DXGI_FORMAT_R16G16B16A16_UNORM,

    // 32 BPP
    DXGI_FORMAT_B8G8R8X8_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM,
};
// [GUID] --> GUID
const GUID common::image::WicIO::guid_to_guid_SOURCE[CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_GUID] = {
    // 24 BPP
    GUID_WICPixelFormat24bppBGR,
};
// GUID --> [GUID]
const GUID common::image::WicIO::guid_to_guid_TARGET[CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_GUID] = {
    // 32 BPP
    GUID_WICPixelFormat32bppBGR,
};

bool common::image::WicIO::open(LPCWSTR path, WIC_META& refMeta) noexcept {
    // Open file
    refMeta.fileHandle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    refMeta.fromMemory = false;
    if (refMeta.fileHandle != INVALID_HANDLE_VALUE) {
        // Open wic factory
        ScopedComPointer<IWICImagingFactory> ptrWicFactory;
        if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ptrWicFactory.to())))) {
            // Open decoder from file
            ScopedComPointer<IWICBitmapDecoder> ptrWicDecoder;
            if (SUCCEEDED(ptrWicFactory->CreateDecoderFromFileHandle((ULONG_PTR)refMeta.fileHandle, NULL, WICDecodeMetadataCacheOnLoad, ptrWicDecoder.to()))) {
                return open(ptrWicFactory, ptrWicDecoder, refMeta);
            }
        }
    }
    
    // Fallback false
    return false;
}

bool common::image::WicIO::open(void* data, UINT64 size, WIC_META& refMeta) noexcept {
    // Load from memory
    refMeta.Memory.ptrData = data;
    refMeta.Memory.size = size;
    refMeta.fromMemory = true;
    if (data && size) {
        // Open wic factory
        ScopedComPointer<IWICImagingFactory> ptrWicFactory;
        if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ptrWicFactory.to())))) {
            // Create stream
            ScopedComPointer<IWICStream> ptrWicStream;
            if (FAILED(ptrWicFactory->CreateStream(ptrWicStream.to()))) {
                return false;
            }

            // Init stream
            if (FAILED(ptrWicStream->InitializeFromMemory((BYTE*)refMeta.Memory.ptrData, (DWORD)refMeta.Memory.size))) {
                return false;
            }
            
            // Open decoder from stream
            ScopedComPointer<IWICBitmapDecoder> ptrWicDecoder;
            if (SUCCEEDED(ptrWicFactory->CreateDecoderFromStream(ptrWicStream, NULL, WICDecodeMetadataCacheOnLoad, ptrWicDecoder.to()))) {
                return open(ptrWicFactory, ptrWicDecoder, refMeta);
            }
        }
    }

    // Fallback false
    return false;
}

void common::image::WicIO::close(WIC_META& refMeta) noexcept {
    // Check file close
    if (!refMeta.fromMemory && refMeta.fileHandle != INVALID_HANDLE_VALUE) {
        // Close file
        CloseHandle(refMeta.fileHandle);
        refMeta.fileHandle = INVALID_HANDLE_VALUE;
    }

    // Unset
    refMeta.Memory.ptrData = nullptr;
    refMeta.Memory.size = 0;
    refMeta.fromMemory = false;
}

bool common::image::WicIO::open(IWICImagingFactory* ptrFactory, IWICBitmapDecoder* ptrDecoder, WIC_META& refMeta) noexcept {
    // Get first (only) frame
    ScopedComPointer<IWICBitmapFrameDecode> ptrWicFrame;
    if (SUCCEEDED(ptrDecoder->GetFrame(0, ptrWicFrame.to()))) {
        // Get size
        if (FAILED(ptrWicFrame->GetSize(&refMeta.width, &refMeta.height))) {
            return false;
        }

        // Get native format
        if (FAILED(ptrWicFrame->GetPixelFormat(&refMeta.nativePixelFormat))) {
            return false;
        }

        // Check direct conversion
        if (convert_GUID_to_DXGI(refMeta.nativePixelFormat, &refMeta.targetGiFormat)) {
            // Source and target are eqal
            refMeta.targetPixelFormat = refMeta.nativePixelFormat;
        }
        else {
            // Try wic converter search
            if (!convert_GUID_to_GUID(refMeta.nativePixelFormat, &refMeta.targetPixelFormat)) {
                return false;
            }

            // Find DXGI Format
            if (!convert_GUID_to_DXGI(refMeta.targetPixelFormat, &refMeta.targetGiFormat)) {
                return false;
            }
        }

        // Get component info
        ScopedComPointer<IWICComponentInfo> ptrComponentInfo;
        ptrFactory->CreateComponentInfo(refMeta.targetPixelFormat, ptrComponentInfo.to());

        // Query for pixel format info
        ScopedComPointer<IWICPixelFormatInfo> ptrFormatInfo;
        if (!ptrComponentInfo.queryInterface(ptrFormatInfo)) {
            return false;
        }

        // Get BBP 
        if (FAILED(ptrFormatInfo->GetBitsPerPixel(&refMeta.bpp))) {
            return false;
        }

        // Compute stride
        refMeta.stride = (refMeta.width * refMeta.bpp + 7) / 8;

        // Done
        return true;
    }

    // Fallback false
    return false;
}

bool common::image::WicIO::convert_GUID_to_DXGI(GUID inputGuid, DXGI_FORMAT* ptrFormat) noexcept {
    // Find matching GUID
    for (unsigned int i = 0; i < CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_DXGI; i++) {
        if (guid_to_dxgi_SOURCE[i] == inputGuid) {
            // Set and return
            *ptrFormat = guid_to_dxgi_TARGET[i];
            return true;
        }
    }

    // Fallback false
    return false;
}

bool common::image::WicIO::convert_GUID_to_GUID(GUID inputGuid, GUID* ptrGuid) noexcept {
    // Find matching GUID
    for (unsigned int i = 0; i < CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_GUID; i++) {
        if (guid_to_guid_SOURCE[i] == inputGuid) {
            // Set and return
            *ptrGuid = guid_to_guid_TARGET[i];
            return true;
        }
    }

    // Fallback false
    return false;
}
