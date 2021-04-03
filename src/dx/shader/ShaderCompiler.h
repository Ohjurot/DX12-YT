#pragma once

#include <DefHeader.h>
#include <dxc/dxcapi.h>
#include <dxc/d3d12shader.h>

#include <dx/shader/ShaderTypes.h>

namespace dx {
	/// <summary>
	/// Shader compiler
	/// </summary>
	class ShaderCompiler {
		public:
			/// <summary>
			/// Compiles a shader from source to binary
			/// </summary>
			/// <param name="inputFile">Input file path</param>
			/// <param name="outputFile">Output file path</param>
			/// <param name="debugFile">Output debug file path</param>
			/// <param name="type">Type of shader</param>
			/// <returns>true on succeess</returns>
			static bool compileShader(LPCWSTR inputFile, LPCWSTR outputFile, LPCWSTR debugFile, SHADER_TYPE type);

			/// <summary>
			/// Create compiler arguments from input
			/// </summary>
			/// <param name="ptrArgs">Pointer to target args object</param>
			/// <param name="inputFile">Input file name</param>
			/// <param name="outputFile">Output file name</param>
			/// <param name="debugFile">Debug file name</param>
			/// <param name="type">Shader type</param>
			/// <returns>True on succeed</returns>
			static bool createCompilerArgs(IDxcCompilerArgs* ptrArgs, LPCWSTR inputFile, LPCWSTR outputFile, LPCWSTR debugFile, SHADER_TYPE type);
	};
}
