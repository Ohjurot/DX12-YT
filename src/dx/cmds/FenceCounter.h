#pragma once
#include <DefHeader.h>

#include <dx/cmds/XFence.h>

namespace DX {
	/// <summary>
	/// Represents a atomic fence counter for guarding a fence
	/// </summary>
	class FenceCounter {
		// Forward decl
		public:
			class Frontend;
		
		// Class
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			FenceCounter() = default;

			/// <summary>
			/// Create counter on existing fence
			/// </summary>
			/// <param name="fence">Input fence</param>
			FenceCounter(XFence& fence);

			/// <summary>
			/// Release own members
			/// </summary>
			void release() noexcept;

			/// <summary>
			/// Retrives the counters head
			/// </summary>
			/// <returns>Highst fence value or zero</returns>
			UINT64 head() noexcept;

			/// <summary>
			/// Set a new fence object
			/// </summary>
			/// <param name="newFence">New fence</param>
			void setFenceObject(XFence& newFence);

			/// <summary>
			/// Creates a frontend for the counter
			/// </summary>
			/// <returns>Frontend</returns>
			DX::FenceCounter::Frontend newFrontend() noexcept;

			/// <summary>
			/// Assign operator
			/// </summary>
			/// <param name="other"></param>
			void operator=(XFence& other);

			/// <summary>
			/// Int conversion operator
			/// </summary>
			/// <returns></returns>
			operator UINT64() noexcept;

			/// <summary>
			/// Bool conversion operator
			/// </summary>
			/// <returns></returns>
			operator bool() noexcept;

			// Delete unsupported
			FenceCounter(const FenceCounter&) = delete;
			void operator=(const FenceCounter&) = delete;
		// Class
		protected:
			/// <summary>
			/// Retrive the next fence value
			/// </summary>
			/// <returns>Next fence value</returns>
			UINT64 next() noexcept;

		// Class
		private:
			/// <summary>
			/// Fence object
			/// </summary>
			XFence m_fence;

			/// <summary>
			/// Counter value
			/// </summary>
			std::atomic<UINT64> m_counter = 0;

			/// <summary>
			/// Lock any operation on object
			/// </summary>
			std::atomic_flag m_opLock = ATOMIC_FLAG_INIT;

		// Sub clases
		public:
			/// <summary>
			/// Counter frontend
			/// </summary>
			class Frontend {
				public:
					/// <summary>
					/// Default fontend
					/// </summary>
					Frontend() = default;

					/// <summary>
					/// Destruct
					/// </summary> 
					~Frontend();

					/// <summary>
					/// Create frontend from counter
					/// </summary>
					/// <param name="refCounter"></param>
					Frontend(FenceCounter& refCounter) noexcept;

					/// <summary>
					/// Clear internal pointer and head
					/// </summary>
					void release() noexcept;

					/// <summary>
					/// Get the next value (sets head)
					/// </summary>
					/// <returns>New head to signal</returns>
					UINT64 next();

					/// <summary>
					/// Peek the current head value
					/// </summary>
					/// <returns></returns>
					UINT64 head() noexcept;

					/// <summary>
					/// Retrive fence pointer
					/// </summary>
					/// <returns>Fence reference</returns>
					DX::XFence& fence();

					/// <summary>
					/// Create wait object for current head
					/// </summary>
					/// <returns></returns>
					DX::XFence::WaitObject getCurrentWaitObject();

					/// <summary>
					/// Bool operator
					/// </summary>
					/// <returns></returns>
					operator bool() noexcept;
				
					/// <summary>
					/// XFence reference operator
					/// </summary>
					operator DX::XFence&();

					/// <summary>
					/// Fence pointer conversion operator
					/// </summary>
					operator ID3D12Fence* ();

					/// <summary>
					/// Assign new fence counter (flush head)
					/// </summary>
					/// <param name="other">New counter</param>
					void operator=(FenceCounter& other);
				private:
					/// <summary>
					/// Assesioated fence counter
					/// </summary>
					FenceCounter* m_ptrCounter = nullptr;

					/// <summary>
					/// Head of Frontend
					/// </summary>
					UINT64 m_head = 0;
			};
	};
}