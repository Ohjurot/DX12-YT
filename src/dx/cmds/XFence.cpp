#include "DefHeader.h"
#include "XFence.h"
#include "XCommandList.h"

DX::XFence::XFence(ID3D12Device* ptrDevice) {
	// Create fence
	EVALUATE_HRESULT(ptrDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(to())), "ID3D12Device->CreateFence(...)");
}

UINT64 DX::XFence::getValue() noexcept {
	// Return completed value or invalid value
	return (m_comPointer ? m_comPointer->GetCompletedValue() : UINT64_MAX);
}

void DX::XFence::signal(UINT64 value) {
	EXPP_ASSERT(m_comPointer, "ID3D12Fence pointer was null");
	EVALUATE_HRESULT(m_comPointer->Signal(value), "ID3D12Fence->Signal(...)");
}

DX::XFence::operator UINT64() noexcept {
	return getValue();
}

DX::XFence::WaitObject DX::XFence::createWaitObject(UINT64 value) {
	EXPP_ASSERT(m_comPointer, "Cannot create waitobject for null fence");

	// Create wait object for this fence and value
	return WaitObject(*this, value);
}

DX::XFence::WaitObject::WaitObject(XFence& refFence, UINT64 value) :
	m_fence(refFence),
	m_value(value)
{
	EXPP_ASSERT(m_fence.getValue() != UINT64_MAX, "Invalid fence");
}

bool DX::XFence::WaitObject::isDone() noexcept {
	return (UINT64)m_fence >= m_value;
}

void DX::XFence::WaitObject::wait() {
	EXPP_ASSERT(m_fence.getValue() != UINT64_MAX, "Invalid fence value");

	while (!isDone()) {
		THREAD_PAUSE_FUNCTION();
		EXPP_ASSERT_DBG(m_fence.getValue() != UINT64_MAX, "Invalid fence value");
	}
}

DX::XFence::WaitObject::operator bool() noexcept {
	return isDone();
}

void DX::XFence::WaitObject::operator()(){
	wait();
}
