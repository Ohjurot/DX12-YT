#include <DefHeader.h>

#include <dx/core/XDebug.h>

namespace DX {
	/// <summary>
	/// Info Queue
	/// </summary>
	class XInfoQueue : public ScopedComPointer<ID3D12InfoQueue> {
		public:
			/// <summary>
			/// Default empty debug device
			/// </summary>
			XInfoQueue() = default;

			/// <summary>
			/// Create debugdevice from adapter
			/// </summary>
			/// <param name="ptrDevice">Pointer to device</param>
			XInfoQueue(ID3D12Device* ptrDevice);

			/// <summary>
			/// Retrive the number of messages in the que
			/// </summary>
			/// <returns>Number of messages</returns>
			uint64_t getMessageCount() noexcept;

			/// <summary>
			/// Retrive message
			/// </summary>
			/// <param name="index">Index of the message to retrive</param>
			/// <param name="ptrMessage">Pointer to message</param>
			/// <returns>Size of the message</returns>
			size_t getMessage(uint64_t index, D3D12_MESSAGE* ptrMessage) noexcept;

			/// <summary>
			/// Registers a message callback
			/// </summary>
			/// <param name="fptrMessageFunction">Callback function</param>
			/// <param name="functionData">Pointer input for callback function</param>
			/// <param name="flags">Input flags</param>
			/// <returns>Message associated cookie value or NULL</returns>
			DWORD registerCallback(D3D12MessageFunc fptrMessageFunction, void* functionData = nullptr, D3D12_MESSAGE_CALLBACK_FLAGS flags = D3D12_MESSAGE_CALLBACK_FLAG_NONE) noexcept;

			/// <summary>
			/// Unregisters a message callback
			/// </summary>
			/// <param name="cookie">Associated callback cookie</param>
			/// <returns>true if unregister succeeds</returns>
			bool unregisterCallback(DWORD cookie) noexcept;
	};
}