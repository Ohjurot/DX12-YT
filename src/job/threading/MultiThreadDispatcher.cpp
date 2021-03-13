#include "DefHeader.h"
#include "MultiThreadDispatcher.h"

Threading::MultiThreadDispatcher::MultiThreadDispatcher(unsigned int numThreads, Threading::IThread* ptrInterface, void* userdata) :
	m_cNumThreads(numThreads),
	m_ptrInterface(ptrInterface)
{
	// Alloc data
	m_arrThreads = (Thread*)_malloca(sizeof(Thread) * m_cNumThreads);
	m_arrThreadsData = (MTD_ThreadArgs*)_malloca(sizeof(MTD_ThreadArgs) * m_cNumThreads);

	// Assert
	EXPP_ASSERT(m_arrThreads && m_arrThreadsData, "Stack alloc failed");

	// Zero Data
	ZeroMemory(m_arrThreads, sizeof(Thread) * m_cNumThreads);

	// Fill and create threads
	for (unsigned int i = 0; i < m_cNumThreads; i++) {
		// Create data
		m_arrThreadsData[i].threadIndex = i;
		m_arrThreadsData[i].ptrUserData = userdata;

		// Dispatch thread
		m_arrThreads[i].init(this, &m_arrThreadsData[i], i);
	}
}

Threading::MultiThreadDispatcher::~MultiThreadDispatcher() {
	for (unsigned int i = 0; i < m_cNumThreads; i++) {
		// Join / Kill
		if (!m_arrThreads[i].join(MAX_THREAD_TIMEOUT_TIME_MS)) {
			m_arrThreads[i].kill();
		}
	}

	// Unset data
	_freea(m_arrThreads);
	_freea(m_arrThreadsData);
}

void Threading::MultiThreadDispatcher::joinAll(DWORD joinTimePerThread) {
	// Join all
	for (unsigned int i = 0; i < m_cNumThreads; i++) {
		m_arrThreads[i].join(joinTimePerThread);
	}
}

void Threading::MultiThreadDispatcher::killAll() {
	// kill all
	for (unsigned int i = 0; i < m_cNumThreads; i++) {
		m_arrThreads[i].kill();
	}
}

void Threading::MultiThreadDispatcher::terminateKillWait(DWORD joinTimePerThread) {
	// Join / kill all
	for (unsigned int i = 0; i < m_cNumThreads; i++) {
		if (!m_arrThreads[i].join(joinTimePerThread)) {
			m_arrThreads[i].kill();
		}
	}
}

bool Threading::MultiThreadDispatcher::keepUpThreads() {
	// Exeception flag
	bool hasExeption = false;

	// Number of running threads
	unsigned int threadRunCount = m_cNumThreads;

	// Check all threads
	for (unsigned int i = 0; i < m_cNumThreads; i++) {
		// Check if thread is no more running and has execption
		if (m_arrThreads[i].getResult() != Threading::ExecutionState::PENDING) {
			// Decrement run count
			threadRunCount--;

			// Check if force termination is required
			if (m_arrThreads[i].getResult() == Threading::ExecutionState::FAILED) {
				hasExeption = true;
			}
		}
	}

	// Check if terminates now regularay
	if (threadRunCount == 0 || hasExeption) {
		return false;
	}

	return true;
}

void Threading::MultiThreadDispatcher::raisExeception() {
	if (m_storedResult.failed()) {
		// m_storedResult.rais();
	}
}

DWORD Threading::MultiThreadDispatcher::threadExecute(void* parameter) {
	// Call impl
	return m_ptrInterface->threadExecute(parameter);
}

void Threading::MultiThreadDispatcher::threadHandleExeception(EXPP::InvocationResult<DWORD>& refResult) noexcept {
	// Call impl
	m_ptrInterface->threadHandleExeception(refResult);

	// Check if threads need to terminate
	if (refResult.failed() && !m_exceptionFlag.test_and_set(std::memory_order_acquire)) {
		common::EXPPHandler::reportExeption(refResult);
		m_storedResult = refResult;
	}
}
