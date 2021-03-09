#pragma once

#include <DefHeader.h>

namespace Threading {
	/// <summary>
	/// Result of a invocation
	/// </summary>
	enum class ExecutionState {
		/// <summary>
		/// Invalid
		/// </summary>
		INVALID,

		/// <summary>
		/// Result pending (Executing)
		/// </summary>
		PENDING,

		/// <summary>
		/// Thread succeeded
		/// </summary>
		SUCCEED,

		/// <summary>
		/// Thread failed
		/// </summary>
		FAILED,

		/// <summary>
		/// The thread has been terminated programatly
		/// </summary>
		FORCE_TERMINATED,
	};
}