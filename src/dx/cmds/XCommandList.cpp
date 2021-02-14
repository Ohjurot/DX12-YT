#include "DefHeader.h"
#include "XCommandList.h"

DX::XCommandList::~XCommandList() {
	release();
}

void DX::XCommandList::init(ID3D12Device* ptrDevice, XCommandQueue& refCommandQueue, FenceCounter& refFenceCounter) {
	// Assert state
	EXPP_ASSERT(m_state == XCommandList_State::INVALID, "Init can only be called on an invalid command list");

	// Reference queue
	m_ptrCommandQueue = &refCommandQueue;
	m_ptrCommandQueue->incrementRef();

	// Create command allocator
	EVALUATE_HRESULT(ptrDevice->CreateCommandAllocator(refCommandQueue.getQueueType(), IID_PPV_ARGS(m_allocator.to())), "ID3D12Device->CreateCommandAllocator(...)");

	// Create command list
	EVALUATE_HRESULT(ptrDevice->CreateCommandList(NULL, refCommandQueue.getQueueType(), m_allocator, NULL, IID_PPV_ARGS(to())), "ID3D12Device->CreateCommandList(...)");

	// Assign frontend
	m_fenceFrontend = refFenceCounter.newFrontend();

	// Set state
	m_state = XCommandList_State::OPEN;
}

void DX::XCommandList::release() {
	// Wait for frontend
	if (m_fenceFrontend) {
		m_fenceFrontend.getCurrentWaitObject().wait();
	}
	
	// Invalidate
	m_state = XCommandList_State::INVALID;

	// Unreference queue
	if (m_ptrCommandQueue) {
		m_ptrCommandQueue->decrementRef();
		m_ptrCommandQueue = nullptr;
	}

	// Release allocator
	m_allocator.release();
	m_fenceFrontend.release();
	
	// Releaee own
	ScopedComPointer::release();

	// Set state
	m_state = XCommandList_State::INVALID;
}

DX::XCommandList_State DX::XCommandList::getState() {
	// Refresh state
	refresh();

	// Return new / old state
	return m_state;
}

void DX::XCommandList::refresh(){
	// Check if list is done executing
	if (m_state == XCommandList_State::QUEUE_EXECUTING && m_fenceFrontend.getCurrentWaitObject().isDone()) {
		// Reset all
		EVALUATE_HRESULT(m_allocator->Reset(), "ID3D12CommandAllocator->Reset(...)");
		m_comPointer->Reset(m_allocator, NULL);
		// Is back open
		m_state = XCommandList_State::OPEN;
		// Increment iteration
		m_uiIteration++;
	}
}

DX::XCommandList::WaitObject DX::XCommandList::close() {
	EXPP_ASSERT(getState() == XCommandList_State::OPEN, "Close can only be called on an open command list");

	// Close list
	EVALUATE_HRESULT(m_comPointer->Close(), "ID3D12GraphicsCommandList->Close(...)");
	m_state = XCommandList_State::CLOSED;

	// Return wait object
	return getUniversalWaitObject();
}

DX::XFence::WaitObject DX::XCommandList::execute() {
	EXPP_ASSERT(getState() == XCommandList_State::CLOSED, "Execute can only be called on a close command list");

	// Execute the command list
	m_ptrCommandQueue->ptr()->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_comPointer);
	m_ptrCommandQueue->ptr()->Signal(m_fenceFrontend, m_fenceFrontend.next());
	m_state = XCommandList_State::QUEUE_EXECUTING;

	// Return wait object
	return getExecutionWaitObject();
}

DX::XFence::WaitObject DX::XCommandList::getExecutionWaitObject() {
	// Return fences current wait object
	return m_fenceFrontend.getCurrentWaitObject();
}

DX::XCommandList::WaitObject DX::XCommandList::getUniversalWaitObject() {
	// Create wait object on current itteration
	return WaitObject(this, m_uiIteration);
}

UINT64 DX::XCommandList::getCurrentItteration() noexcept {
	return m_uiIteration;
}

DX::XCommandList::WaitObject::WaitObject(XCommandList* ptrCommandList, UINT64 value) noexcept :
	m_ptrCommandList(ptrCommandList),
	m_value(value)
{}

bool DX::XCommandList::WaitObject::isDone() {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on NULL Object");

	return m_value < m_ptrCommandList->getCurrentItteration();
}