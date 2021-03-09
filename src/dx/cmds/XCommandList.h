#pragma once
#include <DefHeader.h>

#include <dx/cmds/XCommandQueue.h>
#include <dx/cmds/FenceCounter.h>
#include <dx/cmds/CommandQueueManager.h>

#include <common/MT/Locks/SpinLock.h>

#define CLS_DX_XCommandList__MAX_DEPENDENCYS 128

namespace DX {
	// Predeclare
	class XCommandList;

	/// <summary>
	/// Current state of the command list
	/// </summary>
	enum class XCommandList_State {
		/// <summary>
		/// The command list is invalid
		/// </summary>
		INVALID,
		
		/// <summary>
		/// Command list can recive commands 
		/// </summary>
		OPEN,

		/// <summary>
		/// Command list is ready to execute
		/// </summary>
		CLOSED,

		/// <summary>
		/// Command list is queue or executing
		/// </summary>
		QUEUE_EXECUTING,
	};

	/// <summary>
	/// Dependants list
	/// </summary>
	template<unsigned int N>
	struct XCommandList_DependantsList {
		/// <summary>
		/// Count of dependants
		/// </summary>
		UINT64 dependantsCount = 0;

		/// <summary>
		/// Dependants count list
		/// </summary>
		XCommandList* arrPtrDependants[N] = {};

		/// <summary>
		/// Size of list
		/// </summary>
		/// <returns>Returns N</returns>
		static constexpr unsigned int size() { return N; }
	};

	/// <summary>
	/// Command list
	/// </summary>
	class XCommandList : public ScopedComPointer<ID3D12GraphicsCommandList> {
		// Predeclare
		public:
			class WaitObject;

		// Class
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			XCommandList() = default;

			/// <summary>
			/// Destruct
			/// </summary>
			~XCommandList();

			/// <summary>
			/// Init function
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refCommandQueue">Reference to command queue</param>
			/// <param name="refFenceCounter">Reference to counter responsible for que executions</param>
			void init(ID3D12Device* ptrDevice, XCommandQueue& refCommandQueue, FenceCounter& refFenceCounter);

			/// <summary>
			/// Release function
			/// </summary>
			void release() override;

			/// <summary>
			/// Retrive the state of the command list
			/// </summary>
			/// <returns></returns>
			DX::XCommandList_State getState();

			/// <summary>
			/// Refresh command list
			/// </summary>
			void refresh();

			/// <summary>
			/// Closes the command list
			/// </summary>
			DX::XCommandList::WaitObject close();

			/// <summary>
			/// Execute command list
			/// </summary>
			/// <param name="ptrArrFence">Array of execution fences</param>
			/// <returns></returns>
			DX::XFence::WaitObject execute(XFence* ptrArrFence);

			/// <summary>
			/// Creates a wait object for the command lists last exection
			/// </summary>
			/// <returns></returns>
			DX::XFence::WaitObject getExecutionWaitObject();

			/// <summary>
			/// Gets a universal wait object independent of execution
			/// </summary>
			/// <returns>Wait object</returns>
			DX::XCommandList::WaitObject getUniversalWaitObject();

			/// <summary>
			/// Get current itteration index
			/// </summary>
			/// <returns>Itteration</returns>
			UINT64 getCurrentItteration() noexcept;

			/// <summary>
			/// Checks if the list has dependencys
			/// </summary>
			/// <returns>True if it has dependencys</returns>
			bool hasDependencys() noexcept;

			/// <summary>
			/// Inject a dependency in the command list
			/// </summary>
			/// <param name="dependant">New dependancy</param>
			void injectDependency(WaitObject& dependancy);

			// Delete unsupported
			XCommandList(const XCommandList&) = delete;
			void operator=(const XCommandList&) = delete;
		
		private:
			/// <summary>
			/// Resolve dependencys on that list (recursivly resolves dependants)
			/// </summary>
			/// <param name="arrFences">Feces</param>
			/// <param name="fencePage">Resolved fence page</param>
			/// <param name="fenceValue">Resolved fence value</param>
			void resolveDependency(XFence* arrFences, unsigned int fencePage, UINT64 fenceValue);

			/// <summary>
			/// Set internal strong dependency
			/// </summary>
			/// <param name="fencePage">Fence page</param>
			/// <param name="fenceValue">Value of fence</param>
			void setResolvedDependency(unsigned int fencePage, UINT64 fenceValue);

			/// <summary>
			/// Insert a dependant into this list
			/// </summary>
			/// <param name="waitObject">Target wait object (ref)</param>
			/// <param name="ptrCmdList">Pointer to target cmd list</param>
			/// <returns>If operation completed without an error</returns>
			bool insertDependant(WaitObject& waitObject, XCommandList* ptrCmdList);

		private:
			/// <summary>
			/// Initial invalid state
			/// </summary>
			XCommandList_State m_state = XCommandList_State::INVALID;

			/// <summary>
			/// Fence fronted
			/// </summary>
			FenceCounter::Frontend m_fenceFrontend;

			/// <summary>
			/// Pointer to related command queue
			/// </summary>
			XCommandQueue* m_ptrCommandQueue = nullptr;

			/// <summary>
			/// Assoiated command allocator
			/// </summary>
			ScopedComPointer<ID3D12CommandAllocator> m_allocator;

			/// <summary>
			/// Revision of the command list
			/// </summary>
			UINT64 m_uiIteration = 1;

			/// <summary>
			/// Count of unresolved dependencys
			/// </summary>
			std::atomic<unsigned int> m_dependencysCount = 0;

			/// <summary>
			/// Read dependancy fence values
			/// </summary>
			std::atomic<UINT64> m_arrWaitDependency[CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES] = {};

			/// <summary>
			/// Dependants of the class
			/// </summary>
			MT::ObjectGuard<XCommandList_DependantsList<CLS_DX_XCommandList__MAX_DEPENDENCYS>, MT::SpinLock<THREAD_PAUSE_FUNCTION_F>> m_dependants;
			
		// Subclass
		public:
			/// <summary>
			/// Wait object
			/// </summary>
			class WaitObject {
				public:
					/// <summary>
					/// Default construct
					/// </summary>
					WaitObject() = default;

					/// <summary>
					/// Create wait object for command list
					/// </summary>
					/// <param name="ptrCommandList">Command list pointersd</param>
					/// <param name="value">Value to wait for</param>
					WaitObject(XCommandList* ptrCommandList, UINT64 value) noexcept;

					/// <summary>
					/// Checks if is done
					/// </summary>
					/// <returns></returns>
					bool isDone();

					/// <summary>
					/// Wait until execution is done
					/// </summary>
					void wait();

					/// <summary>
					/// Extract the wait object value
					/// </summary>
					/// <returns>Value</returns>
					UINT64 value() noexcept;

					/// <summary>
					/// Try to convert wait object to fence wait object
					/// </summary>
					/// <returns>Wait object (can be null)</returns>
					XFence::WaitObject convertToFenceWaitObject();

					/// <summary>
					/// Call wait
					/// </summary>
					void operator()();

					/// <summary>
					/// Compare operator
					/// </summary>
					bool operator==(const WaitObject& other);

					/// <summary>
					/// Check if it is done
					/// </summary>
					operator bool();
				private:
					friend class ::DX::XCommandList;

					/// <summary>
					/// Value of wait object
					/// </summary>
					UINT64 m_value = 0;

					/// <summary>
					/// Command list pointer
					/// </summary>
					XCommandList* m_ptrCommandList = nullptr;
			};
	};
}
