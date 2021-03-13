#include "DefHeader.h"
#include "JobFiberPool.h"

Job::JobFiber* Job::JobFiberPool::notifyFiberCreation() noexcept {
	// Allocate
	if (m_allocatorUsage < JOB_NUM_FIBERS) {
		// Store persist
		JobFiber* ptrFiber = &m_allocator[m_allocatorUsage++];
		ptrFiber->m_address = 0;
		ptrFiber->queuDesc = JobQueueDesc();

		// Push and return
		m_idleFiber.push<JobFiber*>(ptrFiber);
		return ptrFiber;
	}

	// Fallback false
	return nullptr;
}

bool Job::JobFiberPool::enableFiber(JobQueueDesc& description) noexcept {
	// Get element
	auto popElement = m_idleFiber.pop();
	if (popElement) {
		// Takeover fiber
		JobFiber* m_ptrFiber;
		if (popElement.takeover(&m_ptrFiber)) {
			m_ptrFiber->queuDesc = description;
			returnFiber(m_ptrFiber);
			return true;
		}
	}

	// Fallback
	return false;
}

bool Job::JobFiberPool::returnFiber(JobFiber* ptrFiber) noexcept {
	// Check fiber activity
	if (ptrFiber->queuDesc.ptrJob == nullptr) {
		return m_idleFiber.push<JobFiber*>(ptrFiber);
	}

	// Priority
	switch (ptrFiber->queuDesc.jobPriority) {
		// LOW
        case JobPriority::LOW:
            return m_arrActiveFibers[4].push<JobFiber*>(ptrFiber);

        // NORMAL
        case JobPriority::NORMAL:
            return m_arrActiveFibers[3].push<JobFiber*>(ptrFiber);

        // MEDIUM
        case JobPriority::MEDIUM:
            return m_arrActiveFibers[2].push<JobFiber*>(ptrFiber);

        // HIGH
        case JobPriority::HIGH:
            return m_arrActiveFibers[1].push<JobFiber*>(ptrFiber);

        // CRITICAL
        case JobPriority::CRITICAL:
            return m_arrActiveFibers[0].push<JobFiber*>(ptrFiber);
	}

	// Fallback
	return false;
}

bool Job::JobFiberPool::takeFiber(JobFiber** ppFiber) noexcept {
	for (unsigned int i = 0; i < JOB_PRIORITY_COUNT; i++) {
		auto popElement = m_arrActiveFibers[i].pop();
		if (popElement) {
			popElement.takeover(ppFiber);
			return true;
		}
	}
	
	// Fallback
	return false;
}
