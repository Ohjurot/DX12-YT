#pragma once
#include <DefHeader.h>

#include <job/threading/Thread.h>

namespace Threading {	
	/// <summary>
	/// Struct passed as useerdata to MTD Threads
	/// </summary>
	struct MTD_ThreadArgs {
		/// <summary>
		/// Number of current running thread
		/// </summary>
		unsigned int threadIndex;

		/// <summary>
		/// Userdata passed to function
		/// </summary>
		void* ptrUserData;
	};

	/// <summary>
	/// Multithread dispatcher
	/// </summary>
	class MultiThreadDispatcher : public Threading::IThread {
		public:
			/// <summary>
			/// Dispatch all threads
			/// </summary>
			/// <param name="numThreads">Number of threads</param>
			/// <param name="ptrInterface">Interface to call</param>
			/// <param name="userdata">Option userdata</param>
			MultiThreadDispatcher(unsigned int numThreads, Threading::IThread* ptrInterface, void* userdata = nullptr);

			/// <summary>
			/// Destruct
			/// </summary>
			~MultiThreadDispatcher();

			/// <summary>
			/// Join on all threads
			/// </summary>
			/// <param name="joinTimePerThread">Time to join per thread</param>
			void joinAll(DWORD joinTimePerThread);

			/// <summary>
			/// Kill all threads
			/// </summary>
			void killAll();

			/// <summary>
			/// Wait for all threads or kill them
			/// </summary>
			/// <param name="joinTimePerThread">Wait time per thread bevore kill</param>
			void terminateKillWait(DWORD joinTimePerThread);

			/// <summary>
			/// Keepup all threads
			/// </summary>
			/// <returns>false on exit</returns>
			bool keepUpThreads();

		protected:
			/// <summary>
			/// Thread execution function (to be overridden)
			/// </summary>
			/// <param name="parameter">Input user parameter</param>
			/// <returns>Thread return code</returns>
			DWORD threadExecute(void* parameter) override;

			/// <summary>
			/// Thread exeception handling function
			/// </summary>
			/// <param name="refResult">Reference to result</param>
			void threadHandleExeception(EXPP::InvocationResult<DWORD>& refResult) noexcept override;

		private:
			/// <summary>
			/// Number of used threads
			/// </summary>
			const unsigned int m_cNumThreads;

			/// <summary>
			/// Current exception state
			/// </summary>
			std::atomic_flag m_exceptionFlag;

			/// <summary>
			/// Stored invocation result
			/// </summary>
			EXPP::InvocationResult<DWORD> m_storedResult;

			/// <summary>
			/// Thread array
			/// </summary>
			Thread* m_arrThreads;

			/// <summary>
			/// Array for threads data
			/// </summary>
			MTD_ThreadArgs* m_arrThreadsData;

			/// <summary>
			/// Impelementation of one thread
			/// </summary>
			Threading::IThread* m_ptrInterface;
	};
}