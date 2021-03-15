#pragma once

#include <DefHeader.h>

#include <dx/memory/XHeap.h>

namespace DX {
	/// <summary>
	/// Resource
	/// </summary>
	class XResource : public ScopedComPointer<ID3D12Resource> {
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			XResource() = default;

			/// <summary>
			/// Create Resource
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refHeap">Heap reference</param>
			/// <param name="heapOffset">Heap offset</param>
			/// <param name="ptrDesc">Resource description</param>
			/// <param name="ptrClear">Clear description</param>
			/// <param name="state">Initaial state</param>
			XResource(ID3D12Device* ptrDevice, XHeap& refHeap, UINT64 heapOffset, D3D12_RESOURCE_DESC* ptrDesc, D3D12_CLEAR_VALUE* ptrClear, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_GENERIC_READ);

			/// <summary
			/// Move constructor
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			XResource(XResource&& other) noexcept;

			/// <summary>
			/// Retrive current resource state
			/// </summary>
			/// <returns>Resource state</returns>
			D3D12_RESOURCE_STATES getState() noexcept;

			/// <summary>
			/// Gennerates a resource barrier on given command list
			/// </summary>
			/// <param name="ptrCmdList">CommandList pointer</param>
			/// <param name="requestedState">New requested state</param>
			void resourceTransition(ID3D12GraphicsCommandList* ptrCmdList, D3D12_RESOURCE_STATES requestedState);

			/// <summary>
			/// Retrive size of resource
			/// </summary>
			/// <returns>Size of resource</returns>
			UINT64 getSize();

			/// <summary>
			/// Move assign
			/// </summary>
			XResource& operator=(XResource&& other) noexcept;
		private:
			/// <summary>
			/// The last set state of the resource
			/// </summary>
			D3D12_RESOURCE_STATES m_lastSetState = D3D12_RESOURCE_STATE_GENERIC_READ;

			/// <summary>
			/// Size of the resource
			/// </summary>
			UINT64 m_size = 0;
	};
}