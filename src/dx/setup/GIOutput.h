#pragma once
#include <DefHeader.h>

namespace DX {
	/// <summary>
	/// DXGI Output wrapper
	/// </summary>
	class GIOutput : public ScopedComPointer<IDXGIOutput>{
		public:
			/// <summary>
			/// Create empty object
			/// </summary>
			GIOutput() noexcept;

			/// <summary>
			/// Create Output
			/// </summary>
			/// <param name="ptrOutput">Pointer to output. Null not allowed</param>
			GIOutput(IDXGIOutput* ptrOutput) noexcept;

			/// <summary>
			/// Retrive the output description 0 
			/// </summary>
			/// <param name="ptrDesc">Pointer to descriptor</param>
			/// <returns>true if description could be fetched</returns>
			bool getDescription(DXGI_OUTPUT_DESC* ptrDesc) noexcept;

			/// <summary>
			/// Retrive the output description 1
			/// </summary>
			/// <param name="ptrDesc1">Pointer to descriptor</param>
			/// <returns>true if description could be fetched</returns>
			bool getDescription(DXGI_OUTPUT_DESC1* ptrDesc1) noexcept;
	};
}