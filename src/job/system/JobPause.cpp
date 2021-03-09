#include "DefHeader.h"

#include <job/system/SystemInstance.h>

void JobPause::__job_pause() {
	Job::SystemInstance::pause();
}
