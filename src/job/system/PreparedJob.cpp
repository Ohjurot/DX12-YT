#include "DefHeader.h"
#include "PreparedJob.h"

Job::PreparedJob::PreparedJob(PreparedJob&& other) noexcept :
	m_prepareDesc(std::move(other.m_prepareDesc)),
	m_ptrJob(other.m_ptrJob),
	m_bValid(other.m_bValid)
{
	other.m_bValid = false;
}

Job::PreparedJob::PreparedJob(IJob* ptrJob, JobPrepareDesc& prepareDescitpion) noexcept :
	m_ptrJob(ptrJob),
	m_prepareDesc(std::move(prepareDescitpion))
{
	// Check if valid
	m_bValid = m_ptrJob != nullptr;
}

Job::PreparedJob& Job::PreparedJob::operator=(PreparedJob && other) noexcept {
	m_prepareDesc = std::move(other.m_prepareDesc);
	m_ptrJob = other.m_ptrJob;
	m_bValid = other.m_bValid;
	other.m_bValid = false;
	return *this;
}

Job::WaitObject Job::PreparedJob::stageExecution() noexcept {
	// Early return if not valid
	if (!m_bValid) {
		return WaitObject();
	}

	// Stage all jobs
	for (unsigned i = 0; i < m_prepareDesc.numThreads; i++) {
		// Build job queue description
		JobQueueDesc qd;
		qd.dispatchIndex = i;
		qd.jobPriority = m_prepareDesc.priority;
		qd.ptrJob = m_ptrJob;

		// Output wait object
		WaitObject wo(m_ptrJob);

		// Kickoff job
		if (SystemInstance::getSystem()->stageExecution(qd)) {
			return wo;
		}
	}

	// Previously gennerated wait object
	return WaitObject();
}

Job::PreparedJob::operator bool() noexcept {
	return m_bValid;
}

Job::PreparedJob Job::PreparedJob::prepare(Job::IJob& job, void* args) noexcept {
	// Execute prapare
	JobPrepareDesc prepareResult = job.prepare(args);
	if (prepareResult.numThreads) {
		return PreparedJob(&job, prepareResult);
	}

	// Default empty object
	return PreparedJob();
}
