#pragma once

#include <DefHeader.h>
#include <xxhash.h>

#include <common/File/FileHash.h>
#include <dx/shader/ShaderTypes.h>
#include <dx/shader/ShaderCompiler.h>

#define SHADER_SOURCE_FOLDER L"./source/shader/"
#define SHADER_TARGET_FOLDER L"./shader/"

namespace dx {
	/// <summary>
	/// Shader class
	/// </summary>
	class Shader {
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			Shader() = default;

			/// <summary>
			/// Copy constructor
			/// </summary>
			Shader(const Shader& other);

			/// <summary>
			/// Move constructor
			/// </summary>
			Shader(Shader&& other) noexcept;

			/// <summary>
			/// Create shader based on name and type
			/// </summary>
			/// <param name="name">Name of shader</param>
			/// <param name="type">Name of type</param>
			Shader(LPCWSTR name, SHADER_TYPE type);

			/// <summary>
			/// Destructor
			/// </summary>
			~Shader();

			/// <summary>
			/// Assign operator
			/// </summary>
			Shader& operator=(const Shader& other);

			/// <summary>
			/// Set buffer details
			/// </summary>
			/// <param name="ppVoid">Pointer to target void pointer</param>
			/// <param name="ptrSize">Pointer to target size</param>
			/// <returns>true on succeed</returns>
			bool setBuffer(void** ppVoid, UINT64* ptrSize) noexcept;

			/// <summary>
			/// Reload the shader
			/// </summary>
			void reload();

			/// <summary>
			/// Checks weather the shader is uptodata
			/// </summary>
			/// <returns>true if loaded shader is the most recent version</returns>
			bool isUptodata();

		private:
			/// <summary>
			/// Name of the shader
			/// </summary>
			WCHAR m_name[MAX_PATH] = {};

			/// <summary>
			/// Type of shader
			/// </summary>
			SHADER_TYPE m_type = SHADER_TYPE_INVALID;

			/// <summary>
			/// Pointer to blob
			/// </summary>
			void* m_ptrBlob = nullptr;

			/// <summary>
			/// Size of blob
			/// </summary>
			UINT64 m_size = 0;
	};
}
