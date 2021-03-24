#pragma once

#include <DefHeader.h>

#include <common/MT/Locks/SpinLock.h>
#include <engine/resources/heapTools/HeapAllocation.h>
#include <dx/memory/XHeap.h>
#include <dx/memory/XResource.h>
#include <dx/cmds/XCommandList.h>
#include <dx/cmds/CommandListAccessObject.h>

namespace engine {
	/// <summary>
	/// Stack buffer for gpu uploading
	/// </summary>
	class GpuUploadStackBuffer {
		public:
			/// <summary>
			/// GPU Upload stack buffer
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refHeap">Heap to place upload buffer on</param>
			/// <param name="heapOffset">Offset into heap</param>
			/// <param name="size">Size of upload buffer</param>
			GpuUploadStackBuffer(ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 heapOffset, UINT64 size);

			/// <summary>
			/// GPU Upload stack buffer
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refAllocation">Reference to heap allocation</param>
			/// <param name="size">Size of upload buffer</param>
			GpuUploadStackBuffer(ID3D12Device* ptrDevice, HEAP_ALLOCATION& refAllocation, UINT64 size);

			/// <summary>
			/// Destructor
			/// </summary>
			~GpuUploadStackBuffer();

			/// <summary>
			/// Release function
			/// </summary>
			void release();

			/// <summary>
			/// Name the object
			/// </summary>
			/// <param name="name">Name</param>
			void name(LPCWSTR name);

			/// <summary>
			/// Queue a copy operation
			/// </summary>
			/// <param name="ptrTarget">Target pointer</param>
			/// <param name="targetOffset">Offset into target</param>
			/// <param name="ptrSource">Source memory</param>
			/// <param name="copySize">amount to copy</param>
			/// <returns>true if memory was sufficient</returns>
			bool queueCopy(ID3D12Resource* ptrTarget, UINT64 targetOffset, void* ptrSource, UINT64 copySize);

			/// <summary>
			/// Execute copy operations
			/// </summary>
			/// <param name="dependency">Optional input dependency</param>
			/// <returns>Wait object of current execution (can be null!)</returns>
			DX::XCommandList::WaitObject execute(DX::XCommandList::WaitObject dependency = DX::XCommandList::WaitObject());

		private:
			/// <summary>
			/// Spin lock for concurrent access
			/// </summary>
			MT::SpinLock<THREAD_PAUSE_FUNCTION_F> m_lock;

			/// <summary>
			/// Size of the upload buffer
			/// </summary>
			const UINT64 m_size;

			/// <summary>
			/// Upload buffer
			/// </summary>
			DX::XResource m_buffer;

			/// <summary>
			/// Write head
			/// </summary>
			UINT64 m_writeHead = 0;

			/// <summary>
			/// Access object
			/// </summary>
			DX::CommandListAccessObject m_accessObject;

			/// <summary>
			/// Wait object
			/// </summary>
			DX::XCommandList::WaitObject m_waitObject;
	};
}