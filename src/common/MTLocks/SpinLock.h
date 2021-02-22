#pragma once
#include<DefHeader.h>

namespace MT {
	/// <summary>
	/// Spin lock
	/// </summary>
	class SpinLock : public MT::ILock{
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			SpinLock() = default;
			
			/// <summary>
			/// Aquire the SpinLock (blocking)
			/// </summary>
			void aquire() noexcept override;

			/// <summary>
			/// Try to aquire the SpinLock (non blocking)
			/// </summary>
			/// <returns>True if aquesition succeeded</returns>
			bool tryAquire() noexcept override;

			/// <summary>
			/// Release the SpinLock
			/// </summary>
			void release() noexcept override;

			// Delete
			SpinLock(const SpinLock& other) = delete;
			void operator=(const SpinLock& other) = delete;
		private:
			/// <summary>
			/// Locks flag
			/// </summary>
			std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
	};
}