#include "GIFactory.h"

DX::GIFactory::GIFactory() :
	ScopedComPointer<IDXGIFactory>()
{
	// Create DXGI Factory
	EVALUATE_HRESULT(CreateDXGIFactory(IID_PPV_ARGS(&m_comPointer)), "CreateDXGIFactory(...)");
}

DX::GIAdapter DX::GIFactory::getAdapter(LUID preferedAdapterLuid){
	// Output adapter buffer
	ScopedComPointer<IDXGIAdapter> outputAdapter;

	// Get adapter by LUID if specified
	LUID nullLuid = {};
	if (memcmp(&preferedAdapterLuid, &nullLuid, sizeof(LUID)) != 0) {
		// Get factory 4
		ScopedComPointer<IDXGIFactory4> ptrFactory4;
		if (queryInterface(ptrFactory4)) {
			// Enum adpater by LUID
			HRESULT hr = ptrFactory4->EnumAdapterByLuid(preferedAdapterLuid, IID_PPV_ARGS(outputAdapter.to()));
			// Check result
			switch (hr) {
				// If succeed return adpter and end
				case S_OK:
					return GIAdapter(outputAdapter);
					break;

				// If not found proceed to default adpter
				case DXGI_ERROR_NOT_FOUND:
					break;

				// Else throw an error
				default:
					throw EXPP_HRESULT(hr, "IDXGIFactory4->EnumAdapterByLuid(...)");
					break;
			}
		}
	}

	// Find performant adpater
	ScopedComPointer<IDXGIFactory6> ptrFactory6;
	if (queryInterface(ptrFactory6)) {
		// Enum High performance apter
		HRESULT hr = ptrFactory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(outputAdapter.to()));
		// Check result
		switch (hr) {
			// If succeed return adpter and end
			case S_OK:
				return GIAdapter(outputAdapter);
				break;

			// If not found proceed to default adpter
			case DXGI_ERROR_NOT_FOUND:
				break;

			// Else throw an error
			default:
				throw EXPP_HRESULT(hr, "IDXGIFactory6->EnumAdapterByGpuPreference(...)");
				break;
		}
	}

	// Return default adapter
	EVALUATE_HRESULT(m_comPointer->EnumAdapters(0, outputAdapter.to()), "No default GPU availible!\nIDXGIFactory->EnumAdapters(...)");
	return GIAdapter(outputAdapter);
}

bool DX::GIFactory::checkTearingSupport() {
	// Temporary factory 5
	ScopedComPointer<IDXGIFactory5> ptrFactory5;
	
	// If internal pointer is not valid or IDXGIFactory5 is not supported return false
	if (!queryInterface(ptrFactory5)) {
		return false;
	}

	// Query support
	BOOL allowTearingFlag = FALSE;
	EVALUATE_HRESULT(ptrFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearingFlag, sizeof(BOOL)), "IDXGIFactory5->CheckFeatureSupport(...)");

	// Return flag
	return allowTearingFlag;
}
