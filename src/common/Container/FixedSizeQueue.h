#pragma once

#include <DefHeader.h>
#include <common/MT/Locks/SpinLock.h>
#include <common/Memory/QuadTreePoolAllocator.h>


namespace Container {
	// Predecl
	template<typename T, unsigned int N, PAUSE_FUNCTION F>
	class FixedSizeQueue;

	/// <summary>
	/// Element for fixed size list
	/// </summary>
	/// <typeparam name="T">Type of element</typeparam>
	template<typename T>
	struct FixedSizeQueue_Element {
		/// <summary>
		/// Element
		/// </summary>
		T element = T();

		/// <summary>
		/// Next element
		/// </summary>
		FixedSizeQueue_Element* ptrNext = nullptr;

		/// <summary>
		/// Allocation proxy
		/// </summary>
		Memory::QuadTreePoolAllocator_AllocatioProxy<FixedSizeQueue_Element> proxy;
	};

	/// <summary>
	/// List peask object
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T, unsigned int N, PAUSE_FUNCTION F>
	class FixedSizeQueue_Peak {
		public:
			/// <summary>
			/// Default construct
			/// </summary>
			FixedSizeQueue_Peak() noexcept = default;
			
			/// <summary>
			/// Construct from proxy
			/// </summary>
			/// <param name="ptrProxy">Proxy pointer</param>
			FixedSizeQueue_Peak(Memory::QuadTreePoolAllocator_AllocatioProxy<FixedSizeQueue_Element<T>>* ptrProxy) noexcept :
				m_ptrProxy(ptrProxy)
			{};

			/// <summary>
			/// Copy operator (move)
			/// </summary>
			/// <param name="other">Other object</param>
			FixedSizeQueue_Peak(FixedSizeQueue_Peak& other) noexcept {
				m_ptrProxy = other.m_ptrProxy;
				other.m_ptrProxy = nullptr;
			}

			/// <summary>
			/// Destruct
			/// </summary>
			~FixedSizeQueue_Peak() noexcept {
				if (m_ptrProxy) {
					// Free memory
					m_ptrProxy->release();
				}
			}

			/// <summary>
			/// Takeover the object (copy from allocator)
			/// </summary>
			/// <param name="ppT">Pointer to target</param>
			/// <returns>true on succeed</returns>
			bool takeover(T* ppT) noexcept {
				if (m_ptrProxy) {
					*ppT = *ptr();
					m_ptrProxy->release();
					m_ptrProxy = nullptr;
					return true;
				}

				return false;
			}

			/// <summary>
			/// Assign operator
			/// </summary>
			/// <param name="other">Other element</param>
			FixedSizeQueue_Peak<T, N, F>& operator=(FixedSizeQueue_Peak<T, N, F>& other) noexcept {
				m_ptrProxy = other.m_ptrProxy;
				other.m_ptrProxy = nullptr;
				return *this;
			}

			/// <summary>
			/// Pointer function
			/// </summary>
			/// <returns>Pointer to T</returns>
			T* ptr() noexcept {
				if (m_ptrProxy) {
					return &m_ptrProxy->ptr()->element;
				}

				return nullptr;
			}

			/// <summary>
			/// Pointer operator
			/// </summary>
			/// <returns>Pointer to T</returns>
			T* operator->() noexcept {
				return ptr();
			}

			/// <summary>
			/// T* conversion
			/// </summary>
			operator T* () noexcept {
				return ptr();
			}

			/// <summary>
			/// Bool conversion operator
			/// </summary>
			/// <returns>true if proxy is not nullptr</returns>
			operator bool() noexcept {
				return m_ptrProxy != nullptr;
			}

		protected:
			friend class FixedSizeQueue<T, N, F>;

			/// <summary>
			/// Peak proxy
			/// </summary>
			Memory::QuadTreePoolAllocator_AllocatioProxy<FixedSizeQueue_Element<T>>* m_ptrProxy = nullptr;
	};

	/// <summary>
	/// Fixed size list
	/// </summary>
	/// <typeparam name="T">List element type</typeparam>
	/// <typeparam name="N">Number of list elements</typeparam>
	template<typename T, unsigned int N, PAUSE_FUNCTION F = &THREAD_PAUSE_FUNCTION_F>
	class FixedSizeQueue {
		public:
			/// <summary>
			/// Push and new element on the stack (copy)
			/// </summary>
			/// <param name="element">Input element</param>
			/// <returns>True if succeed</returns>
			template<typename TR = T, typename = std::enable_if_t<!std::is_reference_v<TR>>>
			bool push(T element) noexcept {
				return push<T&>(element);
			}

			/// <summary>
			/// Push and new element on the stack (ref)
			/// </summary>
			/// <param name="element">Ref input element</param>
			/// <returns>True if succeed</returns>
			template<typename TN = T&, typename = std::enable_if_t<std::is_reference_v<TN>>>
			bool push(T& element) noexcept {
				// Try allocated
				Memory::QuadTreePoolAllocator_AllocatioProxy<FixedSizeQueue_Element<T>> tempProxy;
				if (!m_allocator.allocate(tempProxy)) {
					return false;
				}

				// Get last
				auto last = m_last.get();
				if (*last.ptr()) {
					// Relink last
					(*last.ptr())->ptrNext = tempProxy.ptr();
				}
				*last.ptr() = tempProxy.ptr();

				// Optional link first
				auto first = m_first.get();
				if (!(*(first.ptr()))) {
					*(first.ptr()) = tempProxy.ptr();
				}
				first.release();

				// Copy element
				tempProxy->element = element;
				tempProxy->ptrNext = nullptr;
				tempProxy->proxy = std::move(tempProxy);

				return true;
			}

			/// <summary>
			/// Push and new element on the stack
			/// </summary>
			/// <param name="refPeak">Input peak element</param>
			/// <returns>True if succeed</returns>
			bool push(FixedSizeQueue_Peak<T, N, F>& refPeak) noexcept {
				// Proxy need to be valid
				if (refPeak.m_ptrProxy) {
					// Get last
					auto last = m_last.get();
					if (*last.ptr()) {
						// Relink last
						(*last.ptr())->ptrNext = refPeak.m_ptrProxy->ptr();
					}
					*last.ptr() = refPeak.m_ptrProxy->ptr();
					
					// Optional link first
					auto first = m_first.get();
					if (!(*(first.ptr()))) {
						*(first.ptr()) = refPeak.m_ptrProxy->ptr();
					}
					first.release();

					// Unset peak and return true
					refPeak.m_ptrProxy = nullptr;
					return true;
				}

				return false;
			}

			/// <summary>
			/// POP first element of the queue
			/// </summary>
			/// <returns>Peak element object (Lives still on queues allocator)</returns>
			FixedSizeQueue_Peak<T, N, F> pop() {
				// Get first pointer
				auto first = m_first.get();
				auto firstPointer = *first.ptr();
				if (firstPointer) {
					// End remove link
					auto last = m_last.get();
					if (*last.ptr() == *first.ptr()) {
						*last.ptr() = nullptr;
					}
					last.release();
					
					// Relink
					*first.ptr() = firstPointer->ptrNext;

					// Return object
					firstPointer->proxy->ptrNext = nullptr;
					return FixedSizeQueue_Peak<T, N, F>(&firstPointer->proxy);
				}

				// Return null object
				return FixedSizeQueue_Peak<T, N, F>();
			}

		private:
			/// <summary>
			/// Allocator for the list
			/// </summary>
			Memory::QuadTreePoolAllocator<FixedSizeQueue_Element<T>, N> m_allocator;

			/// <summary>
			/// First and last pointer
			/// </summary>
			MT::ObjectGuard<FixedSizeQueue_Element<T>*, MT::SpinLock<F>> m_first = nullptr, m_last = nullptr;

	};
}