#include "DefHeader.h"
#include "ShaderCompiler.h"

bool dx::ShaderCompiler::compileShader(LPCWSTR inputFile, LPCWSTR outputFile, LPCWSTR debugFile, SHADER_TYPE type) {
    // Create util instance
    ScopedComPointer<IDxcUtils> ptrUtils;
    EVALUATE_HRESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(ptrUtils.to())), "DxcCreateInstance(...) for IDxcUtils");

    // Create compiler instance
    ScopedComPointer<IDxcCompiler3> ptrCompiler;
    EVALUATE_HRESULT(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(ptrCompiler.to())), "DxcCreateInstance(...) for IDxcCompiler3");

    // Create command args
    ScopedComPointer<IDxcCompilerArgs> ptrCompilerArgs;
    EVALUATE_HRESULT(DxcCreateInstance(CLSID_DxcCompilerArgs, IID_PPV_ARGS(ptrCompilerArgs.to())), "DxcCreateInstance(...) for IDxcCompilerArgs");

    // Craft commands
    EXPP_ASSERT(createCompilerArgs(ptrCompilerArgs, inputFile, outputFile, debugFile, type), "dx::ShaderCompiler::createCompilerArgs(...)");
    
    // Create default include header
    ScopedComPointer<IDxcIncludeHandler> ptrDefIncludeHeader;
    EVALUATE_HRESULT(ptrUtils->CreateDefaultIncludeHandler(ptrDefIncludeHeader.to()), "IDxcUtils->CreateDefaultIncludeHandler(...)");

    // Open source file
    ScopedComPointer<IDxcBlobEncoding> ptrSourceFile;
    EVALUATE_HRESULT(ptrUtils->LoadFile(inputFile, nullptr, ptrSourceFile.to()), "IDxcUtils->LoadFile(...)");

    // Extract buffer info
    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr = ptrSourceFile->GetBufferPointer();
    sourceBuffer.Size = ptrSourceFile->GetBufferSize();
    sourceBuffer.Encoding = DXC_CP_ACP;

    // Compile shader
    ScopedComPointer<IDxcResult> ptrCompilationResult;
    EVALUATE_HRESULT(ptrCompiler->Compile(&sourceBuffer, ptrCompilerArgs->GetArguments(), ptrCompilerArgs->GetCount(), ptrDefIncludeHeader, IID_PPV_ARGS(ptrCompilationResult.to())), "IDxcCompiler3->Compile(...)");

    // Retrive compilation status
    HRESULT compilationStatus = S_OK;
    EVALUATE_HRESULT(ptrCompilationResult->GetStatus(&compilationStatus), "IDxcResult->GetStatus(...)");

    // Check if failed
    if (FAILED(compilationStatus)) {
        // Check Extract error
        if (ptrCompilationResult->HasOutput(DXC_OUT_ERRORS)) {
            // Extract error output
            ScopedComPointer<IDxcBlobUtf8> ptrErrorBlob;
            EVALUATE_HRESULT(ptrCompilationResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(ptrErrorBlob.to()), NULL), "IDxcResult->GetOutput(...) for DXC_OUT_ERRORS");
            // Message box error
            if (ptrErrorBlob->GetStringLength() > 0) {
                MessageBoxA(NULL, ptrErrorBlob->GetStringPointer(), "Shader compilation error", MB_OK | MB_ICONERROR);
            }
        }

        // Failed
        return false;
    }

    // Retrive shader
    ScopedComPointer<IDxcBlob> ptrShaderBlob;
    EVALUATE_HRESULT(ptrCompilationResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(ptrShaderBlob.to()), NULL), "IDxcResult->GetOutput(...) for DXC_OUT_OBJECT");

    // Retrive debug
    ScopedComPointer<IDxcBlob> ptrDebugBlob;
    EVALUATE_HRESULT(ptrCompilationResult->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(ptrDebugBlob.to()), NULL), "IDxcResult->GetOutput(...) for DXC_OUT_PDB");

    // Safe shader
    ScopedHandle hFileShaderOutput = CreateFile(outputFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
    EXPP_ASSERT(hFileShaderOutput, "Cannot access shader output file");
    SetFilePointer(hFileShaderOutput, 0, NULL, FILE_BEGIN);
    EXPP_ASSERT(WriteFile(hFileShaderOutput, ptrShaderBlob->GetBufferPointer(), ptrShaderBlob->GetBufferSize(), NULL, NULL), "Cannor write shader output file");

    // Safe debug
    ScopedHandle hFileDebugOutput = CreateFile(debugFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
    EXPP_ASSERT(hFileDebugOutput, "Cannot access debug output file");
    SetFilePointer(hFileDebugOutput, 0, NULL, FILE_BEGIN);
    EXPP_ASSERT(WriteFile(hFileDebugOutput, ptrDebugBlob->GetBufferPointer(), ptrDebugBlob->GetBufferSize(), NULL, NULL), "Cannor write debug output file");

    // Passed
    return true;
}

bool dx::ShaderCompiler::createCompilerArgs(IDxcCompilerArgs* ptrArgs, LPCWSTR inputFile, LPCWSTR outputFile, LPCWSTR debugFile, SHADER_TYPE type) {
    // Add source filename
    EVALUATE_HRESULT(ptrArgs->AddArguments(&inputFile, 1), "IDxcCompilerArgs->AddArguments(...)");

    // Add edntry point and target switch
    if (type != SHADER_TYPE_ROOTSIGNATURE) {
        LPCWSTR entryArgs[] = { L"-E", L"main", L"-T" };
        EVALUATE_HRESULT(ptrArgs->AddArguments(entryArgs, 3), "IDxcCompilerArgs->AddArguments(...)");
    }
    else {
        LPCWSTR entryArgs[] = { L"-E", L"ROOTSIG", L"-T" };
        EVALUATE_HRESULT(ptrArgs->AddArguments(entryArgs, 3), "IDxcCompilerArgs->AddArguments(...)");
    }

    // Add target (Shader Modell 6.4)
    switch (type) {
        // Vertex Shader 
        case SHADER_TYPE_VERTEX: {
            LPCWSTR vsArg = L"vs_6_4";
            EVALUATE_HRESULT(ptrArgs->AddArguments(&vsArg, 1), "IDxcCompilerArgs->AddArguments(...)");
            break;
        }

        // Pixel Shader 
        case SHADER_TYPE_PIXEL: {
            LPCWSTR psArg = L"ps_6_4";
            EVALUATE_HRESULT(ptrArgs->AddArguments(&psArg, 1), "IDxcCompilerArgs->AddArguments(...)");
            break;
        }

        // Hull Shader 
        case SHADER_TYPE_HULL: {
            LPCWSTR hsArg = L"hs_6_4";
            EVALUATE_HRESULT(ptrArgs->AddArguments(&hsArg, 1), "IDxcCompilerArgs->AddArguments(...)");
            break;
        }

        // Domain Shader 
        case SHADER_TYPE_DOMAIN: {
            LPCWSTR dsArg = L"ds_6_4";
            EVALUATE_HRESULT(ptrArgs->AddArguments(&dsArg, 1), "IDxcCompilerArgs->AddArguments(...)");
            break;
        }

        // Pixel Shader 
        case SHADER_TYPE_ROOTSIGNATURE: {
            LPCWSTR rsArg = L"rootsig_1_0";
            EVALUATE_HRESULT(ptrArgs->AddArguments(&rsArg, 1), "IDxcCompilerArgs->AddArguments(...)");
            break;
        }

        // Default fallback
        default:
            return false;
    }

    // Debug info and next switch
    LPCWSTR debugArgs[] = { L"-Zi", L"-Fo" };
    EVALUATE_HRESULT(ptrArgs->AddArguments(debugArgs, 2), "IDxcCompilerArgs->AddArguments(...)");

    // Output name
    EVALUATE_HRESULT(ptrArgs->AddArguments(&outputFile, 1), "IDxcCompilerArgs->AddArguments(...)");

    // PDB Switch and file name
    LPCWSTR fdArg = L"-Fd";
    EVALUATE_HRESULT(ptrArgs->AddArguments(&fdArg, 1), "IDxcCompilerArgs->AddArguments(...)");
    EVALUATE_HRESULT(ptrArgs->AddArguments(&debugFile, 1), "IDxcCompilerArgs->AddArguments(...)");

    // Passed
    return true;
}
