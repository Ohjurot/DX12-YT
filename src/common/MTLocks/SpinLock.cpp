#include "DefHeader.h"
#include "SpinLock.h"

void MT::SpinLock::aquire() noexcept {
	// While aquesition fails wait
	while (!tryAquire()) {
		THREAD_PAUSE_FUNCTION();
	}
}

bool MT::SpinLock::tryAquire() noexcept {
	return !m_flag.test_and_set(std::memory_order_acquire);
}

void MT::SpinLock::release() noexcept {
	m_flag.clear(std::memory_order_release);
}
