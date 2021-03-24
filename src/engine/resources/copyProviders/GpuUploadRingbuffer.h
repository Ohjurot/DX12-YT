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
	/// GPU uploading ringbuffer
	/// </summary>
	class GpuUploadRingbuffer {
		public:
			/// <summary>
			/// Create ringbuffer
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refHeap">Heap to place resource on</param>
			/// <param name="offset">Offset in heap</param>
			/// <param name="size">Size of the upload buffer</param>
			GpuUploadRingbuffer(ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 offset, UINT64 size);

			/// <summary>
			/// Create ringbuffer
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refAllocation">Allocation info</param>
			/// <param name="size">Size of the upload buffer</param>
			GpuUploadRingbuffer(ID3D12Device* ptrDevice, HEAP_ALLOCATION& refAllocation, UINT64 size);

			/// <summary>
			/// Destructor
			/// </summary>
			~GpuUploadRingbuffer();

			/// <summary>
			/// Release function
			/// </summary>
			void release();

			/// <summary>
			/// Queue a upload
			/// </summary>
			/// <param name="ptrTarge"></param>
			/// <param name="memory"></param>
			/// <param name="offset"></param>
			/// <param name="size"></param>
			/// <param name="refWaitObject"></param>
			/// <param name="dependency"></param>
			/// <returns></returns>
			bool queueUpload(ID3D12Resource* ptrTarge, void* memory, UINT64 offset, UINT64 size, DX::XCommandList::WaitObject& refWaitObject, DX::XCommandList::WaitObject dependency = DX::XCommandList::WaitObject());

			/// <summary>
			/// Kickoff all pending copy
			/// </summary>
			void kickoff();

			/// <summary>
			/// Name the COM object
			/// </summary>
			/// <param name="name">New name</param>
			void name(LPCWSTR name);

			// Delete unused
			GpuUploadRingbuffer(const GpuUploadRingbuffer&) = delete;
			GpuUploadRingbuffer& operator=(const GpuUploadRingbuffer&) = delete;

		private:
			/// <summary>
			/// Internal execute dispatch
			/// </summary>
			void _internal_execute();

		private:
			/// <summary>
			/// MT Lock for class
			/// </summary>
			MT::SpinLock<THREAD_PAUSE_FUNCTION_F> m_lock;

			/// <summary>
			/// Indicates that the object is valid
			/// </summary>
			bool m_bValid = false;

			/// <summary>
			/// Size of the buffer
			/// </summary>
			const UINT64 m_size;

			/// <summary>
			/// Device pointer
			/// </summary>
			ScopedComPointer<ID3D12Device> m_ptrDevice;

			/// <summary>
			/// Command list access object
			/// </summary>
			DX::CommandListAccessObject m_accessObject;

			/// <summary>
			/// Buffer resource
			/// </summary>
			DX::XResource m_buffer;

			/// <summary>
			/// Current execution wait object
			/// </summary>
			DX::XCommandList::WaitObject m_currentWaitObject;

			/// <summary>
			/// Write head
			/// </summary>
			UINT64 m_writeHead = 0;

			/// <summary>
			/// Read head
			/// </summary>
			UINT64 m_readHead = 0;

			/// <summary>
			/// Read head after execute complets
			/// </summary>
			UINT64 m_readHeadInFlight = 0;

			/// <summary>
			/// Upload backlock
			/// </summary>
			UINT64 m_uiBacklock = 0;
	};
}