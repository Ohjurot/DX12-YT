#pragma once
#include<DefHeader.h>

namespace MT {
	/// <summary>
	/// Spin lock
	/// </summary>
	template<PAUSE_FUNCTION F = nullptr>
	class SpinLock : public MT::ILock{
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			SpinLock() = default;
			
			/// <summary>
			/// Aquire the SpinLock (blocking)
			/// </summary>
			void aquire() noexcept override {
				// While aquesition fails wait
				while (!tryAquire()) {
					if (F) {
						F();
					} else {
						_mm_pause();
					}
				}
			}

			/// <summary>
			/// Try to aquire the SpinLock (non blocking)
			/// </summary>
			/// <returns>True if aquesition succeeded</returns>
			bool tryAquire() noexcept override {
				return !m_flag.test_and_set(std::memory_order_acquire);
			}

			/// <summary>
			/// Release the SpinLock
			/// </summary>
			void release() noexcept override {
				m_flag.clear(std::memory_order_release);
			}

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