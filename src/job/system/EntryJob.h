#pragma once

#include <DefHeader.h>
#include <job/system/IJob.h>

namespace Job {
	// Predecl
	class Entry;

	/// <summary>
	/// WinMain job
	/// </summary>
	JOB_CLASS(__winMainJob) {
		/// <summary>
		/// Job prepare function
		/// </summary>
		/// <param name="args">Input arguments (unused here)</param>
		JOB_PREPARE_FUNCTION(void* args);

		// Virtual
		protected: virtual Job::JobReturnValue execute(unsigned int index) = 0;

		protected:
			friend class Entry;

			/// <summary>
			/// HINSTANCE of the application
			/// </summary>
			HINSTANCE m_hInstance = NULL;

			/// <summary>
			/// Command args
			/// </summary>
			PWSTR m_cmdArgs = NULL;

			/// <summary>
			/// CMD Show hint
			/// </summary>
			INT m_nCmdShow = NULL;
	};
}