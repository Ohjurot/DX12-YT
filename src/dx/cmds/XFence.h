#pragma once
#include <DefHeader.h>

namespace DX {
	/// <summary>
	/// Fence
	/// </summary>
	class XFence : public ScopedComPointer<ID3D12Fence> {
		// Predeclarations	
		public:
			class WaitObject;

		// Class
		public:
			/// <summary>
			/// Default empty object
			/// </summary>
			XFence() = default;

			/// <summary>
			/// Create fence from adapter
			/// </summary>
			/// <param name="ptrAdapter">Pointer to device</param>
			XFence(ID3D12Device* ptrDevice);

			/// <summary>
			/// Retrive the value for the fence
			/// </summary>
			/// <returns></returns>
			UINT64 getValue() noexcept;

			/// <summary>
			/// Signal the fence
			/// </summary>
			/// <param name="value">new value</param>
			void signal(UINT64 value);

			/// <summary>
			/// Convert to uint64
			/// </summary>
			operator UINT64() noexcept;

			/// <summary>
			/// Create a wait object for fence
			/// </summary>
			/// <param name="value"></param>
			/// <returns></returns>
			DX::XFence::WaitObject createWaitObject(UINT64 value);

		// Sub classes
		public:
			/// <summary>
			/// Create wait object for fence
			/// </summary>
			class WaitObject {
				public:
					/// <summary>
					/// Create wait object
					/// </summary>
					/// <param name="refFence">Reference fence</param>
					/// <param name="value">Reference value</param>
					WaitObject(XFence& refFence, UINT64 value);

					/// <summary>
					/// Check if the operation is done
					/// </summary>
					/// <returns>True if done</returns>
					bool isDone() noexcept;

					/// <summary>
					/// Execute a blocking wait
					/// </summary>
					void wait();

					/// <summary>
					/// Check if completed
					/// </summary>
					/// <returns></returns>
					operator bool() noexcept;

					/// <summary>
					/// Execute operator
					/// </summary>
					void operator()();
				private:
					/// <summary>
					/// Reference to fence
					/// </summary>
					XFence& m_fence;

					/// <summary>
					/// Wait value
					/// </summary>
					const UINT64 m_value;
			};
	};
}