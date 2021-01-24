#pragma once
#include <DefHeader.h>

#include <dx/core/XDebug.h>

namespace DX {
	/// <summary>
	/// Device pointer
	/// </summary>
	class XDebugDevice : public ScopedComPointer<ID3D12DebugDevice> {
		public:
			/// <summary>
			/// Default empty debug device
			/// </summary>
			XDebugDevice() = default;

			/// <summary>
			/// Create debugdevice from adapter
			/// </summary>
			/// <param name="ptrDevice">Pointer to device</param>
			XDebugDevice(ID3D12Device* ptrDevice);

			/// <summary>
			/// Retrive the debug feature mask
			/// </summary>
			/// <returns>Feature mask (eqals null if failed)</returns>
			D3D12_DEBUG_FEATURE getFeatureMask() noexcept;

			/// <summary>
			/// Sets the debug feature mask
			/// </summary>
			/// <param name="featureMask">Input mask</param>
			/// <returns>returns true if succeeded</returns>
			bool setFeatureMask(D3D12_DEBUG_FEATURE featureMask) noexcept;

			/// <summary>
			/// Retrives the GPU Validator info
			/// </summary>
			/// <param name="ptrInfo">Pointer to info struct (will be set)</param>
			/// <returns>true if succeeded</returns>
			bool getGpuValidatorInfo(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS* ptrInfo) noexcept;

			/// <summary>
			/// Sets the GPU Validator info
			/// </summary>
			/// <param name="ptrInfo">Pointer to info struct (will be read)</param>
			/// <returns>true if succeeded</returns>
			bool setGpuValidatorInfo(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS* ptrInfo) noexcept;

			/// <summary>
			/// Retrives the debug device slowdown factor
			/// </summary>
			/// <param name="ptrInfo">Pointer to info struct (will be set)</param>
			/// <returns>factor or 0.0f if failed or invalid</returns>
			float getDebugSlowdownFactor() noexcept;
	};
}