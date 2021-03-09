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
			/// <param name="fiberAddress"></param>
			/// <returns>JobFiber pointer</returns>
			JobFiber* notifyFiberCreation(LPVOID fiberAddress) noexcept;

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
			Memory::QuadTreePoolAllocator<JobFiber, JOB_NUM_FIBERS> m_allocator;

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