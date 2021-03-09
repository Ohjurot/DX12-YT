#include "DefHeader.h"
#include "Thread.h"

Thread::Thread(Threading::IThread* ptrImplementation, void* ptrUserData, int targetLogicalCore) {
    // Init thread
    init(ptrImplementation, ptrUserData, targetLogicalCore);
}

Thread::~Thread() {
    // Join thread or kill is after 5s
    if (!join(MAX_THREAD_TIMEOUT_TIME_MS)) {
        kill();
    }
}

void Thread::init(Threading::IThread* ptrImplementation, void* ptrUserData, int targetLogicalCore) {
    EXPP_ASSERT(!m_hThread, "Thread is already valid");

    // Assign
    m_ptrImplementation = ptrImplementation;
    m_ptrUserThreadData = ptrUserData;

    // Create thread
    m_hThread = CreateThread(NULL, NULL, &Thread::_winapi_threadEntryPoint, this, CREATE_SUSPENDED, &m_dwThreadId);
    EVALUATE_HRESULT(GetLastError(), "Thread creation failed!");
    EXPP_ASSERT(m_hThread, "Thread handle was null!");

    // Set threads affinity
    if (targetLogicalCore >= 0) {
        SetThreadAffinityMask(m_hThread, (1ULL << targetLogicalCore));
    }

    // Set thread priority Above normal
    EXPP_ASSERT(SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL), "SetThreadPriority(...) width THREAD_PRIORITY_ABOVE_NORMAL failed!");

    // Set state
    m_executionResult = Threading::ExecutionState::PENDING;

    // Dispatch thread
    ResumeThread(m_hThread);

    // Wait for thread creation
    while (!m_creationFlag.test(std::memory_order_relaxed));
}

bool Thread::join(DWORD maxWaitTime) noexcept {
    // Return true if already closed
    if (!m_hThread) {
        return true;
    }

    // Wait for thread
    if (WaitForSingleObject(m_hThread, maxWaitTime) == WAIT_OBJECT_0) {
        CloseHandle(m_hThread);
        m_hThread = 0;
        return true;
    }

    // Falback false
    return false;
}

void Thread::kill() noexcept {
    if (isRunning()) {
        m_executionResult = Threading::ExecutionState::FORCE_TERMINATED;
        TerminateThread(m_hThread, -1);
        CloseHandle(m_hThread);
        m_hThread = 0;
    }
}

Threading::ExecutionState Thread::getResult() noexcept {
    return m_executionResult;
}

bool Thread::isRunning() noexcept {
    // Check if running
    if (m_hThread) {
        return WaitForSingleObject(m_hThread, 0) == WAIT_OBJECT_0;
    }

    // Return false not running
    return false;
}

void Thread::raisThreadException() {
    // Rais execpeiton
    m_result.rais();
    m_result.handle();
}

DWORD Thread::getThreadId() noexcept {
    if (isRunning()) {
        return m_dwThreadId;
    }

    return 0;
}

LPVOID Thread::getFiberAddress() noexcept {
    return m_relatedFiberAddress;
}

Thread::operator HANDLE() noexcept {
    return m_hThread;
}

Thread& Thread::get() noexcept {
    return *((Thread*)Threading::LocalStorageProvider::getInstance().getTLSData());
}

void Thread::convertToFiber() {
    // Get thread pointer
    Thread* ptrThread = (Thread*)Threading::LocalStorageProvider::getInstance().getTLSData();

    // Assert states
    EXPP_ASSERT_DBG(ptrThread && ptrThread->getFiberAddress() == nullptr, "Invalid thread or thread is already a fiber");

    // Convert to fiber
    ptrThread->m_relatedFiberAddress = ConvertThreadToFiber(NULL);
    
    // Indicate that fiber has no parent container ("root" fiber)
    Threading::LocalStorageProvider::getInstance().setFLSData(ptrThread->m_relatedFiberAddress);
}

DWORD Thread::threadEntryPoint() {
    // Setup thread data
    Threading::LocalStorageProvider::getInstance().setTLSData(this);

    // Call user function
    return m_ptrImplementation->threadExecute(m_ptrUserThreadData);
}

DWORD Thread::threadEntryPoint_safe(Thread* ptrThread) {
    // Thread created
    ptrThread->m_creationFlag.test_and_set(std::memory_order_acquire);
    
    // Call typed entry point
    return ptrThread->threadEntryPoint();
}

DWORD __stdcall Thread::_winapi_threadEntryPoint(LPVOID _In_ lpParameter) {
    // Cast to thread
    Thread* pThread = (Thread*)lpParameter;
    
    // Call oop function
    pThread->m_result = EXPP::invoke<DWORD>(&threadEntryPoint_safe, pThread);

    // Check return code
    if (pThread->m_result.succeeded()) {
        pThread->m_executionResult = Threading::ExecutionState::SUCCEED;
        return pThread->m_result.returnValue();
    }

    // Call handler
    pThread->m_executionResult = Threading::ExecutionState::FAILED;
    pThread->m_ptrImplementation->threadHandleExeception(pThread->m_result);

    // Return -1
    return -1;
}
