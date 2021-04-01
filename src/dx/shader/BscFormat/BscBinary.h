#pragma once

#include <DefHeader.h>

namespace BSC {
	/// <summary>
	/// Binary shader representation
	/// </summary>
	class Binary {
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			Binary() noexcept = default;

			/// <summary>
			/// Construct with data
			/// </summary>
			/// <param name="ptrRef">Pointer to refcounter</param>
			/// <param name="ptrBlob">Pointer to combined blob</param>
			/// <param name="shaderSize">Size of shader blob</param>
			/// <param name="debugSize">Size of debug blob</param>
			/// <param name="reflectionSize">Size of reflection blob</param>
			Binary(LONG64* ptrRef, void* ptrBlob, UINT64 shaderSize, UINT64 debugSize, UINT64 reflectionSize) noexcept;

			/// <summary>
			/// Copy constructor
			/// </summary>
			/// <param name="other">Other</param>
			Binary(const Binary& other) noexcept;

			/// <summary>
			/// Destructor
			/// </summary>
			~Binary() noexcept;

			/// <summary>
			/// Assign operator
			/// </summary>
			/// <param name="other">Other</param>
			Binary& operator=(const Binary& other) noexcept;

			/// <summary>
			/// Bool conversion operator
			/// </summary>
			operator bool() noexcept;

			/// <summary>
			/// Retrive shader pointer
			/// </summary>
			/// <param name="ppData">Pointer to target void*</param>
			/// <returns>Shader size</returns>
			UINT64 getShader(void** ppData) noexcept;

			/// <summary>
			/// Retrive debug pointer
			/// </summary>
			/// <param name="ppData">Pointer to target void*</param>
			/// <returns>Debug size</returns>
			UINT64 getDebug(void** ppData) noexcept;

			/// <summary>
			/// Retrive reflection pointer
			/// </summary>
			/// <param name="ppData">Pointer to target void*</param>
			/// <returns>Reflection size</returns>
			UINT64 getReflection(void** ppData) noexcept;

		private:
			/// <summary>
			/// Pointer to source refcounter
			/// </summary>
			LONG64* m_refCounter = nullptr;

			/// <summary>
			/// Shader pointer
			/// </summary>
			void* m_ptrShader = nullptr;

			/// <summary>
			/// Debug pointer
			/// </summary>
			void* m_ptrDebug = nullptr;

			/// <summary>
			/// Refelction pointer
			/// </summary>
			void* m_ptrReflection = nullptr;

			/// <summary>
			/// Size of shader
			/// </summary>
			UINT64 m_sizeShader = 0;

			/// <summary>
			/// Size of debug
			/// </summary>
			UINT64 m_sizeDebug = 0;
			
			/// <summary>
			/// Size of reflection
			/// </summary>
			UINT64 m_sizeReflection = 0;
	};
}
