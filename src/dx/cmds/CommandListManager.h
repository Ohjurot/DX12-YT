#pragma once

#include <dx/cmds/CommandQueueManager.h>
#include <dx/cmds/XCommandList.h>

#define CLS_DX_CommandListManager__NUM_COMMAND_LISTS_PER_TYPE 32 * 2

namespace DX {
	/// <summary>
	/// Entry for command list manager
	/// </summary>
	struct CommandListManger_ListEntry {
		/// <summary>
		/// If set list is used externaly
		/// </summary>
		std::atomic_flag locked_flag = ATOMIC_FLAG_INIT;

		/// <summary>
		/// Command list object
		/// </summary>
		XCommandList commandList;
	};
	
	/// <summary>
	/// Command list manager
	/// </summary>
	class CommandListManager {
		// Class
		public:
			/// <summary>
			/// Create all internal objects
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			void createInternalObjects(ID3D12Device* ptrDevice);

			/// <summary>
			/// Destroy internal objects
			/// </summary>
			void destroyInternalObjects() noexcept;

			/// <summary>
			/// Refresh internal manged objects
			/// </summary>
			void refreshInternal();

			/// <summary>
			/// Lock and get a list 
			/// </summary>
			/// <param name="type">Type of list</param>
			/// <returns>Command List pointer</returns>
			XCommandList* lockList(D3D12_COMMAND_LIST_TYPE type);

			/// <summary>
			/// Unlock list / return list
			/// </summary>
			/// <param name="ptrList">List pointer</param>
			/// <param name="execute">True if list requires execution</param>
			void unlockList(XCommandList* ptrList, bool execute = true);

		private:
			/// <summary>
			/// Fences per type
			/// </summary>
			XFence m_fences[CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES] = {};
			
			/// <summary>
			/// Counters per type
			/// </summary>
			FenceCounter m_fenceCounters[CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES] = {};

			/// <summary>
			/// Array of command lists
			/// </summary>
			CommandListManger_ListEntry m_commandList2DArr[CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES][CLS_DX_CommandListManager__NUM_COMMAND_LISTS_PER_TYPE] = {};

		// Singelton
		public:
			/// <summary>
			/// Retrive the instance of the command list manger
			/// </summary>
			/// <returns>Refernce to instance</returns>
			static CommandListManager& getInstance() noexcept;

		private:
			/// <summary>
			/// Default constructor
			/// </summary>
			CommandListManager() {};

			/// <summary>
			/// Static instance
			/// </summary>
			static CommandListManager s_instance;
	};
}