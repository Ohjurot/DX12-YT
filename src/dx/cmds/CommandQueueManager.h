#pragma once
#include <DefHeader.h>

#include <dx/cmds/XCommandQueue.h>

#define CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES 3

namespace DX {
	/// <summary>
	/// Command que manager singelton
	/// </summary>
	class CommandQueueManager {
		// Class
		public:
			/// <summary>
			/// Create all command ques
			/// </summary>
			/// <param name="ptrDevice">Pointer to device</param>
			void createInternalObjects(ID3D12Device* ptrDevice);

			/// <summary>
			/// Destroy internal objects
			/// </summary>
			void destroyInternalObjects() noexcept;

			/// <summary>
			/// Retrive the command queue for a type
			/// </summary>
			/// <param name="type">Input type</param>
			/// <returns>Command queue reference</returns>
			XCommandQueue& getCommandQueue(D3D12_COMMAND_LIST_TYPE type);

		private:
			/// <summary>
			/// Array of command ques
			/// </summary>
			XCommandQueue m_arrCommandQueues[CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES];

			/// <summary>
			/// Array of command que types
			/// </summary>
			const D3D12_COMMAND_LIST_TYPE mc_arrCommandQueueTypes[CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES] = {
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				D3D12_COMMAND_LIST_TYPE_COPY,
				D3D12_COMMAND_LIST_TYPE_COMPUTE
			};

		// Singelton
		public:
			/// <summary>
			/// Retrive the static instance of the command que manger
			/// </summary>
			/// <returns></returns>
			static CommandQueueManager& getInstance();

			// Delete unsupported
			CommandQueueManager(const CommandQueueManager&) = delete;
			void operator=(const CommandQueueManager&) = delete;
		private:
			// Singelton
			CommandQueueManager() {};

			/// <summary>
			/// Static / Singelton instance
			/// </summary>
			static CommandQueueManager s_instance;
	};
}