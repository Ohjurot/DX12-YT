#pragma once
#include <DefHeader.h>

#include <dx/cmds/XCommandQueue.h>
#include <dx/cmds/FenceCounter.h>

namespace DX {
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
			DX::XFence::WaitObject execute();

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

			// Delete unsupported
			XCommandList(const XCommandList&) = delete;
			void operator=(const XCommandList&) = delete;
		
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
					/// Checks if wait object is valid
					/// </summary>
					/// <returns>True if valid</returns>
					bool isValid();

					/// <summary>
					/// Check if it is done
					/// </summary>
					operator bool();

					/// <summary>
					/// Call wait
					/// </summary>
					void operator()();
				private:
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