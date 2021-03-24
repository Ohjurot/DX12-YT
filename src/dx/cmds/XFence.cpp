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
	m_ptrFence(&refFence),
	m_value(value)
{
	EXPP_ASSERT(m_ptrFence && m_ptrFence->getValue() != UINT64_MAX, "Invalid fence");
}

bool DX::XFence::WaitObject::isDone() noexcept {
	return m_ptrFence->getValue() >= m_value;
}

void DX::XFence::WaitObject::wait() {
	EXPP_ASSERT(m_ptrFence && m_ptrFence->getValue() != UINT64_MAX, "Invalid fence value");

	if (!isDone()) {
		// Gennerate event
		ScopedHandle e = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (e) {
			if (SUCCEEDED(m_ptrFence->m_comPointer->SetEventOnCompletion(m_value, e))) {
				// Wait for event
				DWORD waitResult;
				while ((waitResult = WaitForSingleObject(e, 0)) == WAIT_TIMEOUT) {
					THREAD_PAUSE_FUNCTION();
				}

				// Check if done
				if (waitResult == WAIT_OBJECT_0) {
					return;
				}
			}
		}

		// Fallback wait
		while (!isDone()) {
			THREAD_PAUSE_FUNCTION();
			EXPP_ASSERT_DBG(m_ptrFence && m_ptrFence->getValue() != UINT64_MAX, "Invalid fence value");
		}
	}
}

UINT64 DX::XFence::WaitObject::value() noexcept {
	return m_value;
}

void DX::XFence::WaitObject::operator()() {
	wait();
}

bool DX::XFence::WaitObject::operator==(const WaitObject& other) {
	return other.m_ptrFence == m_ptrFence;
}

DX::XFence::WaitObject::operator bool() noexcept {
	return m_ptrFence;
}
