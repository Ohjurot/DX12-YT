#pragma once

#include <DefHeader.h>
#include <job/threading/Thread.h>
#include <job/threading/Fiber.h>
#include <job/threading/MultiThreadDispatcher.h>
#include <job/system/JobFiberPool.h>
#include <job/system/JobQueue.h>

#include <common/Time/StopWatch.h>

#include <string>
#include <sstream>

namespace Job {
	class SystemInstance;

	/// <summary>
	/// Job System
	/// </summary>
	class JobSystem : public Threading::IThread, public Threading::IFiber{
		public:
			/// <summary>
			/// Create a job system
			/// </summary>
			/// <param name="threadCount">Number of worker threads "0 >=" means automatic</param>
			JobSystem(int threadCount = 0);

			/// <summary>
			/// Destruct
			/// </summary>
			~JobSystem();

			/// <summary>
			/// Stage a job for execution
			/// </summary>
			/// <param name="description">Job description</param>
			/// <returns>true if stage succeded</returns>
			bool stageExecution(JobQueueDesc& description) noexcept;

			/// <summary>
			/// Notify exit condition
			/// </summary>
			void exit() noexcept;

			/// <summary>
			/// Waits for the job system
			/// </summary>
			void wait();

		protected:
			friend class Job::SystemInstance;

			/// <summary>
			/// Pause function to pause a job / thread pause
			/// </summary>
			void jobPauseFunction();

			/// <summary>
			/// Thread execution function
			/// </summary>
			/// <param name="parameter">Input user parameter</param>
			/// <returns>Thread return code</returns>
			DWORD threadExecute(void* parameter) override;

			/// <summary>
			/// Fiber execution function
			/// </summary>
			/// <param name="parameter">Thread</param>
			void fiberExecute(void* parameter) override;

			/// <summary>
			/// Job system execute 
			/// </summary>
			void jobSystemExecute(JobFiber* ptrFiber);

		private:
			/// <summary>
			/// Multithread object
			/// </summary>
			Threading::MultiThreadDispatcher* m_ptrMtd;

			/// <summary>
			/// Fiber array
			/// </summary>
			Fiber m_arrFibers[JOB_NUM_FIBERS];

			/// <summary>
			/// Number of jobs currently in system
			/// </summary>
			std::atomic<UINT64> m_jobCount = 0;

			/// <summary>
			/// Fiber pool
			/// </summary>
			JobFiberPool m_fiberPool;

			/// <summary>
			/// Job queue
			/// </summary>
			JobQueue m_jobQueue;

			/// <summary>
			/// FLS Cookie for current job pointer
			/// </summary>
			DWORD m_flsJobCookie;

			/// <summary>
			/// Running condition of system
			/// </summary>
			bool m_bKeepRunning = true;
	};
}