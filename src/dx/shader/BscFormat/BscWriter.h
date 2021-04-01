#pragma once

#include <DefHeader.h>
#include <string>
#include <sstream>

#include <dx/shader/BscFormat/BscConfig.h>
#include <xxhash.h>
#include <xxh3.h>
#include <lz4.h>

namespace BSC {
	/// <summary>
	/// BSC File Writer
	/// </summary>
	class Writer {
		public:
			/// <summary>
			/// Write a BSC File
			/// </summary>
			/// <param name="basePath">Directory for target file</param>
			/// <param name="ptrInfo">Pointer to related info struct</param>
			/// <param name="sourceChecksum">Checksum of source file</param>
			/// <param name="ptrShaderData">Pointer to shader data</param>
			/// <param name="shaderDataLength">Size of shader blob</param>
			/// <param name="ptrDebugData">Pointer to debug buffer</param>
			/// <param name="debugDataLength">Size of debug blob</param>
			/// <param name="ptrReflectionData">Pointer to reflection blob</param>
			/// <param name="reflectionDataLength">Size of reflection blob</param>
			/// <returns>true if write succeeds</returns>
			static bool writeFile(LPCWSTR basePath, BSC_SHADER_INFO* ptrInfo, XXH128_hash_t sourceChecksum, void* ptrShaderData, UINT64 shaderDataLength, void* ptrDebugData, UINT64 debugDataLength, void* ptrReflectionData, UINT64 reflectionDataLength);
	
			/// <summary>
			/// Read a BSC File
			/// </summary>
			/// <param name="basePath">Base path</param>
			/// <param name="shaderName">Name of shader to load</param>
			/// <param name="ptrInfo">Pointer to target info struct</param>
			/// <param name="ptrSourceChecksum">Pointer to target source hashsum</param>
			/// <param name="ppBlob">Pointer to output blob</param>
			/// <param name="ptrShaderDataLength">Pointer to shader data length</param>
			/// <param name="ptrDebugDataLength">Pointer to debug deta length</param>
			/// <param name="ptrReflectionDataLength">Pointer to reflection data length</param>
			/// <returns>true on succeed</returns>
			static bool readFile(LPCWSTR basePath, LPCSTR shaderName, BSC_SHADER_INFO* ptrInfo, XXH128_hash_t* ptrSourceChecksum, void** ppBlob, UINT64* ptrShaderDataLength, UINT64* ptrDebugDataLength, UINT64* ptrReflectionDataLength);

		private:
			/// <summary>
			/// File Meta info
			/// </summary>
			#pragma pack(push,1)
			struct _BSC_FILE_META {
				UINT8 targetShaderModell;
				UINT8 targetShader;
				UINT8 encodingType;
				UINT64 sourceHashLowByte;
				UINT64 sourceHashHighByte;
				UINT64 finalDataSize;
				UINT64 shaderBytecodeLen;
				UINT64 shaderDebugInfoLen;
				UINT64 shaderReflectionInfoLen;
			};
			#pragma pack(pop)
	};
}