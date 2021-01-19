#pragma once
#include <DefHeader.h>

#include <dx/setup/GIOutput.h>

namespace DX {
	/// <summary>
	/// DXGI Adapter Wrapper
	/// </summary>
	class GIAdapter : public ScopedComPointer<IDXGIAdapter>{
		public:
			/// <summary>
			/// Create empty adapter
			/// </summary>
			GIAdapter() noexcept;

			/// <summary>
			/// Create adapter
			/// </summary>
			/// <param name="ptrAdapter">Pointer to adapter no null allowed</param>
			GIAdapter(IDXGIAdapter* ptrAdapter) noexcept;

			/// <summary>
			/// Trys to find the the output for a given monitor (will return an invalid GIOuptut if GPU does not support this monitor)
			/// </summary>
			/// <param name="monitor">Input monitor handle to check for</param>
			/// <returns>GIOutput</returns>
			DX::GIOutput findMonitor(const HMONITOR monitor);

			/// <summary>
			/// Gets the current video memory info
			/// </summary>
			/// <param name="ptrMemory">Pointer to video memory info</param>
			/// <returns>true if pointer was set</returns>
			bool getVideoMemory(DXGI_QUERY_VIDEO_MEMORY_INFO* ptrMemory) noexcept;

			/// <summary>
			/// Sets the currently minimum video memory requirements. Requires an active D3D12Device
			/// </summary>
			/// <param name="reservatioMemoryInBytes">Video memory required in bytes</param>
			/// <returns>true if reservation succeded</returns>
			bool setVideoMemoryReservation(UINT64 reservatioMemoryInBytes) noexcept;

			/// <summary>
			/// Retrive description 0
			/// </summary>
			/// <param name="ptrDesc">Pointer to descritor</param>
			/// <returns>True if descrition could be fetched</returns>
			bool getDescription(DXGI_ADAPTER_DESC* ptrDesc) noexcept;

			/// <summary>
			/// Retrive description 1
			/// </summary>
			/// <param name="ptrDesc1">Pointer to descritor</param>
			/// <returns>True if descrition could be fetched</returns>
			bool getDescription(DXGI_ADAPTER_DESC1* ptrDesc1) noexcept;

			/// <summary>
			/// Retrive description 2
			/// </summary>
			/// <param name="ptrDesc2">Pointer to descritor</param>
			/// <returns>True if descrition could be fetched</returns>
			bool getDescription(DXGI_ADAPTER_DESC2* ptrDesc2) noexcept;

			/// <summary>
			/// Retrive description 3
			/// </summary>
			/// <param name="ptrDesc3">Pointer to descritor</param>
			/// <returns>True if descrition could be fetched</returns>
			bool getDescription(DXGI_ADAPTER_DESC3* ptrDesc3) noexcept;
	};
}