#pragma once

#include <DefHeader.h>
#include <dx/memory/XHeap.h>
#include <dx/memory/XResource.h>

#include <engine/resources/heapTools/GpuStackHeap.h>


namespace engine {
	namespace rendering {
		/// <summary>
		/// Alliassing for type
		/// </summary>
		typedef struct __FixedBuffer_BufferDesc {
			union {
				D3D12_VERTEX_BUFFER_VIEW VB;
				D3D12_INDEX_BUFFER_VIEW IB;
				D3D12_CONSTANT_BUFFER_VIEW_DESC CB;
			};
		} FixedBuffer_BufferDesc;

		/// <summary>
		/// Fixed Typed and Sized GPU buffer
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template<typename T, unsigned int N>
		class FixedBuffer {
			public:
				/// <summary>
				/// Default constructor
				/// </summary>
				FixedBuffer() = default;

				/// <summary>
				/// Fixed buffer constructor
				/// </summary>
				/// <param name="ptrDevice">Device pointer</param>
				/// <param name="ptrAllocator">Pointer to allocator</param>
				FixedBuffer(ID3D12Device* ptrDevice, engine::GpuStackHeap* ptrAllocator) {
					// Create descritpor
					D3D12_RESOURCE_DESC bufferDesc;
					ZeroMemory(&bufferDesc, sizeof(D3D12_RESOURCE_DESC));

					// Describe resource
					bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
					bufferDesc.Width = sizeof(T) * N;
					bufferDesc.Height = 1;
					bufferDesc.DepthOrArraySize = 1;
					bufferDesc.MipLevels = 1;
					bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
					bufferDesc.SampleDesc.Count = 1;
					bufferDesc.SampleDesc.Quality = 0;
					bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
					bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

					// Allocate
					HEAP_ALLOCATION allocation;
					EXPP_ASSERT(ptrAllocator->alloc(allocation, DX::XResource::size(ptrDevice, &bufferDesc)), "GPU Memory allocation failed");

					// Create buffer
					m_gpuBuffer = DX::XResource(ptrDevice, allocation, &bufferDesc, NULL, D3D12_RESOURCE_STATE_GENERIC_READ);
				}

				/// <summary>
				/// Destruct
				/// </summary>
				~FixedBuffer() {
					release();
				}

				/// <summary>
				/// Release function
				/// </summary>
				void release() {
					m_gpuBuffer.release();
				}

				/// <summary>
				/// Move construct
				/// </summary>
				/// <param name="other">Other buffer</param>
				FixedBuffer(const FixedBuffer&& other) {
					// Move resource
					m_gpuBuffer = std::move(other.m_gpuBuffer);
					// Copy data
					memcpy(m_cpuBuffer, other.m_cpuBuffer, sizeof(T) * N);
				}

				/// <summary>
				/// Move assign
				/// </summary>
				/// <param name="other">Other class</param>
				FixedBuffer& operator=(const FixedBuffer&& other) {
					// Move resource
					m_gpuBuffer = std::move(other.m_gpuBuffer);
					// Copy data
					memcpy(m_cpuBuffer, other.m_cpuBuffer, sizeof(T) * N);

					return *this;
				}

				/// <summary>
				/// Get at index
				/// </summary>
				/// <param name="index">index to retrive</param>
				/// <returns>Reference to T element</returns>
				T& get(unsigned int index) {
					EXPP_ASSERT(index < N, "Index out of range");
					return m_cpuBuffer[index];
				}

				/// <summary>
				/// Get at index operator
				/// </summary>
				/// <param name="index">index to retrive</param>
				/// <returns>Reference to T element</returns>
				T& operator[](unsigned int index) {
					EXPP_ASSERT(index < N, "Index out of range");
					return m_cpuBuffer[index];
				}

				/// <summary>
				/// Get pointer
				/// </summary>
				/// <returns>Pointer</returns>
				void* ptr() noexcept {
					return m_cpuBuffer;
				}

				/// <summary>
				/// Retrive resource
				/// </summary>
				/// <returns>ref to resource</returns>
				DX::XResource& res() noexcept {
					return m_gpuBuffer;
				}

				/// <summary>
				/// Get size
				/// </summary>
				/// <returns>Size</returns>
				constexpr size_t size() noexcept {
					return sizeof(T) * N;
				}

				/// <summary>
				/// Create vertex buffer view for buffer
				/// </summary>
				/// <returns>Pointer to view</returns>
				D3D12_VERTEX_BUFFER_VIEW* createVertexBufferView() noexcept {
					m_gpuView.VB.BufferLocation = m_gpuBuffer->GetGPUVirtualAddress();
					m_gpuView.VB.SizeInBytes = sizeof(T) * N;
					m_gpuView.VB.StrideInBytes = sizeof(T);

					return &m_gpuView.VB;
				}

				/// <summary>
				/// Create index buffer view
				/// </summary>
				/// <returns>Pointer to view</returns>
				template<typename = std::enable_if_t<std::is_same_v<T, UINT32>>>
				D3D12_INDEX_BUFFER_VIEW* createIndexBufferView() noexcept {
					m_gpuView.IB.BufferLocation = m_gpuBuffer->GetGPUVirtualAddress();
					m_gpuView.IB.Format = DXGI_FORMAT_R32_UINT;
					m_gpuView.IB.SizeInBytes = sizeof(T) * N;

					return &m_gpuView.IB;
				}

				/// <summary>
				/// Create constant buffer view
				/// </summary>
				/// <returns>Pointer to view</returns>
				D3D12_CONSTANT_BUFFER_VIEW_DESC* createConstantBufferView() noexcept {
					m_gpuView.CB.BufferLocation = m_gpuBuffer->GetGPUVirtualAddress();
					m_gpuView.CB.SizeInBytes = sizeof(T) * N;

					return &m_gpuView.CB;
				}

			private:
				/// <summary>
				/// CPU Sided buffer
				/// </summary>
				T m_cpuBuffer[N];

				/// <summary>
				/// GPU View
				/// </summary>
				FixedBuffer_BufferDesc m_gpuView;

				/// <summary>
				/// GPU Sided buffer
				/// </summary>
				DX::XResource m_gpuBuffer;
		};
	}
}