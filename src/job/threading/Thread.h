#pragma once

#include <DefHeader.h>

#include <job/threading/LocalStorageProvider.h>
#include <job/threading/ThreadingExecution.h>

#define MAX_THREAD_TIMEOUT_TIME_MS 5000

// Predeclare
class Thread;

namespace Threading {
	class MultiThreadDispatcher;
	
	/// <summary>
	/// Interface for a thread
	/// </summary>
	class IThread {
		protected:
			friend class ::Thread;
			friend class ::Threading::MultiThreadDispatcher;

			/// <summary>
			/// Thread execution function (to be overridden)
			/// </summary>
			/// <param name="parameter">Input user parameter</param>
			/// <returns>Thread return code</returns>
			virtual DWORD threadExecute(void* parameter) = 0;

			/// <summary>
			/// Thread exeception handling function
			/// </summary>
			/// <param name="refResult">Reference to result</param>
			virtual void threadHandleExeception(EXPP::InvocationResult<DWORD>& refResult) noexcept {};
	};
}

/// <summary>
/// Async thread
/// </summary>
class Thread {
	public:
		/// <summary>
		/// Default construct
		/// </summary>
		Thread() = default;

		/// <summary>
		/// Create a dispatched thread
		/// </summary>
		/// <param name="ptrImplementation">Pointer to threads implementation</param>
		/// <param name="ptrUserData">Pointer to userdata passed to implementation</param>
		/// <param name="targetLogicalCore">Target logical core -1 to use all cores</param>
		Thread(Threading::IThread* ptrImplementation, void* ptrUserData = nullptr, int targetLogicalCore = -1);

		/// <summary>
		/// Destruct
		/// </summary>
		~Thread();

		/// <summary>
		/// Thread init function
		/// </summary>
		/// <param name="ptrImplementation">Pointer to threads implementation</param>
		/// <param name="ptrUserData">Pointer to userdata passed to implementation</param>
		/// <param name="targetLogicalCore">Target logical core -1 to use all cores</param>
		void init(Threading::IThread* ptrImplementation, void* ptrUserData = nullptr, int targetLogicalCore = -1);

		/// <summary>
		/// Join a thread
		/// </summary>
		/// <param name="maxWaitTime">Maximum waiting time</param>
		/// <returns>True if thread joined</returns>
		bool join(DWORD maxWaitTime) noexcept;

		/// <summary>
		/// Kill the thread
		/// </summary>
		/// <returns>Kills the thread</returns>
		void kill() noexcept;

		/// <summary>
		/// Retrive the threads execution result
		/// </summary>
		/// <returns>Execution result</returns>
		Threading::ExecutionState getResult() noexcept;

		/// <summary>
		/// Checks if the thread is running
		/// </summary>
		/// <returns>True if running</returns>
		bool isRunning() noexcept;

		/// <summary>
		/// Rais the threads exeception in the current context (if has any)
		/// </summary>
		void raisThreadException();

		/// <summary>
		/// Retrive the threads ID
		/// </summary>
		/// <returns>Thread ID</returns>
		DWORD getThreadId() noexcept;

		/// <summary>
		/// Retrive the fiber address of the thread
		/// </summary>
		/// <returns>Fiber address or nullptr</returns>
		LPVOID getFiberAddress() noexcept;

		/// <summary>
		/// Convert to handle operator
		/// </summary>
		/// <returns>Thread handle</returns>
		explicit operator HANDLE() noexcept;

		/// <summary>
		/// Get current thread
		/// </summary>
		/// <returns>Reference to current thread</returns>
		static Thread& get() noexcept;

		/// <summary>
		/// Convert current thread to fiber
		/// </summary>
		static void convertToFiber();

		// Delete unsupported
		Thread(const Thread&) = delete;
		void operator=(const Thread&) = delete;
	private:
		/// <summary>
		/// Thread entry point
		/// </summary>
		/// <returns>Thread return code</returns>
		DWORD threadEntryPoint();

		/// <summary>
		/// Thread entry point
		/// </summary>
		/// <returns>Thread return code</returns>
		static DWORD threadEntryPoint_safe(Thread* ptrThread);

		/// <summary>
		/// Static thread proc
		/// </summary>
		/// <param name="lpParameter">Pointer to Thread</param>
		/// <returns>Thread return code</returns>
		static DWORD WINAPI _winapi_threadEntryPoint(_In_ LPVOID lpParameter);

	private:
		/// <summary>
		/// Thread handle
		/// </summary>
		HANDLE m_hThread = INVALID_HANDLE_VALUE;

		/// <summary>
		/// ID of the thread
		/// </summary>
		DWORD m_dwThreadId = 0;

		/// <summary>
		/// Result of Thread invocation
		/// </summary>
		Threading::ExecutionState m_executionResult = Threading::ExecutionState::INVALID;

		/// <summary>
		/// Thread result
		/// </summary>
		EXPP::InvocationResult<DWORD> m_result;

		/// <summary>
		/// User thread data
		/// </summary>
		void* m_ptrUserThreadData = nullptr;

		/// <summary>
		/// User thread implementation
		/// </summary>
		Threading::IThread* m_ptrImplementation = nullptr;

		/// <summary>
		/// Related fiber address (if any
		/// </summary>
		LPVOID m_relatedFiberAddress = nullptr;

		/// <summary>
		/// Thread creation flag
		/// </summary>
		std::atomic_flag m_creationFlag = ATOMIC_FLAG_INIT;
};