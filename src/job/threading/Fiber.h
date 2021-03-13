#pragma once

#include <DefHeader.h>
#include <job/threading/LocalStorageProvider.h>
#include <job/threading/ThreadingExecution.h>

class Fiber;

namespace Threading {
	/// <summary>
	/// Interface for a fiber
	/// </summary>
	class IFiber {
		protected:
			friend class ::Fiber;

			/// <summary>
			/// Fiber execution function (to be overridden)
			/// </summary>
			/// <param name="parameter">Input user parameter</param>
			/// <returns>Thread return code</returns>
			virtual void fiberExecute(void* parameter) = 0;

			/// <summary>
			/// Fiber exeception handling function
			/// </summary>
			/// <param name="refResult">Reference to result</param>
			virtual void fiberHandleExeception(EXPP::InvocationResult<DWORD>& refResult) noexcept {};
	};	
}

/// <summary>
/// Fiber
/// </summary>
class Fiber {
	public:
		/// <summary>
		/// Default construct
		/// </summary>
		Fiber() = default;

		/// <summary>
		/// Init and create a fiber
		/// </summary>
		/// <param name="ptrImplementation">Fiber implementation</param>
		/// <param name="ptrUserData">Fiber userdata</param>
		Fiber(Threading::IFiber* ptrImplementation, void* ptrUserData = nullptr);

		/// <summary>
		/// Destructor
		/// </summary>
		~Fiber();

		/// <summary>
		/// Init and create a fiber
		/// </summary>
		/// <param name="ptrImplementation">Fiber implementation</param>
		/// <param name="ptrUserData">Fiber userdata</param>
		void init(Threading::IFiber* ptrImplementation, void* ptrUserData = nullptr);

		/// <summary>
		/// Release fiber
		/// </summary>
		void release() noexcept;

		/// <summary>
		/// Retrive the fiber address
		/// </summary>
		/// <returns>Address of the fiber</returns>
		LPVOID getAddress() noexcept;

		/// <summary>
		/// LPVOID conversion operator
		/// </summary>
		operator LPVOID();

		/// <summary>
		/// Get static address of current fiber
		/// </summary>
		/// <returns>Fiber address</returns>
		static LPVOID getStaticFiberAddress() noexcept;

		/// <summary>
		/// Convert fiber to thread
		/// </summary>
		static void convertToThread();

	private:
		/// <summary>
		/// Safe fiber entry point
		/// </summary>
		/// <param name="ptrFiber">Pointer to fiber</param>
		static int fiberEntryPoint_s(Fiber* ptrFiber);

		/// <summary>
		/// Fiber entry point
		/// </summary>
		/// <param name="lpParameter">Fiber parameter</param>
		/// <returns></returns>
		static void WINAPI _winapi_fiberEntryPoint(_In_ LPVOID lpParameter);

	private:
		/// <summary>
		/// Execution state
		/// </summary>
		Threading::ExecutionState m_state = Threading::ExecutionState::INVALID;

		/// <summary>
		/// Indicates the termination of the flag
		/// </summary>
		std::atomic_flag m_terminationFlag = ATOMIC_FLAG_INIT;
		
		/// <summary>
		/// Adress of the fiber
		/// </summary>
		LPVOID m_fiberAdress = nullptr;

		/// <summary>
		/// Fiber implementation
		/// </summary>
		Threading::IFiber* m_ptrImplementation = nullptr;

		/// <summary>
		/// Invocation result
		/// </summary>
		EXPP::InvocationResult<int> m_result;

		/// <summary>
		/// Fiber userdata
		/// </summary>
		void* m_ptrUserData = nullptr;
};