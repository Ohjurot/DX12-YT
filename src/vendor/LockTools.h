#pragma once

/// <summary>
/// Multithread
/// </summary>
namespace MT {
	/// <summary>
	/// Pause function pointer
	/// </summary>
	typedef void(*PAUSE_FUNCTION)(void);

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
			/// Retrive object pointer
			/// </summary>
			/// <returns>Pointer to object</returns>
			T* ptr() noexcept {
				return m_ptrObject;
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

	/// <summary>
	/// Proxy for RW multiaccess object
	/// </summary>
	/// <typeparam name="T">Guarded type</typeparam>
	/// <typeparam name="IS_CONST">Indicates wather the access to the data is const</typeparam>
	template<typename T, bool IS_CONST>
	class MultiAccessProxy {
		public:
			MultiAccessProxy() = default;

			/// <summary>
			/// Construct
			/// </summary>
			/// <param name="ptrElement">Pointer to element</param>
			/// <param name="ptrCounter">Pointer to counter</param>
			MultiAccessProxy(T* ptrElement, std::atomic<unsigned int>* ptrCounter) noexcept :
				m_ptrElement(ptrElement),
				m_ptrCounter(ptrCounter)
			{}

			/// <summary>
			/// Destructor
			/// </summary>
			~MultiAccessProxy() noexcept {
				release();
			}

			void release() noexcept {
				// Decrement coutner
				if (m_ptrCounter) {
					(*m_ptrCounter)--;
					m_ptrElement = nullptr;
					m_ptrCounter = nullptr;
				}
			}

			/// <summary>
			/// Check if valid
			/// </summary>
			/// <returns>True if valid</returns>
			operator bool() noexcept{
				return m_ptrElement;
			}

			/// <summary>
			/// Checks if write is enabled
			/// </summary>
			/// <returns>true if enabled</returns>
			constexpr bool isWriteEnable() {
				return !IS_CONST;
			}

			/// <summary>
			/// Conversion to const pointer
			/// </summary>
			/// <returns>const Pointer to T</returns>
			template<typename = std::enable_if_t<IS_CONST>>
			operator const T*() noexcept{
				return (const T*)m_ptrElement;
			}

			/// <summary>
			/// Conversion to const pointer
			/// </summary>
			/// <returns>const Pointer to T</returns>
			template<typename = std::enable_if_t<!IS_CONST>>
			operator T* () noexcept {
				return m_ptrElement;
			}

			/// <summary>
			/// Pointer operator
			/// </summary>
			/// <returns>const Pointer to T</returns>
			template<typename = std::enable_if_t<IS_CONST>>
			const T* operator->() noexcept{
				return (const T*)m_ptrElement;
			}

			/// <summary>
			/// Pointer operator
			/// </summary>
			/// <returns>Pointer to T</returns>
			template<typename = std::enable_if_t<!IS_CONST>>
			T* operator->() noexcept {
				return m_ptrElement;
			}

			/// <summary>
			/// Pointer function
			/// </summary>
			/// <returns>const Pointer to T</returns>
			template<typename = std::enable_if_t<IS_CONST>>
			const T* ptr() noexcept {
				return (const T*)m_ptrElement;
			}

			/// <summary>
			/// Pointer function
			/// </summary>
			/// <returns>Pointer to T</returns>
			template<typename = std::enable_if_t<!IS_CONST>>
			T* ptr() noexcept {
				return m_ptrElement;
			}


			MultiAccessProxy& operator=(const MultiAccessProxy& other) {
				m_ptrCounter = other.m_ptrCounter;
				m_ptrElement = other.m_ptrElement;
				other.m_ptrCounter = nullptr;

				return *this;
			}

			// Delete
			MultiAccessProxy(const MultiAccessProxy&) = delete;
		private:
			/// <summary>
			/// Pointer to element
			/// </summary>
			T* m_ptrElement = nullptr;

			/// <summary>
			/// Atomic counter pointer
			/// </summary>
			std::atomic<unsigned int>* m_ptrCounter = nullptr;
	};

	/// <summary>
	/// Guard the object for multiple read singel write access
	/// </summary>
	/// <typeparam name="T">Type</typeparam>
	/// <typeparam name="LT">Lock type</typeparam>
	/// <typeparam name="PAUSE_FUNCTION">Pause function pointer</typeparam>
	template<typename T, typename LT, PAUSE_FUNCTION PAUSE = nullptr>
	class MultiAccessGuard {
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			MultiAccessGuard() :
				m_element()
			{}

			/// <summary>
			/// Construct copy
			/// </summary>
			/// <param name="element">Input element</param>
			MultiAccessGuard(T& element) 
				: m_element(element)
			{}

			/// <summary>
			/// Construct move
			/// </summary>
			/// <param name="element">input element</param>
			MultiAccessGuard(T&& element) :
				m_element(std::move(element))
			{}

			/// <summary>
			/// Destructor
			/// </summary>
			~MultiAccessGuard() noexcept {
				// Wait until done
				while (countAccess()) {
					__pause();
				}
			}

			/// <summary>
			/// Count numeber of access
			/// </summary>
			/// <returns>Number of current access</returns>
			unsigned int countAccess() noexcept{
				return m_counterRead.load(std::memory_order_relaxed) + m_counterWrite.load(std::memory_order_relaxed);
			}

			/// <summary>
			/// Request read access to the object (Blocking)
			/// </summary>
			/// <returns>Acces Proxy</returns>
			MultiAccessProxy<T, true> requestRead() noexcept {
				LockGuard janitor(m_lock);

				// Write check
				while (m_counterWrite.load(std::memory_order_relaxed) != 0) {
					__pause();
				}

				// Build read
				m_counterRead++;
				return MultiAccessProxy<T, true>(&m_element, &m_counterRead);
			}

			/// <summary>
			/// Request write access to the object (Blocking)
			/// </summary>
			/// <returns>Acces Proxy</returns>
			MultiAccessProxy<T, false> requestWrite() noexcept {
				LockGuard janitor(m_lock);

				// Write check
				while (m_counterRead.load(std::memory_order_relaxed) != 0 || m_counterWrite.load(std::memory_order_relaxed) != 0) {
					__pause();
				}

				// Build write
				m_counterWrite++;
				return MultiAccessProxy<T, false>(&m_element, &m_counterWrite);
			}

			// Delete
			MultiAccessGuard(const MultiAccessGuard&) = delete;
			void operator=(const MultiAccessGuard&) = delete;
		private:
			/// <summary>
			/// Pause function for no pause
			/// </summary>
			template<typename = std::enable_if_t<(PAUSE == nullptr)>>
			inline void __pause(int i = 0) {
				// noop
			}

			/// <summary>
			/// Pause function for pause
			/// </summary>
			template<typename = std::enable_if_t<(PAUSE != nullptr)>>
			inline void __pause(unsigned int i = 0) {
				PAUSE();
			}

		private:
			/// <summary>
			/// Internal element
			/// </summary>
			T m_element;

			/// <summary>
			/// Internal get lock
			/// </summary>
			LT m_lock;

			/// <summary>
			/// RW Counter
			/// </summary>
			std::atomic<unsigned int> m_counterRead = 0, m_counterWrite = 0;
	};
}
