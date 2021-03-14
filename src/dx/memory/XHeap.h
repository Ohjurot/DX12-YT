#pragma once
#include <DefHeader.h>

#include <dx/cmds/XFence.h>
#include <dx/cmds/FenceCounter.h>

namespace DX {
	/// <summary>
	/// Device pointer
	/// </summary>
	class XHeap : public ScopedComPointer<ID3D12Heap> {
		public:
			/// <summary>
			/// Default empty object
			/// </summary>
			XHeap() = default;

			/// <summary>
			/// Move construct
			/// </summary>
			/// <param name="other">other heap</param>
			XHeap(XHeap&& other) noexcept;

			/// <summary>
			/// Create inited heap
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="size">Size of heap</param>
			/// <param name="type">Heap type</param>
			/// <param name="flags">Heap flags (D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT and D3D12_HEAP_FLAG_CREATE_NOT_ZEROED will always be set)</param>
			/// <param name="allignement">Heap allignment</param>
			XHeap(ID3D12Device* ptrDevice, UINT64 size, D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE, UINT64 allignement = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

			/// <summary>
			/// Release function
			/// </summary>
			void release() override;

			/// <summary>
			/// Makes the heap resident
			/// </summary>
			/// <param name="refWaitObject">Wait object to set</param>
			/// <returns>Result of call</returns>
			HRESULT makeResident(XFence::WaitObject& refWaitObject);

			/// <summary>
			/// Evicts the heap from GPU memory
			/// </summary>
			/// <returns>Result of call</returns>
			HRESULT evict();

			/// <summary>
			/// Return the size of the heao
			/// </summary>
			/// <returns>Size of heap in bytes</returns>
			UINT64 getSize() noexcept;

			/// <summary>
			/// Assign operator
			/// </summary>
			XHeap& operator=(XHeap&& other) noexcept;
		private:
			/// <summary>
			/// Pointer to device
			/// </summary>
			ScopedComPointer<ID3D12Device3> m_ptrDevice;

			/// <summary>
			/// Heap fence
			/// </summary>
			XFence m_fence;

			/// <summary>
			/// Counter for fence
			/// </summary>
			FenceCounter m_fenceCounter;

			/// <summary>
			/// Fence counter frontend for memory residency
			/// </summary>
			FenceCounter::Frontend m_counterFrontent;

			/// <summary>
			/// Indicates weather or not the heap is resident
			/// </summary>
			bool m_bResident = false;

			/// <summary>
			/// Size of heap
			/// </summary>
			UINT64 m_size = 0;
	};
}