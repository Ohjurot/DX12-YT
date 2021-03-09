#pragma once

#include <DefHeader.h>
#include <job/system/JobData.h>

#define JOB_CLASS(className) class className : public Job::IJob
#define JOB_INTERFACE public
#define JOB_DATA private

#define JOB_PREPARE_FUNCTION(...) protected: Job::JobPrepareDesc prepare(__VA_ARGS__) override
#define JOB_EXECUTE_FUNCTION(...) protected: Job::JobReturnValue execute(__VA_ARGS__) override

namespace Job {
	// Predecl
	class JobSystem;
	class PreparedJob;
	class WaitObject;

	/// <summary>
	/// Basic job interface
	/// </summary>
	class IJob {
		public:
			/// <summary>
			/// Checks weather the job has dependencys or not
			/// </summary>
			/// <returns>true if dependencys exist</returns>
			bool hasDependencys() noexcept;
			
			/// <summary>
			/// Increment the depedency counter
			/// </summary>
			void incrementDependencyCounter() noexcept;

			/// <summary>
			/// Decrement the depedency counter
			/// </summary>
			void decrementDependencyCounter() noexcept;

		protected:
			friend class Job::JobSystem;
			friend class Job::PreparedJob;
			friend class Job::WaitObject;

			/// <summary>
			/// Job prepare function
			/// </summary>
			/// <param name="args">Arguments</param>
			/// <returns>Job preparation meta</returns>
			virtual Job::JobPrepareDesc prepare(void* args);

			/// <summary>
			/// Job execution function (pure virtual)
			/// </summary>
			/// <param name="ptrUserData">Userdata pointer</param>
			/// <param name="invocationIndex">Invocation index</param>
			virtual Job::JobReturnValue execute(const unsigned int invocationIndex) = 0;
			
			/// <summary>
			/// Current finish index;
			/// </summary>
			UINT64 p_finishIndex = 0;

		private:
			/// <summary>
			/// Dependency counter
			/// </summary>
			std::atomic<UINT64> m_dependancyCounter = 0;
	};
}
