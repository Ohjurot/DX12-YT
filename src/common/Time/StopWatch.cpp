#include "DefHeader.h"
#include "StopWatch.h"

Time::StopWatch::StopWatch() noexcept {
	// Get freqency
	QueryPerformanceFrequency(&m_timerFreqency);

	// Zero times
	m_timerCurrentStartTime.QuadPart = 0;
	m_timerElapsed.QuadPart = 0;
}

void Time::StopWatch::start() noexcept {
	if (!m_bRunning) {
		// Start the timer
		QueryPerformanceCounter(&m_timerCurrentStartTime);
		m_bRunning = true;
	}
}

void Time::StopWatch::pause() noexcept {
	if (m_bRunning) {
		// Get stop time
		LARGE_INTEGER stopTime;
		QueryPerformanceCounter(&stopTime);

		// Add to elapsed
		m_timerElapsed.QuadPart += (stopTime.QuadPart - m_timerCurrentStartTime.QuadPart);

		// Pause
		m_bRunning = false;
	}
}

void Time::StopWatch::reset() noexcept {
	// Reset timer
	m_timerElapsed.QuadPart = 0;
	QueryPerformanceCounter(&m_timerCurrentStartTime);
}

UINT64 Time::StopWatch::deltaMs() noexcept {
	// Build time
	LARGE_INTEGER deltaTime = m_timerElapsed;
	if (m_bRunning) {
		// Add stop timer
		LARGE_INTEGER stopTime;
		QueryPerformanceCounter(&stopTime);
		deltaTime.QuadPart += (stopTime.QuadPart - m_timerCurrentStartTime.QuadPart);
	}

	// Convert to ms
	deltaTime.QuadPart *= 1000;
	deltaTime.QuadPart /= m_timerFreqency.QuadPart;
	return deltaTime.QuadPart;
}

UINT64 Time::StopWatch::deltaUs() noexcept {
	// Build time
	LARGE_INTEGER deltaTime = m_timerElapsed;
	if (m_bRunning) {
		// Add stop timer
		LARGE_INTEGER stopTime;
		QueryPerformanceCounter(&stopTime);
		deltaTime.QuadPart += (stopTime.QuadPart - m_timerCurrentStartTime.QuadPart);
	}

	// Convert to us
	deltaTime.QuadPart *= 1000000;
	deltaTime.QuadPart /= m_timerFreqency.QuadPart;
	return deltaTime.QuadPart;
}
