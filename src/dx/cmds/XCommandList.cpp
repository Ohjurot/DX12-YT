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

	// Releaee own
	ScopedComPointer::release();

	// Release allocator
	m_allocator.release();
	m_fenceFrontend.release();

	// Set state
	m_state = XCommandList_State::INVALID;

	// Unreference queue
	if (m_ptrCommandQueue) {
		m_ptrCommandQueue->decrementRef();
		m_ptrCommandQueue = nullptr;
	}
}

DX::XCommandList_State DX::XCommandList::getState() {
	// Refresh state
	refresh();

	// Return new / old state
	return m_state;
}

void DX::XCommandList::refresh(){
	static std::atomic_flag guard = ATOMIC_FLAG_INIT;
	while (guard.test_and_set(std::memory_order_acquire)) {
		THREAD_PAUSE_FUNCTION();
	}

	// Check if list is done executing
	if (m_state == XCommandList_State::QUEUE_EXECUTING && m_fenceFrontend.getCurrentWaitObject().isDone()) {
		// Reset all
		EVALUATE_HRESULT(m_allocator->Reset(), "ID3D12CommandAllocator->Reset(...)");
		EVALUATE_HRESULT(m_comPointer->Reset(m_allocator, NULL), "ID3D12GraphicsCommandList->Reset(...)");
		// Is back open
		m_state = XCommandList_State::OPEN;
		// Increment iteration
		m_uiIteration++;
	}

	guard.clear(std::memory_order_release);
}

DX::XCommandList::WaitObject DX::XCommandList::close() {
	EXPP_ASSERT(getState() == XCommandList_State::OPEN, "Close can only be called on an open command list");

	// Close list
	EVALUATE_HRESULT(m_comPointer->Close(), "ID3D12GraphicsCommandList->Close(...)");
	m_state = XCommandList_State::CLOSED;

	// Return wait object
	return getUniversalWaitObject();
}

DX::XFence::WaitObject DX::XCommandList::execute(XFence* ptrArrFence) {
	// Directly return if has dependancys
	if (hasDependencys()) {
		return XFence::WaitObject();
	}

	// Const retrive and assert
	const int thisTypeIndex = DX::CommandQueueManager::getInstance().indexAtType(m_ptrCommandQueue->getQueueType());

	EXPP_ASSERT(getState() == XCommandList_State::CLOSED, "Execute can only be called on a close command list");
	EXPP_ASSERT_DBG(thisTypeIndex >= 0, "Invalid type index");

	// LOCK COMMAND QUEUE
	m_ptrCommandQueue->lock();

	// Gennerate waits
	for (unsigned int i = 0; i < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; i++) {
		// Only for differente type
		if (thisTypeIndex != i && m_arrWaitDependency[i]) {
			m_ptrCommandQueue->ptr()->Wait(ptrArrFence[i], m_arrWaitDependency[i]);
		}
	}

	// Execute and signal 
	m_ptrCommandQueue->ptr()->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_comPointer);
	m_ptrCommandQueue->ptr()->Signal(m_fenceFrontend, m_fenceFrontend.next());
	
	// UNLOCK COMMAND LIST
	m_ptrCommandQueue->unlock();

	// Clear required waits
	for (unsigned int i = 0; i < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; i++) {
		m_arrWaitDependency[i].store(0, std::memory_order_relaxed);
	}

	// Get dependants list
	auto dependantsList = m_dependants.get();

	// Resolve other
	while (dependantsList->dependantsCount) {
		dependantsList->arrPtrDependants[dependantsList->dependantsCount - 1]->resolveDependency(ptrArrFence, thisTypeIndex, m_fenceFrontend.head());
		// Decrement
		dependantsList->dependantsCount--;
	}

	// Release dependants list
	dependantsList.release();

	// Set state
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

bool DX::XCommandList::hasDependencys() noexcept {
	return m_dependencysCount > 0;
}

void DX::XCommandList::injectDependency(WaitObject& dependancy) {
	EXPP_ASSERT_DBG(dependancy.m_ptrCommandList != this || dependancy.m_value <= m_uiIteration, "Cannot depend on same list with an higher itteration count!");

	// Dependency will only count if not this and still not done
	if (dependancy.m_ptrCommandList != this && !dependancy.isDone()) {
		// Inject into other
		EXPP_ASSERT(dependancy.m_ptrCommandList->insertDependant(dependancy, this), "Dependency Injection not possible: \nThe target command list has already a full list of dependants.");
	}
}

void DX::XCommandList::resolveDependency(XFence* arrFences, unsigned int fencePage, UINT64 fenceValue) {
	EXPP_ASSERT_DBG(hasDependencys(), "Resolve dependancy was called on an command list with no dependancys but the state was closed");

	// Resolve own
	setResolvedDependency(fencePage, fenceValue);
	
	// Resolve own is only required if self has dependencys and is in close
	if (--m_dependencysCount == 0 && m_state == XCommandList_State::CLOSED) {
		// Dispatch own
		execute(arrFences);
	}
}

void DX::XCommandList::setResolvedDependency(unsigned int fencePage, UINT64 fenceValue) {
	// Set own dependency
	UINT64 oldV, newV;
	do {
		oldV = m_arrWaitDependency[fencePage].load(std::memory_order_relaxed);
		newV = std::max(fenceValue, oldV);
	} while (!m_arrWaitDependency[fencePage].compare_exchange_strong(oldV, newV, std::memory_order_relaxed));
}

bool DX::XCommandList::insertDependant(WaitObject& waitObject, XCommandList* ptrCmdList) {
	// Get dependants list
	auto dependantsList = m_dependants.get();

	// Check range
	XFence::WaitObject wo = waitObject.convertToFenceWaitObject();
	bool success = (dependantsList->dependantsCount < dependantsList->size()) || wo;
	if (success && !wo) {
		// Insert
		dependantsList->arrPtrDependants[dependantsList->dependantsCount] = ptrCmdList;
		dependantsList->dependantsCount++;
		ptrCmdList->m_dependencysCount++;
	}
	// Check if strong insert may be required
	else if (wo) {
		// Get list index
		int listTypeIndex = DX::CommandQueueManager::getInstance().indexAtType(m_comPointer->GetType());
		EXPP_ASSERT_DBG(listTypeIndex >= 0, "Type index out of range");

		// Set dependancy
		ptrCmdList->setResolvedDependency(listTypeIndex, m_fenceFrontend.head());
	}

	// Return
	return success;
}

DX::XCommandList::WaitObject::WaitObject(XCommandList* ptrCommandList, UINT64 value) noexcept :
	m_ptrCommandList(ptrCommandList),
	m_value(value)
{}

bool DX::XCommandList::WaitObject::isDone() {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on NULL Object");

	m_ptrCommandList->refresh();
	return m_value < m_ptrCommandList->getCurrentItteration();
}

void DX::XCommandList::WaitObject::wait() {
	// While not done pause
	while (!isDone()) {
		THREAD_PAUSE_FUNCTION();
	}
}

UINT64 DX::XCommandList::WaitObject::value() noexcept{
	return m_value;
}

DX::XFence::WaitObject DX::XCommandList::WaitObject::convertToFenceWaitObject() {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on NULL Object");
	
	// Check if state is valid
	if (
		(m_ptrCommandList->getState() == XCommandList_State::QUEUE_EXECUTING) ||
		(m_value < m_ptrCommandList->getCurrentItteration())
	) {
		// Return fence wait object
		return m_ptrCommandList->getExecutionWaitObject();
	}
	
	// Else return null wait object
	return XFence::WaitObject();
}

void DX::XCommandList::WaitObject::operator()() {
	wait();
}

bool DX::XCommandList::WaitObject::operator==(const WaitObject& other) {
	return other.m_ptrCommandList == m_ptrCommandList;
}

DX::XCommandList::WaitObject::operator bool() {
	return m_ptrCommandList;
}
