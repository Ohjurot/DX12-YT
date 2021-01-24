#pragma once
#include <DefHeader.h>

namespace DX {
	/// <summary>
	/// D3D12 Debug singelton
	/// </summary>
	class XDebug : public ScopedComPointer<ID3D12Debug> {
		// Singelton	
		public:
			/// <summary>
			/// Retrive singelton instance
			/// </summary>
			/// <returns>Instance of XDebug</returns>
			static XDebug& getInstance();

		// Class 
		public:
			/// <summary>
			/// Validate existance of the debug interface / create
			/// </summary>
			void validate();

			/// <summary>
			/// Enable debug interface
			/// </summary>
			void enable();

			/// <summary>
			/// Return weather the debug layer is enabled
			/// </summary>
			/// <returns>true if enabled</returns>
			bool isEnabled() noexcept;

			/// <summary>
			/// Sets the GPU based validation (CALL BEVORE DEVICE CREATION! REQUIRES DEVICE RECREATION)
			/// </summary>
			/// <param name="enabled">Enabled state of GPU based validation</param>
			/// <param name="flags">Option flags</param>
			void setGpuBasedValidation(bool enabled, D3D12_GPU_BASED_VALIDATION_FLAGS flags = D3D12_GPU_BASED_VALIDATION_FLAGS_NONE);

			/// <summary>
			/// Enabled or disabled Synchronized Command-Queue Validation
			/// </summary>
			/// <param name="enabled">If enabled or disabled</param>
			void setEnableSyncCommandQueueValidation(bool enabled = true);

		// Singelton
		private:
			/// <summary>
			/// Indicates that the debug layer is enabled
			/// </summary>
			bool m_bIsEnabled = false;

			/// <summary>
			/// Create empty adapter
			/// </summary>
			XDebug() noexcept {};

			/// <summary>
			/// Static instance of debug interface
			/// </summary>
			static XDebug s_instance;

			// Delete copy and assign
			XDebug(const XDebug&) = delete;
			void operator=(const XDebug&) = delete;
	};
}
