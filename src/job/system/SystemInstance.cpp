#include "DefHeader.h"
#include "SystemInstance.h"

// Instance
Job::JobSystem* Job::SystemInstance::s_ptrJobInstance = nullptr;

bool Job::SystemInstance::init(int threadCount) noexcept {
	if (!s_ptrJobInstance) {
		s_ptrJobInstance = new JobSystem(threadCount);
		return true;
	}

	// Fallback false
	return false;
}

void Job::SystemInstance::shutdown() noexcept {
	if (s_ptrJobInstance) {
		delete s_ptrJobInstance;
		s_ptrJobInstance = nullptr;
	}
}

Job::JobSystem* Job::SystemInstance::getSystem() noexcept {
	return s_ptrJobInstance;
}

void Job::SystemInstance::pause() noexcept {
	// Pause on system
	if(s_ptrJobInstance){
		s_ptrJobInstance->jobPauseFunction();
	}
	// Make space for next thread
	else {
		SwitchToThread();
	}
}
