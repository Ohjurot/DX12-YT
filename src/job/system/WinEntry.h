#pragma once

#include <DefHeader.h>
#include <job/system/IJob.h>
#include <job/system/JobSystem.h>
#include <job/system/SystemInstance.h>
#include <job/system/PreparedJob.h>
#include <job/system/EntryJob.h>
#include <common/Exceptions/EXPP_Handler.h>

#define MAIN_JOB_CLASS(className) class className : public Job::__winMainJob
#define MAIN_JOB_INVOKE(className) int WINAPI wWinMain(HINSTANCE _In_ hInstance, HINSTANCE _In_opt_ hPrevInstance, PWSTR _In_ pCmdLine, int _In_ nCmdShow){return Job::Entry::__winMain<className>(hInstance, hPrevInstance, pCmdLine, nCmdShow);}
#define MAIN_JOB(className) class className; MAIN_JOB_INVOKE(className); MAIN_JOB_CLASS(className)

namespace Job {
	/// <summary>
	/// Class for encapsulating the entry point
	/// </summary>
	class Entry {
		public:
			/// <summary>
			/// NOT TO BE CALLED BY THE PROGRAMMER!!!!! Window entry point call function SAFE
			/// </summary>
			/// <typeparam name="JT">Type of the job (from Job::__winMainJob)</typeparam>
			/// <param name="hInstance">HINSTANCE</param>
			/// <param name="hPrevInstance">unused</param>
			/// <param name="cmdArgs">Command line args</param>
			/// <param name="nCmdShow">Cmd show int</param>
			/// <returns>Application return code</returns>
			template<typename JT>
			static int __winMain_s(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR cmdArgs, INT nCmdShow) {
				// Create static job and pointer to macro asserted one
				JT job = JT();
				__winMainJob* ptrMainJob = (__winMainJob*)&job;

				// Set windows data
				ptrMainJob->m_hInstance = hInstance;
				ptrMainJob->m_cmdArgs = cmdArgs;
				ptrMainJob->m_nCmdShow = nCmdShow;

				// Instance job system
				EXPP_ASSERT(SystemInstance::init(), "Job::SystemInstance::init(...) failed");

				// Dispatch job
				PreparedJob preparedJob = Job::prepare(*ptrMainJob);
				if (preparedJob) {
					EXPP_ASSERT(preparedJob.stageExecution(), "Inital job staging failed!");
				}

				// Wait until no jobs exists execution
				SystemInstance::getSystem()->wait();
				SystemInstance::shutdown();

				// Return zero
				return 0;
			}

			/// <summary>
			/// NOT TO BE CALLED BY THE PROGRAMMER!!!!! Window entry point call function
			/// </summary>
			/// <typeparam name="JT">Type of the job (from Job::__winMainJob)</typeparam>
			/// <param name="hInstance">HINSTANCE</param>
			/// <param name="hPrevInstance">unused</param>
			/// <param name="cmdArgs">Command line args</param>
			/// <param name="nCmdShow">Cmd show int</param>
			/// <returns>Application return code</returns>
			template<typename JT>
			static int __winMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR cmdArgs, INT nCmdShow) {
				// Safe invoke
				EXPP::InvocationResult<int> winmainCall = EXPP::invoke<int>(__winMain_s<JT>, hInstance, hPrevInstance, cmdArgs, nCmdShow);

				// Handle failed jon
				if (winmainCall.failed()) {
					common::EXPPHandler::reportExeption(winmainCall);
					return -1;
				}

				return winmainCall.returnValue();
			}
	};
}
