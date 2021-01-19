#pragma once
#include <DefHeader.h>

#include <dx/setup/GIAdapter.h>

namespace DX {
	/// <summary>
	/// DXGI Factory wrapper
	/// </summary>
	class GIFactory : public ScopedComPointer<IDXGIFactory>{
		public:
			/// <summary>
			/// Creates a new factory
			/// </summary>
			GIFactory();

			/// <summary>
			/// Retrive the desired adapter. Will output preferedAdapter if preferedAdapterLuid is given and valid. Will give most performant or default adapter instead.
			/// </summary>
			/// <param name="preferedAdapterLuid">LUID of prevered adapter</param>
			/// <returns>Pointer to adapter</returns>
			DX::GIAdapter getAdapter(LUID preferedAdapterLuid = {});

			/// <summary>
			/// Check if tearing is supported
			/// </summary>
			/// <returns>true if allowed</returns>
			bool checkTearingSupport();
	};
}