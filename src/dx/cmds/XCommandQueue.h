#pragma once
#include <DefHeader.h>

namespace DX {
	class CommandQueueManager;

	/// <summary>
	/// Device pointer
	/// </summary>
	class XCommandQueue : public ScopedComPointer<ID3D12CommandQueue> {
		public:
			/// <summary>
			/// Retrive the type of the queue
			/// </summary>
			/// <returns>Command list type</returns>
			const D3D12_COMMAND_LIST_TYPE getQueueType();

			/// <summary>
			/// Increments ref counter
			/// </summary>
			void incrementRef() noexcept;

			/// <summary>
			/// Decrements ref counter
			/// </summary>
			void decrementRef() noexcept;

			/// <summary>
			/// Release function
			/// </summary>
			void release() override;
		protected:
			// Allow command queue manager access
			friend class CommandQueueManager;

			/// <summary>
			/// Default empty object
			/// </summary>
			XCommandQueue() = default;

			/// <summary>
			/// Create command que of type
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="type">type of command list</param>
			XCommandQueue(ID3D12Device* ptrDevice, D3D12_COMMAND_LIST_TYPE type);

			/// <summary>
			/// Copy operator
			/// </summary>
			/// <param name="other"></param>
			XCommandQueue& operator=(XCommandQueue& other);
		private:
			/// <summary>
			/// Type of the command list
			/// </summary>
			D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			/// <summary>
			/// Atomic counter for list references
			/// </summary>
			std::atomic<UINT64> m_refCounter = 0;
	};
}