#include "DefHeader.h"
#include "IJob.h"

bool Job::IJob::hasDependencys() noexcept {
    return m_dependancyCounter.load(std::memory_order_relaxed) > 0;
}

void Job::IJob::incrementDependencyCounter() noexcept {
    m_dependancyCounter++;
}

void Job::IJob::decrementDependencyCounter() noexcept {
    m_dependancyCounter--;
}

Job::JobPrepareDesc Job::IJob::prepare(void* args) {
    return Job::JobPrepareDesc();
}
