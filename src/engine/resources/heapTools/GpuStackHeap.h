#pragma once

#include <DefHeader.h>

#include <common/MT/Locks/SpinLock.h>
#include <dx/memory/XHeap.h>
#include <engine/resources/HeapTools/HeapAllocation.h>

namespace engine {
	/// <summary>
	/// GPU Heap stack allocator
	/// </summary>
	class GpuStackHeap {
		public:
			/// <summary>
			/// Create gpu stack heap
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="size">Size of heap</param>
			/// <param name="type">Type of heap</param>
			/// <param name="flags">Heap flags</param>
			/// <param name="allignement">Alligment of heap</param>
			GpuStackHeap(ID3D12Device* ptrDevice, UINT64 size, D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE, UINT64 allignement = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

			/// <summary>
			/// Destructor
			/// </summary>
			~GpuStackHeap() noexcept;

			/// <summary>
			/// Release function
			/// </summary>
			void release() noexcept;

			/// <summary>
			/// Name the object
			/// </summary>
			/// <param name="name">Name of the object</param>
			void name(LPCWSTR name) noexcept;

			/// <summary>
			/// Allocate memory from the heap
			/// </summary>
			/// <param name="allocInfo">Reference to allocation info to be set</param>
			/// <param name="size">Size requested from heap (Multiple of 64KiB)</param>
			/// <returns>True if allocation succeded</returns>
			bool alloc(HEAP_ALLOCATION& allocInfo, UINT64 size);

		private:
			/// <summary>
			/// Class lock
			/// </summary>
			MT::SpinLock<THREAD_PAUSE_FUNCTION_F> m_lock;

			/// <summary>
			/// Size of heap
			/// </summary>
			UINT64 m_size = 0;
			
			/// <summary>
			/// Write head
			/// </summary>
			UINT64 m_writeHead = 0;

			/// <summary>
			/// Heap
			/// </summary>
			DX::XHeap m_heap;
	};
}