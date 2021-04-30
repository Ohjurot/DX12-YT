#pragma once

#include <DefHeader.h>

namespace Time {
	/// <summary>
	/// Stopwatch
	/// </summary>
	class StopWatch {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <returns></returns>
			StopWatch() noexcept;

			/// <summary>
			/// Start the timer
			/// </summary>
			void start() noexcept;

			/// <summary>
			/// Pause the timer
			/// </summary>
			void pause() noexcept;

			/// <summary>
			/// Reset the timer
			/// </summary>
			void reset() noexcept;

			/// <summary>
			/// Get delta time in ms
			/// </summary>
			/// <returns></returns>
			UINT64 deltaMs() noexcept;

			/// <summary>
			/// Get delta time in us
			/// </summary>
			/// <returns></returns>
			UINT64 deltaUs() noexcept;

		private:
			/// <summary>
			/// Timer freqency
			/// </summary>
			LARGE_INTEGER m_timerFreqency;

			/// <summary>
			/// Current timer start time
			/// </summary>
			LARGE_INTEGER m_timerCurrentStartTime;

			/// <summary>
			/// Current timer elapsed time
			/// </summary>
			LARGE_INTEGER m_timerElapsed;

			/// <summary>
			/// Current state of time
			/// </summary>
			bool m_bRunning = false;
	};
}