#include "DefHeader.h"
#include "FenceCounter.h"

DX::FenceCounter::FenceCounter(XFence& fence) :
	m_fence(fence)
{
	// Signal fence to zero if required
	if ((UINT64)fence) {
		fence.signal(0);
	}
}

void DX::FenceCounter::release() noexcept {
	// Lock object
	while (m_opLock.test_and_set(std::memory_order_acquire));
	
	// Release member
	m_fence.release();
	// Store zero
	m_counter.store(0);

	// Unlock object
	m_opLock.clear(std::memory_order_release);
}

UINT64 DX::FenceCounter::head() noexcept{
	// Guard
	while (m_opLock.test(std::memory_order_acquire));

	// Return counter value if fence is valid else zero
	return (m_fence ? m_counter.load(std::memory_order_relaxed) : 0);
}

void DX::FenceCounter::setFenceObject(XFence& newFence) {
	// Lock object
	while (m_opLock.test_and_set(std::memory_order_acquire));
	
	// Clear old fence if required
	if (m_fence.ptr()) {
		// Create wait object and wait for completion
		m_fence.createWaitObject(m_counter).wait();
	}

	// Assign new fence
	m_fence = newFence;
	// Signal new fence to counter value
	newFence.signal(m_counter);

	// Unlock object
	m_opLock.clear(std::memory_order_release);
}

DX::FenceCounter::Frontend DX::FenceCounter::newFrontend() noexcept {
	return Frontend(*this);
}

void DX::FenceCounter::operator=(XFence& other) {
	setFenceObject(other);
}

DX::FenceCounter::operator UINT64() noexcept {
	return head();
}

DX::FenceCounter::operator bool() noexcept {
	return m_fence;
}

UINT64 DX::FenceCounter::next() noexcept {
	// Guard
	while (m_opLock.test(std::memory_order_acquire));

	// Increment
	return ++m_counter;
}

DX::FenceCounter::Frontend::~Frontend() {
	release();
}

DX::FenceCounter::Frontend::Frontend(FenceCounter& refCounter) noexcept :
	m_ptrCounter(&refCounter)
{}

void DX::FenceCounter::Frontend::release() noexcept {
	// If pointer is valid wait
	if (m_ptrCounter) {
		getCurrentWaitObject().wait();
	}

	m_ptrCounter = nullptr;
	m_head = 0;
}

UINT64 DX::FenceCounter::Frontend::next() {
	// Assert internal pointer
	EXPP_ASSERT(m_ptrCounter, "Counter pointer was null!");

	// Set head
	m_head = m_ptrCounter->next();
	return m_head;
}

UINT64 DX::FenceCounter::Frontend::head() noexcept {
	return m_head;
}

DX::XFence& DX::FenceCounter::Frontend::fence() {
	// Assert internal pointer
	EXPP_ASSERT(m_ptrCounter, "Counter pointer was null!");
	
	// Return fence
	return m_ptrCounter->m_fence;
}

DX::XFence::WaitObject DX::FenceCounter::Frontend::getCurrentWaitObject() {
	// Assert internal pointer
	EXPP_ASSERT(m_ptrCounter, "Counter pointer was null!");

	// Create wait object for the current head
	return m_ptrCounter->m_fence.createWaitObject(m_head);
}

DX::FenceCounter::Frontend::operator bool() noexcept {
	return m_ptrCounter;
}

DX::FenceCounter::Frontend::operator DX::XFence&() {
	return fence();
}

DX::FenceCounter::Frontend::operator ID3D12Fence* () {
	// Assert internal pointer
	EXPP_ASSERT(m_ptrCounter && *m_ptrCounter, "Counter pointer or FencePointer was null!");

	return m_ptrCounter->m_fence;
}

void DX::FenceCounter::Frontend::operator=(FenceCounter& other){
	// If pointer is valid wait
	if (m_ptrCounter) {
		getCurrentWaitObject().wait();
	}
	
	m_ptrCounter = &other;
	m_head = 0;
}
