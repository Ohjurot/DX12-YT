#include "DefHeader.h"
#include "XResource.h"

UINT64 DX::XResource::size(ID3D12Device* ptrDevice, D3D12_RESOURCE_DESC* ptrDesc) {
	EXPP_ASSERT(ptrDevice, "Invalid device pointer");

	// Get and return size
	D3D12_RESOURCE_ALLOCATION_INFO info = ptrDevice->GetResourceAllocationInfo(NULL, 1, ptrDesc);
	return info.SizeInBytes;
}

DX::XResource::XResource(ID3D12Device* ptrDevice, HEAP_ALLOCATION& allocation, D3D12_RESOURCE_DESC* ptrDesc, D3D12_CLEAR_VALUE* ptrClear, D3D12_RESOURCE_STATES state) {
	// Assert pointer
	EXPP_ASSERT(ptrDevice, "Invalid device pointer");
	EXPP_ASSERT(allocation.ptrHeap && *allocation.ptrHeap, "Invalid heap");

	// Get and assert size
	D3D12_RESOURCE_ALLOCATION_INFO info = ptrDevice->GetResourceAllocationInfo(NULL, 1, ptrDesc);
	EXPP_ASSERT(allocation.size >= info.SizeInBytes, "Insufficient allocation");

	// Create resource
	EVALUATE_HRESULT(ptrDevice->CreatePlacedResource(*allocation.ptrHeap, allocation.offset, ptrDesc, state, ptrClear, IID_PPV_ARGS(&m_comPointer)), "ID3D12Device->CreatePlacedResource(...)");

	// Set parameters
	m_size = info.SizeInBytes;
	m_lastSetState = state;
}

DX::XResource::XResource(ID3D12Device* ptrDevice, XHeap& refHeap, UINT64 heapOffset, D3D12_RESOURCE_DESC* ptrDesc, D3D12_CLEAR_VALUE* ptrClear, D3D12_RESOURCE_STATES state) {
	// Assert pointer
	EXPP_ASSERT(ptrDevice, "Invalid device pointer");
	EXPP_ASSERT(refHeap, "Invalid heap");

	// Get and assert size
	D3D12_RESOURCE_ALLOCATION_INFO info = ptrDevice->GetResourceAllocationInfo(NULL, 1, ptrDesc);
	EXPP_ASSERT((heapOffset + info.SizeInBytes) <= refHeap.getSize(), "Heap not big enough");

	// Create resource
	EVALUATE_HRESULT(ptrDevice->CreatePlacedResource(refHeap, heapOffset, ptrDesc, state, ptrClear, IID_PPV_ARGS(&m_comPointer)), "ID3D12Device->CreatePlacedResource(...)");

	// Set parameters
	m_size = info.SizeInBytes;
	m_lastSetState = state;
}

DX::XResource::XResource(XResource&& other) noexcept {
	// Take over pointer
	m_comPointer = other.m_comPointer;
	other.m_comPointer = nullptr;

	// Copy data
	m_lastSetState = other.m_lastSetState;
	m_size = other.m_size;
}

D3D12_RESOURCE_STATES DX::XResource::getState() noexcept {
	return m_lastSetState;
}

void DX::XResource::resourceTransition(ID3D12GraphicsCommandList* ptrCmdList, D3D12_RESOURCE_STATES requestedState) {
	// Only trigger state change if required
	if (m_lastSetState != requestedState) {
		// Assert state
		EXPP_ASSERT(m_comPointer, "Invalid Resource Object");
		EXPP_ASSERT_DBG(ptrCmdList, "Invalid input command list");

		// Create barrier descriptor
		D3D12_RESOURCE_BARRIER barrier;
		ZeroMemory(&barrier, sizeof(D3D12_RESOURCE_BARRIER));

		// Describe barrier
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_comPointer;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = m_lastSetState;
		barrier.Transition.StateAfter = requestedState;

		// Queue barrier
		ptrCmdList->ResourceBarrier(1, &barrier);

		// Store stage
		m_lastSetState = requestedState;
	}
}

UINT64 DX::XResource::getSize() {
	return m_size;
}

DX::XResource& DX::XResource::operator=(XResource&& other) noexcept{
	// Take over pointer
	m_comPointer = other.m_comPointer;
	other.m_comPointer = nullptr;

	// Copy data
	m_lastSetState = other.m_lastSetState;
	m_size = other.m_size;

	// 
	return *this;
}
