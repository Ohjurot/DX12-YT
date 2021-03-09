#include "DefHeader.h"
#include "Fiber.h"

Fiber::Fiber(Threading::IFiber* ptrImplementation, void* ptrUserData) {
	init(ptrImplementation, ptrUserData);
}

Fiber::~Fiber() {
	release();
}

void Fiber::init(Threading::IFiber* ptrImplementation, void* ptrUserData) {
	EXPP_ASSERT(!m_fiberAdress, "Fiber already created");

	// Set members
	m_ptrImplementation = ptrImplementation;
	m_ptrUserData = ptrUserData;

	// Create fiber
	m_fiberAdress = CreateFiber(0, &_winapi_fiberEntryPoint, this);
	m_state = Threading::ExecutionState::PENDING;
	EVALUATE_HRESULT(GetLastError(), "CreateFiber(...)");
}

void Fiber::release() noexcept {
	if (m_fiberAdress) {
		// Delete and unset fiber
		DeleteFiber(m_fiberAdress);
		m_fiberAdress = NULL;
	}
}

LPVOID Fiber::getAddress() noexcept {
	return m_fiberAdress;
}

Fiber::operator LPVOID() {
	EXPP_ASSERT_DBG(m_fiberAdress, "Invalid fiber address");
	return m_fiberAdress;
}

LPVOID Fiber::getStaticFiberAddress() noexcept {
	return Threading::LocalStorageProvider::getInstance().getFLSData();
}

void Fiber::convertToThread(){
	Threading::LocalStorageProvider::getInstance().setFLSData(nullptr);
	EXPP_ASSERT(ConvertFiberToThread(), "ConvertFiberToThread(...)");
}

int Fiber::fiberEntryPoint_s(Fiber* ptrFiber) {
	// Invoke fiber
	ptrFiber->m_ptrImplementation->fiberExecute(ptrFiber->m_ptrUserData);
	return 0;
}

void __stdcall Fiber::_winapi_fiberEntryPoint(LPVOID _In_ lpParameter) {
	// Exctract fiber
	Fiber* ptrFiber = (Fiber*)lpParameter;
	Threading::LocalStorageProvider::getInstance().setFLSData(ptrFiber->m_fiberAdress);
	
	// Invoke fiber
	ptrFiber->m_result = EXPP::invoke<int>(&fiberEntryPoint_s, ptrFiber);
	
	// Check and store exception
	if (ptrFiber->m_result.failed()) {
		ptrFiber->m_state = Threading::ExecutionState::FAILED;
	}
	else {
		ptrFiber->m_state = Threading::ExecutionState::SUCCEED;
	}

	Threading::LocalStorageProvider::getInstance().setFLSData(nullptr);
}
