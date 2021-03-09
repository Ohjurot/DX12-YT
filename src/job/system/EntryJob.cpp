#include "DefHeader.h"
#include "EntryJob.h"

Job::JobPrepareDesc Job::__winMainJob::prepare(void* args) {
	// Create default meta and change priority (Make shure main job is executed on highst priority)
	auto meta = DEFAULT_JOB_PREPARE_DESC;
	meta.priority = JobPriority::CRITICAL;

	// Return meta
	return meta;
}