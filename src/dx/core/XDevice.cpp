#include "XDevice.h"

DX::XDevice::XDevice(IDXGIAdapter* ptrAdapter) {
	// Create minimal device
	ScopedComPointer<ID3D12Device> ptrTempDevice;
	EVALUATE_HRESULT(D3D12CreateDevice(ptrAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(ptrTempDevice.to())), "Your GPU does not support DirectX-FL 11.0!\nFL11.0 is required to run this application!\nD3D12CreateDevice(...)");

	// Check highst feature support
	D3D_FEATURE_LEVEL reqArray[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_2,
	};

	// Create request descriptor
	D3D12_FEATURE_DATA_FEATURE_LEVELS flds;
	ZeroMemory(&flds, sizeof(D3D12_FEATURE_DATA_FEATURE_LEVELS));

	// Fill descritor
	flds.pFeatureLevelsRequested = reqArray;
	flds.NumFeatureLevels = _countof(reqArray);

	// Execute feature request
	EVALUATE_HRESULT(ptrTempDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &flds, sizeof(D3D12_FEATURE_DATA_FEATURE_LEVELS)), "ID3D12Device->CheckFeatureSupport(...) for D3D12_FEATURE_FEATURE_LEVELS");

	// Create real device on highst level
	EVALUATE_HRESULT(D3D12CreateDevice(ptrAdapter, flds.MaxSupportedFeatureLevel, IID_PPV_ARGS(to())), "D3D12CreateDevice(...)");
	m_featureLevel = flds.MaxSupportedFeatureLevel;
}

D3D_FEATURE_LEVEL DX::XDevice::getDeviceFeaturLevel() {
	return m_featureLevel;
}
