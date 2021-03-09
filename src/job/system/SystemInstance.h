#pragma once

#include <DefHeader.h>
#include <job/system/JobSystem.h>


namespace Job {
	class SystemInstance {
		public:
			/// <summary>
			/// Job system init function
			/// </summary>
			/// <param name="threadCount">Number of threads to be dispatched</param>
			/// <return>true on succeed</return>
			static bool init(int threadCount = 0) noexcept;

			/// <summary>
			/// Job system shutdown function
			/// </summary>
			static void shutdown() noexcept;

			/// <summary>
			/// Get the job system pointer
			/// </summary>
			/// <returns>JobSystem pointer</returns>
			static Job::JobSystem* getSystem() noexcept;

			/// <summary>
			/// Job pause function
			/// </summary>
			static void pause() noexcept;
		private:
			/// <summary>
			/// Job system pointer static instance
			/// </summary>
			static JobSystem* s_ptrJobInstance;
	};
}
