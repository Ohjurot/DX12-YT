#include "DefHeader.h"
#include "GpuStackHeap.h"

engine::GpuStackHeap::GpuStackHeap(ID3D12Device* ptrDevice, UINT64 size, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, UINT64 allignement) {
	EXPP_ASSERT(ptrDevice, "Invalid device pointer");
	EXPP_ASSERT(size && (size % MEM_KiB(64) == 0), "Invalid size musst be > 0 and multible of MEM_KiB(64)");

	// Create heap
	m_heap = DX::XHeap(ptrDevice, size, type, flags, allignement);

	// Make heap resident
	DX::XFence::WaitObject wo;
	EVALUATE_HRESULT(m_heap.makeResident(wo), "DX::XHeap->makeResident(...)");
	wo.wait();

	// Set size
	m_size = size;
}

engine::GpuStackHeap::~GpuStackHeap() noexcept {
	release();
}

void engine::GpuStackHeap::release() noexcept {
	// Release heap and unset size
	if (m_heap) {
		m_heap.release();
		m_size = 0;
	}
}

void engine::GpuStackHeap::name(LPCWSTR name) noexcept {
	if (m_heap) {
		m_heap.name(name);
	}
}

bool engine::GpuStackHeap::alloc(HEAP_ALLOCATION& allocInfo, UINT64 size) {
	// Only if input size is valid
	if (size) {
		// Fix aligned allocation
		if (size % MEM_KiB(64) != 0) {
			size = size + MEM_KiB(64) - (size % MEM_KiB(64));
		}
		
		// Lock function
		MT::LockGuard janitor(m_lock);

		// Check size
		if (m_writeHead + size > m_size) {
			return false;
		}
		
		// Set info
		allocInfo.offset = m_writeHead;
		allocInfo.size = size;
		allocInfo.ptrHeap = &m_heap;

		// Increment head
		m_writeHead += size;

		// OK
		return true;
	}

	// Fallback false
	return false;
}
