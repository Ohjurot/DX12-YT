#include "DefHeader.h"
#include "CommandListAccessObject.h"

DX::CommandListAccessObject::CommandListAccessObject(D3D12_COMMAND_LIST_TYPE type) {
	// Retrive command list pointer
	m_ptrCommandList = DX::CommandListManager::getInstance().lockList(type);
}

DX::CommandListAccessObject::~CommandListAccessObject() {
	release();
}

void DX::CommandListAccessObject::release() {
	// Execute close
	if (m_ptrCommandList) {
		executeClose();
	}
}

DX::XCommandList::WaitObject DX::CommandListAccessObject::executeExchange() {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on empty access object");

	// Extract type
	D3D12_COMMAND_LIST_TYPE type = m_ptrCommandList->ptr()->GetType();

	// Execute close
	XCommandList::WaitObject wo = executeClose();

	// Get new
	m_ptrCommandList = DX::CommandListManager::getInstance().lockList(type);

	// Return wo
	return wo;
}

DX::XCommandList::WaitObject DX::CommandListAccessObject::executeClose() {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on empty access object");
	
	// NULL wait object
	XCommandList::WaitObject wo;
	
	// Only is list was used
	if (m_bIsDirty) {
		// Close list
		wo = m_ptrCommandList->close();
	}

	// Execute return
	DX::CommandListManager::getInstance().unlockList(m_ptrCommandList, m_bIsDirty);
	m_ptrCommandList = nullptr;
	m_bIsDirty = false;

	// Return wait object
	return wo;
}

DX::XCommandList::WaitObject DX::CommandListAccessObject::createWaitObject() {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on empty access object");

	// Now externaly referenced -> execute required
	m_bIsDirty = true;

	// return wait object
	return m_ptrCommandList->getUniversalWaitObject();
}

void DX::CommandListAccessObject::addDependency(XCommandList::WaitObject& waitObject) {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on empty access object");

	m_bIsDirty = true;

	m_ptrCommandList->injectDependency(waitObject);
}

ID3D12GraphicsCommandList* DX::CommandListAccessObject::operator->() {
	EXPP_ASSERT(m_ptrCommandList, "Invalid call on empty access object");
	
	// Mark dirty
	m_bIsDirty = true;
	
	// Return pointer
	return m_ptrCommandList->ptr();
}

DX::CommandListAccessObject::operator ID3D12GraphicsCommandList* () {
	return operator->();
}

DX::CommandListAccessObject::operator bool() noexcept {
	return m_ptrCommandList;
}
