#include "DefHeader.h"
#include "BscWriter.h"

bool BSC::Writer::writeFile(LPCWSTR basePath, BSC_SHADER_INFO* ptrInfo, XXH128_hash_t sourceChecksum, void* ptrShaderData, UINT64 shaderDataLength, void* ptrDebugData, UINT64 debugDataLength, void* ptrReflectionData, UINT64 reflectionDataLength) {
	EXPP_ASSERT_DBG(basePath, "Invalid target base path");
	EXPP_ASSERT_DBG(ptrInfo, "Invalid info pointer");
	if (shaderDataLength) {
		EXPP_ASSERT(ptrShaderData, "Invalid shader data");
	}
	if (debugDataLength) {
		EXPP_ASSERT(ptrDebugData, "Invalid debug data");
	}
	if (reflectionDataLength) {
		EXPP_ASSERT(ptrReflectionData, "Invalid reflection data");
	}
	
	// Check if slash is required
	SIZE_T baseLength = wcslen(basePath);
	BOOL slashAppend = ((basePath[baseLength - 1] == L'\\') || (basePath[baseLength - 1] == L'/'));

	// Create shader hash
	UINT64 shaderLUID = XXH3_64bits((void*)ptrInfo->shaderName, (strlen(ptrInfo->shaderName) + 1) * sizeof(CHAR));

	// Build filename
	std::wstringstream wss;
	wss << basePath << (slashAppend ? L"\\" : L"") << std::hex << shaderLUID << std::dec << L".bsc";
	const std::wstring targetFileName = wss.str();

	// Open file
	ScopedHandle hFile = CreateFile(targetFileName.c_str(), GENERIC_WRITE | GENERIC_READ, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
	if (!hFile) {
		return false;
	}

	// Set file pointer to begin
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	// Write header
	const LPCSTR headerText = "BSC-V-0-1-0\n";
	EXPP_ASSERT(WriteFile(hFile, headerText, 12, NULL, NULL), "Failed to write BSC Header");

	// Write shader name and new line
	EXPP_ASSERT(WriteFile(hFile, ptrInfo->shaderName, (strlen(ptrInfo->shaderName)) * sizeof(CHAR), NULL, NULL), "Failed to write BSC Shadername");
	EXPP_ASSERT(WriteFile(hFile, "\n", 1, NULL, NULL), "Failed to write BSC Shadername new line");

	// Craft BSC Meta
	_BSC_FILE_META meta;
	meta.targetShaderModell = ((ptrInfo->target.majorVersion & 0x0F) << 4)| (ptrInfo->target.minorVersion & 0x0F);
	meta.targetShader = (UINT8)ptrInfo->target.type;
	meta.sourceHashHighByte = sourceChecksum.high64;
	meta.sourceHashLowByte = sourceChecksum.low64;
	meta.encodingType = (UINT8)ptrInfo->encoding;
	meta.finalDataSize = shaderDataLength + debugDataLength + reflectionDataLength;
	meta.shaderBytecodeLen = shaderDataLength;
	meta.shaderDebugInfoLen = debugDataLength;
	meta.shaderReflectionInfoLen = reflectionDataLength;

	// Write meta
	EXPP_ASSERT(WriteFile(hFile, &meta, sizeof(_BSC_FILE_META), NULL, NULL), "Failed to write BSC Info header");

	// Assemble binary data
	switch (ptrInfo->encoding) {
		// Direct copy
		case BSC_ENCODING_BINARY: {
			// Shader data
			if (ptrShaderData && shaderDataLength) {
				EXPP_ASSERT(WriteFile(hFile, ptrShaderData, shaderDataLength, NULL, NULL), "Failed to write shader bytecode");
			}
			// Debug data
			if (ptrDebugData && debugDataLength) {
				EXPP_ASSERT(WriteFile(hFile, ptrDebugData, debugDataLength, NULL, NULL), "Failed to write shader debig info");
			}
			// Reflection data
			if (ptrReflectionData && reflectionDataLength) {
				EXPP_ASSERT(WriteFile(hFile, ptrReflectionData, reflectionDataLength, NULL, NULL), "Failed to write shader bytecode");
			}
			break;
		}

		// Encode LZ4
		case BSC_ENCODING_BINARY_LZ4_DEFAULT: {
			// Check input size
			if (shaderDataLength + debugDataLength + reflectionDataLength > LZ4_MAX_INPUT_SIZE) {
				return false;
			}

			// Get output size and allocate it
		 	int lz4Size = LZ4_compressBound((int)(shaderDataLength + debugDataLength + reflectionDataLength));
			void* lz4Target = malloc(lz4Size);
			if (lz4Target == nullptr) {
				EXPP_ASSERT(false, "Failed to allocate LZ4 Output buffer");
				return false;
			}

			// Create input buffer
			void* lz4source = malloc(shaderDataLength + debugDataLength + reflectionDataLength);
			if (lz4source == nullptr) {
				free(lz4Target);
				EXPP_ASSERT(false, "Failed to allocate LZ4 Input buffer");
				return false;
			}

			// Copy data to source
			UINT64 srcOffset = 0;
			// Shader
			if (shaderDataLength) {
				memcpy(lz4source, ptrShaderData, shaderDataLength);
				srcOffset += shaderDataLength;
			}
			// Debug
			if (debugDataLength) {
				void* dbgSrc = (void*)(((UINT64) lz4source) + srcOffset);
				memcpy(dbgSrc, ptrDebugData, debugDataLength);
				srcOffset += debugDataLength;
			}
			// Reflection
			if (reflectionDataLength) {
				void* refSrc = (void*)(((UINT64)lz4source) + srcOffset);
				memcpy(refSrc, ptrReflectionData, reflectionDataLength);
			}

			// LZ4 Compress
			int lz4CompressedSize = LZ4_compress_default((char*)lz4source, (char*)lz4Target, shaderDataLength + debugDataLength + reflectionDataLength, lz4Size);

			// Assert size
			if (lz4CompressedSize == 0) {
				free(lz4source);
				free(lz4Target);
				return false;
			}

			// Write outpur file
			if (!WriteFile(hFile, lz4Target, lz4CompressedSize, NULL, NULL)) {
				free(lz4source);
				free(lz4Target);
				return false;
			}

			// Free buffer
			free(lz4source);
			free(lz4Target);
			break;
		}

	}

	// Passed
	return true;
}

bool BSC::Writer::readFile(LPCWSTR basePath, LPCSTR shaderName, BSC_SHADER_INFO* ptrInfo, XXH128_hash_t* ptrSourceChecksum, void** ppBlob, UINT64* ptrShaderDataLength, UINT64* ptrDebugDataLength, UINT64* ptrReflectionDataLength) {
	// Check if slash is required
	SIZE_T baseLength = wcslen(basePath);
	BOOL slashAppend = ((basePath[baseLength - 1] == L'\\') || (basePath[baseLength - 1] == L'/'));

	// Create shader hash
	UINT64 shaderLUID = XXH3_64bits(shaderName, (strlen(shaderName) + 1) * sizeof(CHAR));

	// Build filename
	std::wstringstream wss;
	wss << basePath << (slashAppend ? L"\\" : L"") << std::hex << shaderLUID << std::dec << L".bsc";
	const std::wstring targetFileName = wss.str();

	// Open file
	ScopedHandle hFile = CreateFile(targetFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (!hFile) {
		return false;
	}

	// Set file pointer to begin
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	// Get file size
	const DWORD fileSize = GetFileSize(hFile, NULL);
	
	// Check file size
	if (fileSize < (12 + strlen(shaderName) + 1 + sizeof(_BSC_FILE_META))) {
		return false;
	}

	// Check file header
	CHAR fileHeader[12];
	EXPP_ASSERT(ReadFile(hFile, fileHeader, 12, NULL, NULL), "Read of BSC Header failed");
	if (memcmp(fileHeader, "BSC-V-0-1-0\n", 12) != 0) {
		return false;
	}

	// Read file name
	CHAR shaderNameIn[MAX_PATH];
	EXPP_ASSERT(ReadFile(hFile, shaderNameIn, strlen(shaderName) + 1, NULL, NULL), "Read of BSC shader name failed");
	if (memcmp(shaderNameIn, shaderName, strlen(shaderName)) != 0) {
		return false;
	}

	// Read Info
	_BSC_FILE_META meta;
	EXPP_ASSERT(ReadFile(hFile, &meta, sizeof(_BSC_FILE_META), NULL, NULL), "Read of BSC meta failed");
	
	// Vertify meta
	if (meta.targetShader < 0x01 || meta.targetShader > 0x08) {
		return false;
	}
	if (meta.encodingType != 0x10 && meta.encodingType != 0x20) {
		return false;
	}
	if (meta.finalDataSize != meta.shaderBytecodeLen + meta.shaderDebugInfoLen + meta.shaderReflectionInfoLen) {
		return false;
	}

	// Allocate final data and 
	*ppBlob = malloc(meta.finalDataSize);
	EXPP_ASSERT(*ppBlob, "Failed to allocate target data");

	// Decode based on encoding
	switch ((BSC_ENCODING)meta.encodingType) {
		// BINARY
		case BSC_ENCODING_BINARY: {
			// Read all
			EXPP_ASSERT(ReadFile(hFile, *ppBlob, meta.finalDataSize, NULL, NULL), "Binary read failed");
			break;
		};

		// BINARY LZ4
		case BSC_ENCODING_BINARY_LZ4_DEFAULT: {
			// Compute LZ4 source size
			DWORD filePos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			const DWORD lz4SourceSize = fileSize - filePos;

			// Allocate and read LZ4 data
			void* lz4Source = malloc(lz4SourceSize);
			EXPP_ASSERT(lz4Source, "Failed to allocate temporary buffer");
			if (!ReadFile(hFile, lz4Source, lz4SourceSize, NULL, NULL)) {
				free(lz4Source);
				EXPP_ASSERT(false, "Failed to read binary data");
				return false;
			}

			// LZ4 Decompress
			int lz4DecompressSize = LZ4_decompress_safe((char*)lz4Source, (char*)*ppBlob, lz4SourceSize, meta.finalDataSize);
			free(lz4Source);
			if(lz4DecompressSize != meta.finalDataSize){
				return false;
			}

			break;
		};
	}

	// Set other params
	*ptrShaderDataLength = meta.shaderBytecodeLen;
	*ptrDebugDataLength = meta.shaderDebugInfoLen;
	*ptrReflectionDataLength = meta.shaderReflectionInfoLen;
	ptrSourceChecksum->high64 = meta.sourceHashHighByte;
	ptrSourceChecksum->low64 = meta.sourceHashLowByte;
	ptrInfo->encoding = (BSC_ENCODING)meta.encodingType;
	ptrInfo->shaderName = shaderName;
	ptrInfo->target.type = (BSC_SHADER_TYPE)meta.targetShader;
	ptrInfo->target.minorVersion = (meta.targetShaderModell & 0x0F);
	ptrInfo->target.majorVersion = (meta.targetShaderModell & 0xF0) >> 4;

	// Passed
	return true;
}
