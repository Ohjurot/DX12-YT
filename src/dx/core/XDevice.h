#pragma once
#include <DefHeader.h>

namespace DX {
	/// <summary>
	/// Device pointer
	/// </summary>
	class XDevice : public ScopedComPointer<ID3D12Device> {
		public:
			/// <summary>
			/// Default empty object
			/// </summary>
			XDevice() = default;

			/// <summary>
			/// Create device from adapter
			/// </summary>
			/// <param name="ptrAdapter">Pointer to adapter</param>
			XDevice(IDXGIAdapter* ptrAdapter);

			/// <summary>
			/// Retrive the max supported feature level (device is created on that)
			/// </summary>
			/// <returns>Feature level</returns>
			D3D_FEATURE_LEVEL getDeviceFeaturLevel();

			/// <summary>
			/// Check for a feature on the GPU
			/// </summary>
			/// <typeparam name="T">Type of request parameter</typeparam>
			/// <param name="feature">Feature to query</param>
			/// <param name="data">Data reference</param>
			/// <returns>True if query succeeds</returns>
			template<typename T>
			bool getFeatureSupport(D3D12_FEATURE feature, T& data) {
				// Check if pointer is valid
				if (!m_comPointer) {
					return false;
				}

				// Return query result
				return SUCCEEDED(m_comPointer->CheckFeatureSupport(feature, &data, sizeof(T)));
			}

		private:
			/// <summary>
			/// Feature level of the device
			/// </summary>
			D3D_FEATURE_LEVEL m_featureLevel;
	};
}