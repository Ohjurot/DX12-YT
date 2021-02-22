#pragma once

/// <summary>
/// Multithread
/// </summary>
namespace MT {
	/// <summary>
	/// Lock interface
	/// </summary>
	class ILock {
		public:
			/// <summary>
			/// Try to aquire a lock
			/// </summary>
			/// <returns>true if aquestion succeded</returns>
			virtual bool tryAquire() noexcept = 0;

			/// <summary>
			/// Aquire a lock
			/// </summary>
			virtual void aquire() noexcept = 0;

			/// <summary>
			/// Release a lock
			/// </summary>
			virtual void release() noexcept = 0;
	};

	/// <summary>
	/// Guard for a lock
	/// </summary>
	class LockGuard {
		public:
			/// <summary>
			/// Construct from lock ref (Whill block and require)
			/// </summary>
			/// <param name="refLock">Lock to guard</param>
			LockGuard(ILock& refLock) noexcept :
				m_ptrLock(&refLock)
			{
				// Aquire lock
				m_ptrLock->aquire();
			}

			/// <summary>
			/// Destrucktor for freeing the lock
			/// </summary>
			~LockGuard() noexcept {
				// Release lock
				release();
			}

			/// <summary>
			/// Release lock
			/// </summary>
			void release() noexcept {
				// Only if lock is valid
				if (m_ptrLock) {
					// Release and unset
					m_ptrLock->release();
					m_ptrLock = nullptr;
				}
			}

			// Delete
			LockGuard(const LockGuard&) = delete;
			void operator=(const LockGuard&) = delete;
		private:
			/// <summary>
			/// Lock
			/// </summary>
			ILock* m_ptrLock;
	};

	template<typename T>
	class LockedObjectProxy {
		public:
			/// <summary>
			/// Create proxy for object
			/// </summary>
			/// <param name="ptrObject">Pointer to object</param>
			/// <param name="refLock">Reference to lock that guard object</param>
			LockedObjectProxy(T* ptrObject, ILock& refLock) noexcept :
				m_ptrObject(ptrObject),
				m_guard(refLock)
			{}

			/// <summary>
			/// Destructor
			/// </summary>
			~LockedObjectProxy() noexcept {
				release();
			}

			/// <summary>
			/// Release function
			/// </summary>
			void release() noexcept{
				// Release and unset
				m_guard.release();
				m_ptrObject = nullptr;
			}

			/// <summary>
			/// Convert to reference
			/// </summary>
			operator T&() {
				return *m_ptrObject;
			}

			/// <summary>
			/// Convert to pointer
			/// </summary>
			explicit operator T* () {
				return m_ptrObject;
			}

			/// <summary>
			/// Pointer operator
			/// </summary>
			/// <returns>Pointer to object</returns>
			T* operator->() {
				return m_ptrObject;
			}

			// Delete
			LockedObjectProxy(const LockedObjectProxy&) = delete;
			void operator=(const LockedObjectProxy&) = delete;
		private:
			/// <summary>
			/// Object to access
			/// </summary>
			T* m_ptrObject; 

			/// <summary>
			/// Guar
			/// </summary>
			LockGuard m_guard;
	};

	/// <summary>
	/// Guarded object
	/// </summary>
	/// <typeparam name="T">Type of object</typeparam>
	/// <typeparam name="LT">Type of lock</typeparam>
	template<typename T, typename LT>
	class ObjectGuard {
		// Class
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			ObjectGuard() = default;

			/// <summary>
			/// Copy construct
			/// </summary>
			/// <param name="object">Object to copy</param>
			ObjectGuard(T& object) :
				m_object(object)
			{}

			/// <summary>
			/// Move constructor
			/// </summary>
			/// <param name="object">Object to move</param>
			ObjectGuard(T&& object) :
				m_object(std::move(object))
			{}

			/// <summary>
			/// Copy construct (same type)
			/// </summary>
			/// <param name="other">other object</param>
			ObjectGuard(const ObjectGuard& other) {
				*this = other;
			}

			/// <summary>
			/// Copy operator
			/// </summary>
			/// <param name="other">Other object</param>
			/// <returns>Ref to this</returns>
			ObjectGuard& operator=(const ObjectGuard& other) noexcept{
				// Guard
				MT::LockGuard selfJanitor(m_lock);
				MT::LockGuard otherJanitor(other.m_lock);

				// Copy
				m_object = other.m_object;

				//Return
				return *this;
			}

			/// <summary>
			/// Retrive proxy to object
			/// </summary>
			/// <returns>Proxy to object</returns>
			LockedObjectProxy<T> get() noexcept {
				return LockedObjectProxy(&m_object, m_lock);
			}

		private:
			/// <summary>
			/// Guarded object
			/// </summary>
			T m_object;

			/// <summary>
			/// Guarding lock
			/// </summary>
			LT m_lock;
	};
}
