#include "DefHeader.h"
#include "JobWaitObject.h"

Job::WaitObject::WaitObject(IJob* ptrJob) noexcept :
	m_ptrJob(ptrJob),
	m_value(ptrJob->p_finishIndex + 1)
{}

bool Job::WaitObject::isDone() noexcept {
	return m_ptrJob->p_finishIndex >= m_value;
}

void Job::WaitObject::wait() noexcept {
	// Pause until is done
	while (!isDone()) {
		THREAD_PAUSE_FUNCTION();
	}
}

Job::WaitObject::operator bool() noexcept {
	return m_ptrJob != nullptr;
}
