#pragma once

#include <DefHeader.h>
#include <job/system/IJob.h>
#include <job/system/JobData.h>
#include <job/system/SystemInstance.h>
#include <job/system/JobWaitObject.h>

namespace Job {
	/// <summary>
	/// Represents a job redey to be staged and executed
	/// </summary>
	class PreparedJob {
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			PreparedJob() = default;

			/// <summary>
			/// Move constructor
			/// </summary>
			/// <param name="other">Other job</param>
			PreparedJob(PreparedJob&& other) noexcept;

			/// <summary>
			/// Job prepage function
			/// </summary>
			/// <param name="ptrJob">Job pointer</param>
			/// <param name="prepareDescitpion">Job description</param>
			PreparedJob(IJob* ptrJob, JobPrepareDesc& prepareDescitpion) noexcept;

			/// <summary>
			/// Move assign
			/// </summary>
			/// <param name="other">other job</param>
			/// <returns>Ref to self</returns>
			PreparedJob& operator=(PreparedJob&& other) noexcept;

			/// <summary>
			/// Stage job for exectuion and invalidate instance
			/// </summary>
			/// <returns>Job wait object</returns>
			Job::WaitObject stageExecution() noexcept;

			/// <summary>
			/// Bool conversion operator
			/// </summary>
			/// <returns>true if cals is valid</returns>
			operator bool() noexcept;

			/// <summary>
			/// Job prepare function
			/// </summary>
			/// <param name="job">Input job</param>
			/// <param name="args">Job arguments</param>
			/// <returns>Prepared job</returns>
			static PreparedJob prepare(Job::IJob& job, void* args) noexcept;

			// Unsupported
			PreparedJob(PreparedJob& other) = delete;
			void operator=(PreparedJob& other) = delete;
		protected:
			/// <summary>
			/// Inidcated that this instance is valid
			/// </summary>
			bool m_bValid = false;

			/// <summary>
			/// Job pointer
			/// </summary>
			IJob* m_ptrJob = nullptr;

			/// <summary>
			/// Job preparation description
			/// </summary>
			JobPrepareDesc m_prepareDesc;
	};

	/// <summary>
	/// Job prepare function
	/// </summary>
	/// <param name="job">Input job to be staged</param>
	/// <param name="args">Job arguments</param>
	/// <returns>JobPreparation info</returns>
	inline static PreparedJob prepare(Job::IJob& job, void* args = nullptr) noexcept {
		return PreparedJob::prepare(job, args);
	}
}