#pragma once

#include <DefHeader.h>
#include <job/system/JobFiber.h>
#include <common/Container/FixedSizeQueue.h>

#define JOB_NUM_FIBERS 256

namespace Job {
	class JobFiberPool {
		public:
			/// <summary>
			/// Notify the pool that a fiber was created
			/// </summary>
			/// <returns>JobFiber pointer</returns>
			JobFiber* notifyFiberCreation() noexcept;

			/// <summary>
			/// Enable a fiber
			/// </summary>
			/// <param name="description">Job description</param>
			/// <returns>true if a fiber was able to be loaded</returns>
			bool enableFiber(JobQueueDesc& description) noexcept;

			/// <summary>
			/// Return fiber to pool
			/// </summary>
			/// <param name="ptrFiber">Pointer to fiber</param>
			/// <returns>true if fiber was able to be handeld</returns>
			bool returnFiber(JobFiber* ptrFiber) noexcept;

			/// <summary>
			/// Take a fiber from the pool for execution
			/// </summary>
			/// <param name="ppFiber">Pointer to pointer to be set</param>
			/// <returns>true if pointer was set</returns>
			bool takeFiber(JobFiber** ppFiber) noexcept;

		private:
			/// <summary>
			/// JobFiber allocator
			/// </summary>
			JobFiber m_allocator[JOB_NUM_FIBERS];

			/// <summary>
			/// Current usage of allocator
			/// </summary>
			std::atomic<unsigned int> m_allocatorUsage = 0;

			/// <summary>
			/// Idle fibers
			/// </summary>
			Container::FixedSizeQueue<JobFiber*, JOB_NUM_FIBERS, nullptr> m_idleFiber;

			/// <summary>
			/// Array of all active fibers ordered by priority
			/// </summary>
			Container::FixedSizeQueue<JobFiber*, JOB_NUM_FIBERS, nullptr> m_arrActiveFibers[JOB_PRIORITY_COUNT];
	};
}