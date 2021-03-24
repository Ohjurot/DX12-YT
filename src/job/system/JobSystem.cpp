#include "DefHeader.h"
#include "JobSystem.h"

Job::JobSystem::JobSystem(int threadCount) {
    // Check if auto detect is required
    if (threadCount < 1) {
        // Get system info
        SYSTEM_INFO sysInfo;
        ZeroMemory(&sysInfo, sizeof(SYSTEM_INFO));
        GetSystemInfo(&sysInfo);

        // Read processor info
        threadCount = sysInfo.dwNumberOfProcessors;
    }

    // Allocate FLS
    m_flsJobCookie = FlsAlloc(NULL);
    EXPP_ASSERT(m_flsJobCookie, "FlsAlloc(...)");

    // Create fibers
    for (int i = 0; i < JOB_NUM_FIBERS - threadCount; i++) {
        JobFiber* ptrFiber = m_fiberPool.notifyFiberCreation();
        m_arrFibers[i].init(this, ptrFiber);
        ptrFiber->m_address = m_arrFibers[i].getAddress();
    }

    // Dispatch threads
    m_ptrMtd = new Threading::MultiThreadDispatcher(threadCount, this, nullptr);
}

Job::JobSystem::~JobSystem() {
    /// <summary>
    /// Delete MTD
    /// </summary>
    delete m_ptrMtd;

    // FLS Free
    FlsFree(m_flsJobCookie);
}

bool Job::JobSystem::stageExecution(JobQueueDesc& description) noexcept {
    // If job store succed increment count and return
    if (m_jobQueue.queueJob(description)) {
        m_jobCount++;
        return true;
    }

    // Return false
    return false;
}

void Job::JobSystem::exit() noexcept {
    m_bKeepRunning = false;
}

void Job::JobSystem::wait() {
    // Infinite wait loop
    while (true) {
        // Sleep
        Sleep(500);

        // Normal execution
        if (m_bKeepRunning && m_jobCount.load(std::memory_order_acquire) > 0) {
            // Keepup threads
            if (!m_ptrMtd->keepUpThreads()) {
                m_bKeepRunning = false;
                m_ptrMtd->terminateKillWait(1000);
                m_ptrMtd->raisExeception();
                return;
            }
        }
        // Normal exection on more jobs
        else if (m_bKeepRunning) {
            m_bKeepRunning = false;
            m_ptrMtd->joinAll(1500);
            return;
        }
        // Force exit
        else {
            // Join kill all
            m_ptrMtd->terminateKillWait(1500);
            return;
        }        
    }
}

void Job::JobSystem::jobPauseFunction() {
    // Get fiber and switch
    JobFiber* ptrFiber = (JobFiber*)FlsGetValue(m_flsJobCookie);
    if (ptrFiber) {
        // Get next fiber
        Job::JobFiber* ptrNextFiber;
        Job::JobQueueDesc nextJobDesc;
        if (m_fiberPool.takeFiber(&ptrNextFiber)) {
            // Check fiber
            if (ptrNextFiber->queuDesc.ptrJob && !ptrNextFiber->queuDesc.ptrJob->hasDependencys()) {
                // Store own and switch
                EXPP_ASSERT(m_fiberPool.returnFiber(ptrFiber), "Return of current fiber impossible");
                SwitchToFiber(ptrNextFiber->m_address);
                return;
            }
            else {
                // Return next fiber
                EXPP_ASSERT(m_fiberPool.returnFiber(ptrNextFiber), "Return of previously fetched fiber impossible");
            }
        }
        // Get next job
        else if (m_jobQueue.getNextJob(&nextJobDesc)) {
            // Load job into fiber
            if (!m_fiberPool.enableFiber(nextJobDesc)) {
                m_jobQueue.queueJob(nextJobDesc);
            }
        }
    }
    // Make space for other thread
    SwitchToThread();
}

DWORD Job::JobSystem::threadExecute(void* parameter) {
    // CoInit
    EVALUATE_HRESULT(CoInitialize(NULL), "CoInitialize(...)");

    // MTD Args
    Threading::MTD_ThreadArgs* ptrArgs = (Threading::MTD_ThreadArgs*)parameter;

    // Build thread name
    std::wstringstream wss;
    wss << L"Worker Thread #" << ptrArgs->threadIndex;

    // Name thread
    SetThreadDescription((HANDLE)Thread::get(), wss.str().c_str());
    
    // Convert to fiber
    Thread::convertToFiber();

    // Send fiber to pool
    JobFiber* ptrJobFiber = m_fiberPool.notifyFiberCreation();
    EXPP_ASSERT(ptrJobFiber, "Invalid fiber value");
    ptrJobFiber->m_address = Thread::get().getFiberAddress();
    FlsSetValue(m_flsJobCookie, ptrJobFiber);

    // Call jobsystemmain
    jobSystemExecute(ptrJobFiber);

    // CoUninit
    CoUninitialize();

    // Return zero
    return 0;
}

void Job::JobSystem::fiberExecute(void* parameter) {
    // Store fiber in fls
    FlsSetValue(m_flsJobCookie, parameter);

    // Directly job execute
    jobSystemExecute((JobFiber*)parameter);
}

void Job::JobSystem::jobSystemExecute(JobFiber* ptrFiber) {
    // Idle timer
    Time::StopWatch timer;
    timer.start();

    // Job loop
    while (m_bKeepRunning) {
        jobWhileStart:

        // Check if a job exists
        if (ptrFiber->queuDesc.ptrJob) {
            // Execute job
            jobExecute:
            switch (ptrFiber->queuDesc.ptrJob->execute(ptrFiber->queuDesc.dispatchIndex)) {
                case JobReturnValue::JOB_DONE:
                    ptrFiber->queuDesc.ptrJob->p_finishIndex++;
                    break;
                case JobReturnValue::JOB_RESTAGE:
                    goto jobExecute;
                    break;
            }

            // remove job from fiber
            ptrFiber->queuDesc.ptrJob = nullptr;
            m_jobCount--;

            // Reset timer
            timer.reset();
        }
        // Check how and if to delay execution
        else {
            // Make space for other fiber
            Job::JobFiber* ptrNextFiber;
            Job::JobQueueDesc nextDesc;
            if (m_fiberPool.takeFiber(&ptrNextFiber)) {
                // Check fiber
                if (ptrNextFiber->queuDesc.ptrJob && !ptrNextFiber->queuDesc.ptrJob->hasDependencys()) {
                    // Return this fiber and switch
                    EXPP_ASSERT(m_fiberPool.returnFiber(ptrFiber), "Return of current fiber impossible");
                    SwitchToFiber(ptrNextFiber->m_address);
                    SwitchToThread();
                    goto jobWhileStart;
                }
                // Return next fiber
                EXPP_ASSERT(m_fiberPool.returnFiber(ptrNextFiber), "Return of previously fetched fiber impossible");
            }
            else if (m_jobQueue.getNextJob(&nextDesc)) {
                if (nextDesc.ptrJob && !nextDesc.ptrJob->hasDependencys()) {
                    // Set and go on
                    ptrFiber->queuDesc = nextDesc;
                    SwitchToThread();
                    goto jobWhileStart;
                } else {
                    // Queue job agian
                    EXPP_ASSERT(m_jobQueue.queueJob(nextDesc), "Queueing of former fetched job not possible");
                }
            }
            // Check if "long" sleep is allowed
            else if (timer.deltaMs() >= 1500) {
                Sleep(500);
                continue;
            }
        }

        // Make space for windows
        SwitchToThread();
    }
}
