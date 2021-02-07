#include "DefHeader.h"
#include "XDebugDevice.h"

DX::XDebugDevice::XDebugDevice(ID3D12Device* ptrDevice) {
	// Query device for debug device (only if debug is enabled)
	if (DX::XDebug::getInstance().isEnabled()) {
		EVALUATE_HRESULT(ptrDevice->QueryInterface(IID_PPV_ARGS(to())), "ID3D12Device->QueryInterface(...) for ID3D12DebugDevice");
	}
}

D3D12_DEBUG_FEATURE DX::XDebugDevice::getFeatureMask() noexcept {
	// Get feature mask 
	D3D12_DEBUG_FEATURE featurMask = (D3D12_DEBUG_FEATURE)NULL;

	// Query for ID3D12DebugDevice1
	ScopedComPointer<ID3D12DebugDevice1> ptrDebugDevice1;
	if (queryInterface(ptrDebugDevice1)) {
		ptrDebugDevice1->GetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_FEATURE_FLAGS, &featurMask, sizeof(D3D12_DEBUG_FEATURE));
	}

	// Return feature mask
	return featurMask;
}

bool DX::XDebugDevice::setFeatureMask(D3D12_DEBUG_FEATURE featureMask) noexcept {
	// Query for ID3D12DebugDevice1 else return false
	ScopedComPointer<ID3D12DebugDevice1> ptrDebugDevice1;
	if (!queryInterface(ptrDebugDevice1)) {
		return false;
	}
	
	// If pointer exists return call result else return false
	return SUCCEEDED(ptrDebugDevice1->SetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_FEATURE_FLAGS, &featureMask, sizeof(D3D12_DEBUG_FEATURE)));
}

bool DX::XDebugDevice::getGpuValidatorInfo(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS* ptrInfo) noexcept {
	// Query for ID3D12DebugDevice1 return false if failed
	ScopedComPointer<ID3D12DebugDevice1> ptrDebugDevice1;
	if (!queryInterface(ptrDebugDevice1)) {
		return false;
	}

	// Return query result
	return SUCCEEDED(ptrDebugDevice1->GetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_GPU_BASED_VALIDATION_SETTINGS, ptrInfo, sizeof(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS)));
}

bool DX::XDebugDevice::setGpuValidatorInfo(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS* ptrInfo) noexcept {
	// Query for ID3D12DebugDevice1 return false if failed
	ScopedComPointer<ID3D12DebugDevice1> ptrDebugDevice1;
	if (!queryInterface(ptrDebugDevice1)) {
		return false;
	}

	// Return query result
	return SUCCEEDED(ptrDebugDevice1->SetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_GPU_BASED_VALIDATION_SETTINGS, ptrInfo, sizeof(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS)));
}

float DX::XDebugDevice::getDebugSlowdownFactor() noexcept {
	// Query for ID3D12DebugDevice1 return 0.0f if failed
	ScopedComPointer<ID3D12DebugDevice1> ptrDebugDevice1;
	if (!queryInterface(ptrDebugDevice1)) {
		return 0.0f;
	}

	// Create empty struct
	D3D12_DEBUG_DEVICE_GPU_SLOWDOWN_PERFORMANCE_FACTOR qData;
	qData.SlowdownFactor = 0.0f;
	
	// If query succeeds return query value else return 0.0f
	return (
		SUCCEEDED(ptrDebugDevice1->GetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_GPU_SLOWDOWN_PERFORMANCE_FACTOR, &qData, sizeof(D3D12_DEBUG_DEVICE_GPU_SLOWDOWN_PERFORMANCE_FACTOR))) 
		? qData.SlowdownFactor : 0.0f
	);
}
