#pragma once

#include <DefHeader.h>
#include <common/Container/FixedSizeQueue.h>
#include <job/system/IJob.h>

#define JOB_QUEUE_SIZE_PER_PRIORITY 1024

namespace Job {
	/// <summary>
	/// Job queue
	/// </summary>
	class JobQueue {
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			JobQueue() = default;

			/// <summary>
			/// Queue a new job execution
			/// </summary>
			/// <param name="jobDesc">Job description</param>
			/// <returns>True is job was queued successfully</returns>
			bool queueJob(JobQueueDesc& jobDesc) noexcept;

			/// <summary>
			/// Retrive the next job
			/// </summary>
			/// <param name="ptrJob">Pointer to be set</param>
			/// <returns>True if pointer was set</returns>
			bool getNextJob(JobQueueDesc* ptrJob) noexcept;

			// Delete unused
			JobQueue(const JobQueue&) = delete;
			void operator=(const JobQueue&) = delete;
		private:
			/// <summary>
			/// Array of queues (one for each priority)
			/// </summary>
			Container::FixedSizeQueue<JobQueueDesc, JOB_QUEUE_SIZE_PER_PRIORITY, nullptr> m_arrQueue[JOB_PRIORITY_COUNT];
	};
}