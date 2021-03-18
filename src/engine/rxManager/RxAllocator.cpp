#include "DefHeader.h"
#include "RxAllocator.h"

// Implementation
engine::_RxAllocator* engine::RxAllocator::s_ptrInstance = nullptr;

void engine::RxAllocator::init() {
	// Allocate if required
	if (!s_ptrInstance) {
		s_ptrInstance = new _RxAllocator();
	}
}

void engine::RxAllocator::shutdown() {
	// Delete and unset if required
	if (s_ptrInstance) {
		delete s_ptrInstance;
		s_ptrInstance = nullptr;
	}
}

engine::_RxAllocator* engine::RxAllocator::getInstance() noexcept {
	return s_ptrInstance;
}

engine::_RxAllocator::_RxAllocator() {
	// Allocate and assert
	m_arrAllocator[0] = (__RX_DATA*)malloc(sizeof(__RX_DATA) * CLS_ENGINE_RXMANAGER_RXALLOCATOR__ALLOCATION_COUNT);
	EXPP_ASSERT(m_arrAllocator[0], "Allocation for resources failed");

	// Init array to zero
	memset(&m_arrAllocator[1], 0x00, sizeof(__RX_DATA*) * (CLS_ENGINE_RXMANAGER_RXALLOCATOR__ARRAY_ENTRYS - 1));
}

engine::_RxAllocator::~_RxAllocator() {
	// Free all array members that are set
	for (unsigned int i = 0; i < CLS_ENGINE_RXMANAGER_RXALLOCATOR__ARRAY_ENTRYS && m_arrAllocator[i]; i++) {
		free(m_arrAllocator[i]);
	}
}

RESOURCE engine::_RxAllocator::alloc() noexcept {
	MT::LockGuard janitor(m_allocationLock);

	// Check current pointer
	if (m_arrIndex >= CLS_ENGINE_RXMANAGER_RXALLOCATOR__ARRAY_ENTRYS || !m_arrAllocator[m_arrIndex]) {
		return { 0,0 };
	}

	// Get current pointer
	__RX_DATA* ptrData = &m_arrAllocator[m_arrIndex][m_elementIndex];

	// Find next index
	m_elementIndex++;
	if (m_elementIndex == CLS_ENGINE_RXMANAGER_RXALLOCATOR__ALLOCATION_COUNT) {
		// Next array
		m_elementIndex = 0;
		m_arrIndex++;

		// Allocate next array
		if (m_arrIndex < CLS_ENGINE_RXMANAGER_RXALLOCATOR__ARRAY_ENTRYS) {
			m_arrAllocator[m_arrIndex] = (__RX_DATA*)malloc(sizeof(__RX_DATA) * CLS_ENGINE_RXMANAGER_RXALLOCATOR__ALLOCATION_COUNT);
		}
	}

	// Release janitor
	janitor.release();

	// Craft handle
	RESOURCE rHandle;
	rHandle.refId = 0;
	rHandle.pReserved = ptrData;
	
	// Return crafted handle
	return rHandle;
}
