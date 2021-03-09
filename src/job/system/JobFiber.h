#pragma once

#include <DefHeader.h>

#include <job/system/IJob.h>
#include <job/system/JobData.h>

namespace Job {
	/// <summary>
	/// Fiber for a job
	/// </summary>
	struct JobFiber {
		/// <summary>
		/// Fiber address
		/// </summary>
		LPVOID m_address;

		/// <summary>
		/// Currents jobs description
		/// </summary>
		JobQueueDesc queuDesc;
	};

}