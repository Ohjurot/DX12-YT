#pragma once

#include <DefHeader.h>

#include <job/system/IJob.h>

namespace Job {
	/// <summary>
	/// Wait object for a job
	/// </summary>
	class WaitObject {
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			WaitObject() = default;

			/// <summary>
			/// Creare wait object for job
			/// </summary>
			/// <param name="ptrJob">Input job</param>
			WaitObject(IJob* ptrJob) noexcept;

			/// <summary>
			/// Return the state of the wait object
			/// </summary>
			/// <returns>true if done</returns>
			bool isDone() noexcept;

			/// <summary>
			/// Waits until is done
			/// </summary>
			void wait() noexcept;

			/// <summary>
			/// Bool conversion operator
			/// </summary>
			/// <returns>true if wait object is valid</returns>
			operator bool() noexcept;
		private:
			/// <summary>
			/// Job pointer
			/// </summary>
			IJob* m_ptrJob = nullptr;

			/// <summary>
			/// Requested value
			/// </summary>
			UINT64 m_value = 0;
	};
}
