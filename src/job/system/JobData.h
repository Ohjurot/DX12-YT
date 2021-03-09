#pragma once

#include <DefHeader.h>

#define DEFAULT_JOB_PREPARE_DESC Job::JobPrepareDesc();
#define JOB_PRIORITY_COUNT 5

#define JOB_RETURN_DONE Job::JobReturnValue::JOB_DONE
#define JOB_RETURN_RESTAGE Job::JobReturnValue::JOB_RESTAGE

namespace Job {
	// Predecl
	class IJob;

	/// <summary>
	/// Priority of the job
	/// </summary>
	enum class JobPriority : int {
		/// <summary>
		/// Low priority jon
		/// </summary>
		LOW = -10,

		/// <summary>
		/// Normal priority job
		/// </summary>
		NORMAL = 0,

		/// <summary>
		/// Medium priority job
		/// </summary>
		MEDIUM = 10,

		/// <summary>
		/// High priority job
		/// </summary>
		HIGH = 20,

		/// <summary>
		/// Critical job
		/// </summary>
		CRITICAL = 100,
	};

	/// <summary>
	/// Return value of the job
	/// </summary>
	enum class JobReturnValue {
		/// <summary>
		/// Job finished execution
		/// </summary>
		JOB_DONE,

		/// <summary>
		/// Job requires restaging
		/// </summary>
		JOB_RESTAGE,
	};

	/// <summary>
	/// Desciption for preparing a job
	/// </summary>
	struct JobPrepareDesc {
		/// <summary>
		/// Priority of the job
		/// </summary>
		JobPriority priority = JobPriority::NORMAL;

		/// <summary>
		/// How many times the job needs to be invoked (may be paralell or serial)
		/// </summary>
		unsigned int numThreads = 1;
	};

	/// <summary>
	/// Description for the job queue
	/// </summary>
	struct JobQueueDesc {
		/// <summary>
		/// Pointer to job
		/// </summary>
		IJob* ptrJob = nullptr;

		/// <summary>
		/// 
		/// </summary>
		JobPriority jobPriority = JobPriority::NORMAL;

		/// <summary>
		/// Job dispatch index
		/// </summary>
		unsigned int dispatchIndex = 0;
	};
}