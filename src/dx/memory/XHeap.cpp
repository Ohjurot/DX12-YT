#include "DefHeader.h"
#include "XHeap.h"

DX::XHeap::XHeap(XHeap&& other) noexcept :
	m_bResident(other.m_bResident),
	m_ptrDevice(other.m_ptrDevice),
	m_fence(std::move(other.m_fence)),
	m_size(other.m_size)
{
	// Reset fence counter
	m_fenceCounter.setFenceObject(m_fence);
	m_counterFrontent = m_fenceCounter.newFrontend();

	// Copy and release other
	m_comPointer = other.m_comPointer;
	other.m_comPointer = nullptr;
}

DX::XHeap::XHeap(ID3D12Device* ptrDevice, UINT64 size, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, UINT64 allignement) :
	m_fence(ptrDevice)
{
	// Set counter
	m_fenceCounter.setFenceObject(m_fence);
	m_counterFrontent = m_fenceCounter.newFrontend();

	// Get device
	EVALUATE_HRESULT(ptrDevice->QueryInterface(IID_PPV_ARGS(m_ptrDevice.to())), "ID3D12Device->QueryInterface(...) for ID3D12Device3");

	// Create Heap descriptor
	D3D12_HEAP_DESC hd;
	ZeroMemory(&hd, sizeof(D3D12_HEAP_DESC));

	// Describe heap
	hd.SizeInBytes = size;
	hd.Properties.Type = type;
	hd.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hd.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hd.Properties.CreationNodeMask = NULL;
	hd.Properties.VisibleNodeMask = NULL;
	hd.Alignment = allignement;
	hd.Flags = flags | D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT | D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

	// Create Heap
	EVALUATE_HRESULT(m_ptrDevice->CreateHeap(&hd, IID_PPV_ARGS(to())), "ID3D12Device->CreateHeap(...)");
	m_size = size;
}

void DX::XHeap::release() {
	m_counterFrontent.release();
	m_fenceCounter.release();
	m_fence.release();
	m_ptrDevice.release();

	ScopedComPointer::release();
}

HRESULT DX::XHeap::makeResident(XFence::WaitObject& refWaitObject) {
	EXPP_ASSERT_DBG(m_comPointer && m_ptrDevice, "Invalid Heap object");

	// Make resident if required
	if (!m_bResident) {
		// Make resident
		HRESULT hr = m_ptrDevice->EnqueueMakeResident(D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET, 1, (ID3D12Pageable* const*)&m_comPointer, m_fence, m_counterFrontent.next());
		if (SUCCEEDED(hr)) {
			refWaitObject = m_counterFrontent.getCurrentWaitObject();
			m_bResident = true;
		}

		// Return HRESULTf
		return hr;
	}

	// Default ok
	return S_OK;
}

HRESULT DX::XHeap::evict() {
	EXPP_ASSERT_DBG(m_comPointer && m_ptrDevice, "Invalid Heap object");

	// Evict if required
	if (m_bResident) {
		HRESULT hr = m_ptrDevice->Evict(1, (ID3D12Pageable* const*)to());
		if (SUCCEEDED(hr)) {
			m_bResident = false;
		}

		return hr;
	}

	// Default ok
	return S_OK;
}

UINT64 DX::XHeap::getSize() noexcept {
	return m_size;
}

DX::XHeap& DX::XHeap::operator=(XHeap&& other) noexcept {
	// Copy memebers
	m_bResident = std::move(other.m_bResident);
	m_ptrDevice = std::move(other.m_ptrDevice);
	m_fence = std::move(other.m_fence);
	m_size = other.m_size;

	// Update internal
	m_fenceCounter.setFenceObject(m_fence);
	m_counterFrontent = m_fenceCounter.newFrontend();

	// Copy and release other
	m_comPointer = other.m_comPointer;
	other.m_comPointer = nullptr;
	
	// Return 
	return *this;
}
