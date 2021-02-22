#pragma once
#include <DefHeader.h>

#include <dx/cmds/XFence.h>
#include <dx/cmds/XCommandList.h>
#include <dx/cmds/CommandListManager.h>

namespace DX {
	/// <summary>
	/// Command list access object
	/// </summary>
	class CommandListAccessObject {
		public:
			/// <summary>
			/// Create command list access object
			/// </summary>
			/// <param name="type">Type of command list requested</param>
			CommandListAccessObject(D3D12_COMMAND_LIST_TYPE type);

			/// <summary>
			/// Destruct
			/// </summary>
			~CommandListAccessObject();

			/// <summary>
			/// Release object
			/// </summary>
			void release();

			/// <summary>
			/// Execute and exchange access object
			/// </summary>
			/// <returns>Wait object</returns>
			DX::XCommandList::WaitObject executeExchange();
			
			/// <summary>
			/// Execute and close access object
			/// </summary>
			/// <returns>Wait object</returns>
			DX::XCommandList::WaitObject executeClose();

			/// <summary>
			/// Precreates a work object (marks list dirty)
			/// </summary>
			/// <returns>wait object</returns>
			DX::XCommandList::WaitObject createWaitObject();

			/// <summary>
			/// Add dependency to list
			/// </summary>
			/// <param name="waitObject">Input dependancy</param>
			void addDependency(XCommandList::WaitObject& waitObject);

			/// <summary>
			/// Pointer operator for GFX Command list
			/// </summary>
			/// <returns></returns>
			ID3D12GraphicsCommandList* operator->();

			/// <summary>
			/// Pointer conversion operator
			/// </summary>
			operator ID3D12GraphicsCommandList*();

			/// <summary>
			/// Bool conversion operator
			/// </summary>
			operator bool() noexcept;

			// Delete unnused
			CommandListAccessObject(const CommandListAccessObject&) = delete;
			void operator=(const CommandListAccessObject&) = delete;

		private:
			/// <summary>
			/// Command list pointer
			/// </summary>
			XCommandList* m_ptrCommandList;

			/// <summary>
			/// Indicates that the list was used
			/// </summary>
			bool m_bIsDirty = false;
	};
}