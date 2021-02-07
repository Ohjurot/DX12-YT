#include "DefHeader.h"
#include "XInfoQueue.h"

DX::XInfoQueue::XInfoQueue(ID3D12Device* ptrDevice) {
	// Query device for debug device (only if debug is enabled)
	if (DX::XDebug::getInstance().isEnabled()) {
		EVALUATE_HRESULT(ptrDevice->QueryInterface(IID_PPV_ARGS(to())), "ID3D12Device->QueryInterface(...) for ID3D12InfoQueue");
	}
}

uint64_t DX::XInfoQueue::getMessageCount() noexcept{
	// If pointer is valid return num stored messages else 0
	return (m_comPointer ? m_comPointer->GetNumStoredMessages() : 0);
}

size_t DX::XInfoQueue::getMessage(uint64_t index, D3D12_MESSAGE* ptrMessage) noexcept {
	// Set message length to 0
	size_t messageLength = 0;

	// If pointer exists get message
	if (m_comPointer) {
		m_comPointer->GetMessage(index, ptrMessage, &messageLength);
	}
	
	// Return length of message
	return messageLength;
}

DWORD DX::XInfoQueue::registerCallback(D3D12MessageFunc fptrMessageFunction, void* functionData, D3D12_MESSAGE_CALLBACK_FLAGS flags) noexcept {
	// Get ID3D12InfoQueue1 or return NULL
	ScopedComPointer<ID3D12InfoQueue1> ptrInfoQueue1;
	if (!queryInterface(ptrInfoQueue1)) {
		return NULL;
	}

	// Register message
	DWORD cookie = NULL;
	ptrInfoQueue1->RegisterMessageCallback(fptrMessageFunction, flags, functionData, &cookie);

	// Return cookie
	return cookie;
}

bool DX::XInfoQueue::unregisterCallback(DWORD cookie) noexcept {
	// Get ID3D12InfoQueue1 or return
	ScopedComPointer<ID3D12InfoQueue1> ptrInfoQueue1;
	if (!queryInterface(ptrInfoQueue1)) {
		return false;
	}

	// Unregister callback
	return SUCCEEDED(ptrInfoQueue1->UnregisterMessageCallback(cookie));
}
