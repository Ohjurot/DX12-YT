#pragma once

#include <DefHeader.h>

#include <string>
#include <sstream>

#include <engine/resources/HeapTools/HeapAllocation.h>
#include <common/MT/Locks/SpinLock.h>
#include <dx/memory/XHeap.h>
#include <dx/memory/XResource.h>
#include <dx/cmds/XCommandList.h>
#include <dx/cmds/CommandListAccessObject.h>

namespace engine {
	/// <summary>
	/// Pooled texture upload
	/// </summary>
	class GpuUploadTexturePool {
		public:
			/// <summary>
			/// Create texture uploading pool (64 * 64 pixel tiles)
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refHeap">Heap to place tiles on</param>
			/// <param name="offset">Offset in heap</param>
			/// <param name="bpp">Bits per pixel</param>
			/// <param name="tileCount">Number of tiles</param>
			GpuUploadTexturePool(ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 offset, unsigned int bpp = 8 * 4, unsigned int tileCount = 64);

			/// <summary>
			/// Create texture uploading pool (64 * 64 pixel tiles)
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="allocation">Allocation info</param>
			/// <param name="bpp">Bits per pixel</param>
			/// <param name="tileCount">Number of tiles</param>
			GpuUploadTexturePool(ID3D12Device* ptrDevice, HEAP_ALLOCATION& allocation, unsigned int bpp = 8 * 4, unsigned int tileCount = 64);
			
			/// <summary>
			/// Destructor
			/// </summary>
			~GpuUploadTexturePool();

			/// <summary>
			/// Release texture pool
			/// </summary>
			void release();

			/// <summary>
			/// Name the pool
			/// </summary>
			/// <param name="name">Name of the pool</param>
			void name(LPCWSTR name);

			/// <summary>
			/// Kickoff all pending copy
			/// </summary>
			void kickoff();

			/// <summary>
			/// Queue a texture for upload
			/// </summary>
			/// <param name="ptrTarget">Target resource</param>
			/// <param name="ptrSource">Pointer to source memory</param>
			/// <param name="sourceHeight">Source image width</param>
			/// <param name="sourceWidth">Source image height</param>
			/// <param name="sourceFormate">DXGI Formate</param>
			/// <param name="refWaitObject">Wait object to set</param>
			/// <param name="dependency">Optional dependency to queue on</param>
			void queueUploadTexture(ID3D12Resource* ptrTarget, void* ptrSource, UINT64 sourceHeight, UINT64 sourceWidth, DXGI_FORMAT sourceFormate, DX::XCommandList::WaitObject& refWaitObject, DX::XCommandList::WaitObject dependency = DX::XCommandList::WaitObject());

			// Delete unsupported
			GpuUploadTexturePool(const GpuUploadTexturePool&) = delete;
			GpuUploadTexturePool& operator=(const GpuUploadTexturePool&) = delete;

		private:
			/// <summary>
			/// Internal copy operation
			/// </summary>
			/// <param name="ptrTarget">Target resource</param>
			/// <param name="tile">Tile to index</param>
			/// <param name="x">X-Position</param>
			/// <param name="y">Y-Position</param>
			/// <param name="copyWidth">Copy width</param>
			/// <param name="copyHeight">Copy height</param>
			/// <param name="totalWidth">Total width</param>
			/// <param name="ptrSource">Source pointer</param>
			/// <param name="targetFormat">Target buffer format</param>
			void _internal_tileUpload(ID3D12Resource* ptrTarget, unsigned int tile, UINT64 x, UINT64 y, UINT64 copyWidth, UINT64 copyHeight, UINT64 totalWidth, BYTE* ptrSource, DXGI_FORMAT targetFormat);

			/// <summary>
			/// Internaly allocate a tile for uploading
			/// </summary>
			/// <param name="refWaitObject"></param>
			/// <param name="dependency"></param>
			/// <returns></returns>
			unsigned int _internal_tileAllocator(DX::XCommandList::WaitObject& refWaitObject, DX::XCommandList::WaitObject dependency);

			/// <summary>
			/// Internal execute dispatch
			/// </summary>
			void _internal_execute();

		private:
			/// <summary>
			/// Bytes per pixel
			/// </summary>
			const unsigned int m_bytesPerPixel;

			/// <summary>
			/// Number of tiles allocated
			/// </summary>
			const unsigned int m_tileCount;

			/// <summary>
			/// Size of a tile
			/// </summary>
			const UINT64 m_tileSize;

			/// <summary>
			/// Indicates that the class is valid
			/// </summary>
			bool m_bValid = false;

			/// <summary>
			/// Class spin lock
			/// </summary>
			MT::SpinLock<THREAD_PAUSE_FUNCTION_F> m_lock;

			/// <summary>
			/// Upload buffer
			/// </summary>
			DX::XResource m_buffer;

			/// <summary>
			/// Command list access object
			/// </summary>
			DX::CommandListAccessObject m_accessObject;

			/// <summary>
			/// Current write positon
			/// </summary>
			unsigned int m_writeHead = 0;

			/// <summary>
			/// Number of tiles in flisht
			/// </summary>
			unsigned int m_numInFlight = 0;
			 
			/// <summary>
			/// Current usage
			/// </summary>
			unsigned int usage = 0;

			/// <summary>
			/// Execution wait object
			/// </summary>
			DX::XCommandList::WaitObject m_waitObject;
	};
}