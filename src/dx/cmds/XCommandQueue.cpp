#include "DefHeader.h"
#include "XCommandQueue.h"

const D3D12_COMMAND_LIST_TYPE DX::XCommandQueue::getQueueType() {
	return m_type;
}

void DX::XCommandQueue::incrementRef() noexcept {
	// Increment
	m_refCounter++;
}

void DX::XCommandQueue::decrementRef() noexcept {
	// Decrement
	m_refCounter--;
}

void DX::XCommandQueue::release() {
	// Assert counter value
	EXPP_ASSERT(!m_refCounter, "Command queue cannot be release while command lists still reference it!");
	// Call parent
	ScopedComPointer::release();
}

void DX::XCommandQueue::lock() noexcept {
	while (m_executionFlag.test_and_set(std::memory_order_acquire)) {
		THREAD_PAUSE_FUNCTION();
	}
}

void DX::XCommandQueue::unlock() noexcept {
	m_executionFlag.clear(std::memory_order_release);
}

DX::XCommandQueue::XCommandQueue(ID3D12Device* ptrDevice, D3D12_COMMAND_LIST_TYPE type) :
	m_type(type)
{
	// Assert device
	EXPP_ASSERT(ptrDevice, "ID3D12Device device pointer was null");

	// Create Descriptor
	D3D12_COMMAND_QUEUE_DESC qDesc;
	ZeroMemory(&qDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

	// Setup descriptor
	qDesc.Type = type;
	qDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	qDesc.NodeMask = NULL;

	// Create command queue
	EVALUATE_HRESULT(ptrDevice->CreateCommandQueue(&qDesc, IID_PPV_ARGS(to())), "ID3D12Device->CreateCommandQueue(...)");
}

DX::XCommandQueue& DX::XCommandQueue::operator=(XCommandQueue& other) {
	// Assert state
	EXPP_ASSERT(!m_refCounter && !m_comPointer, "Copy is only applicable for empty command queues");

	// Copy data
	m_type = other.m_type;
	ScopedComPointer::operator=(other);

	// Return this
	return *this;
}
