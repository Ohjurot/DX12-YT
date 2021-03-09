#include "DefHeader.h"
#include "JobQueue.h"

bool Job::JobQueue::queueJob(JobQueueDesc& jobDesc) noexcept {
    switch (jobDesc.jobPriority) {
        // LOW
        case JobPriority::LOW:
            return m_arrQueue[4].push<JobQueueDesc>(jobDesc);

        // NORMAL
        case JobPriority::NORMAL:
            return m_arrQueue[3].push<JobQueueDesc>(jobDesc);

        // MEDIUM
        case JobPriority::MEDIUM:
            return m_arrQueue[2].push<JobQueueDesc>(jobDesc);

        // HIGH
        case JobPriority::HIGH:
            return m_arrQueue[1].push<JobQueueDesc>(jobDesc);

        // CRITICAL
        case JobPriority::CRITICAL:
            return m_arrQueue[0].push<JobQueueDesc>(jobDesc);

    }

    // Fallback
    return false;
}

bool Job::JobQueue::getNextJob(JobQueueDesc* ptrJob) noexcept {
    // Start at highst prio to lows
    for (unsigned int i = 0; i < JOB_PRIORITY_COUNT; i++) {
        // Pop while element is valid
        auto popElement = m_arrQueue[i].pop();
        // Check if element is valid
        if (popElement) {
            // Take over and return
            popElement.takeover(ptrJob);
            return true;
        }
    }

    // Fallback false
    return false;
}
