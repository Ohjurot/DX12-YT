#pragma once

#include <DefHeader.h>

namespace Memory {
	/// <summary>
	/// Interface for rescursive flag clearing
	/// </summary>
	class QuadTreePoolAllocator_IClearable {
		public:
			/// <summary>
			/// Clear function
			/// </summary>
			/// <param name="mask">Mask to clear</param>
			virtual void clear(unsigned char mask = 0xFF) = 0;
	};

	/// <summary>
	/// Proxy class to allocation element
	/// </summary>
	/// <typeparam name="T">Type of element</typeparam>
	template<typename T>
	class QuadTreePoolAllocator_AllocatioProxy {
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			QuadTreePoolAllocator_AllocatioProxy() = default;

			/// <summary>
			/// Move constructor
			/// </summary>
			/// <param name="other">Other element</param>
			QuadTreePoolAllocator_AllocatioProxy(QuadTreePoolAllocator_AllocatioProxy&& other) {
				// Set own
				m_ptrElement = other.m_ptrElement;
				m_ptrClearAble = other.m_ptrClearAble;

				// Clear other
				other.m_ptrElement = nullptr;
			}

			/// <summary>
			/// Destruct
			/// </summary>
			~QuadTreePoolAllocator_AllocatioProxy() noexcept {
				release();
			}

			/// <summary>
			/// Init memory function
			/// </summary>
			/// <param name="ptrElement">Pointer to element</param>
			/// <param name="ptrClear">Clearable pointer</param>
			void init(T* ptrElement, QuadTreePoolAllocator_IClearable* ptrClear) {
				// Release and set
				release();
				m_ptrElement = ptrElement;
				m_ptrClearAble = ptrClear;
			}

			/// <summary>
			/// Pointerfunction
			/// </summary>
			/// <returns>T pointer</returns>
			T* ptr() noexcept {
				return m_ptrElement;
			}

			/// <summary>
			/// Pointer operator
			/// </summary>
			/// <returns>T poiter</returns>
			T* operator->() noexcept {
				return m_ptrElement;
			}

			/// <summary>
			/// Pointer conversion
			/// </summary>
			/// <returns>T pointer</returns>
			operator T* () noexcept {
				return m_ptrElement;
			}

			/// <summary>
			/// Bool conversion operator
			/// </summary>
			/// <returns>true if element is not nullptr</returns>
			operator bool() noexcept {
				return m_ptrElement != nullptr;
			}

			/// <summary>
			/// Guarded release function
			/// </summary>
			/// <returns></returns>
			void release() noexcept {
				if (m_ptrElement) {
					// Clear and release
					m_ptrElement = nullptr;
					m_ptrClearAble->clear();
				}
			}

			/// <summary>
			/// Persist allocation
			/// </summary>
			void persist() noexcept {
				m_ptrElement = nullptr;
			}

			/// <summary>
			/// Assign move
			/// </summary>
			/// <param name="other"></param>
			void operator=(QuadTreePoolAllocator_AllocatioProxy&& other) {
				m_ptrClearAble = other.m_ptrClearAble;
				m_ptrElement = other.m_ptrElement;
				other.m_ptrElement = nullptr;
			}

			// Delete
			QuadTreePoolAllocator_AllocatioProxy(const QuadTreePoolAllocator_AllocatioProxy&) = delete;
			void operator=(const QuadTreePoolAllocator_AllocatioProxy&) = delete;
		private:
			/// <summary>
			/// Pointer to element
			/// </summary>
			T* m_ptrElement = nullptr;

			/// <summary>
			/// flag to unsed
			/// </summary>
			QuadTreePoolAllocator_IClearable* m_ptrClearAble = nullptr;
	};

	/// <summary>
	/// QuadTree accelerated pool allocator
	/// </summary>
	/// <typeparam name="T">Type of allocation element (4, 16, 64, 256, 1024, 4096, 16384, ...)</typeparam>
	/// <typeparam name="N">Size of sub elements</typeparam>
	template<typename T, unsigned int N>
	class QuadTreePoolAllocator : public QuadTreePoolAllocator_IClearable {
		static_assert(N % 4 == 0, "Memory::QuadTreePoolAllocator : N need to be power of 4 (4, 16, 64, 256, 1024, ...)");
	
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			QuadTreePoolAllocator() :
				m_ptrClearable(nullptr),
				m_clearMask(0xFF)
			{
				auto flag = m_treeFlags.get();
				*flag.ptr() = 0x00;
			}

			/// <summary>
			/// Recursion constructor
			/// </summary>
			/// <param name="ptrClear">Clearable pointer</param>
			/// <param name="mask">Clearable mask</param>
			QuadTreePoolAllocator(QuadTreePoolAllocator_IClearable* ptrClear, unsigned int mask) :
				m_ptrClearable(ptrClear),
				m_clearMask(mask)
			{};

			/// <summary>
			/// Allocare memory on the allocator
			/// </summary>
			/// <param name="refProxy">Referenced proxy</param>
			/// <returns>true if succeed</returns>
			bool allocate(Memory::QuadTreePoolAllocator_AllocatioProxy<T>& refProxy) noexcept {
				__internal__allocate(&refProxy);
				return refProxy;
			}

			/// <summary>
			/// Retrive size of allocator
			/// </summary>
			/// <returns>Size of allocator</returns>
			static constexpr unsigned int size() noexcept {
				return N;
			}

			/// <summary>
			/// Recursive clear function
			/// </summary>
			/// <param name="mask"></param>
			void clear(unsigned char mask = 0xFF) override {
				// Read old and clear
				unsigned char oldValue = *m_treeFlags.get().ptr();
				__internal__flagClear(mask);

				// Recursion
				if (oldValue == 0x0F && m_ptrClearable) {
					m_ptrClearable->clear(m_clearMask);
				}
			}

		protected:
			friend class QuadTreePoolAllocator<T, N * 4>;

			/// <summary>
			/// Internal flag check and set helper
			/// </summary>
			/// <param name="mask">Mask</param>
			/// <returns>True is succeed</returns>
			bool __internal__flagCheckAndSet(unsigned char mask) {
				MT::LockedObjectProxy<unsigned char> flag = m_treeFlags.get();
				
				// Check
				if ((*flag.ptr() & mask) == mask) {
					return false;
				}

				// Set
				*flag.ptr() = *flag.ptr() | mask;
				return true;
			}

			/// <summary>
			/// Internal flag clear
			/// </summary>
			/// <param name="mask"></param>
			void __internal__flagClear(unsigned char mask) {
				MT::LockedObjectProxy<unsigned char> flag = m_treeFlags.get();
				*flag.ptr() = ((~mask) & *flag.ptr());
			}

			/// <summary>
			/// Allocation function
			/// </summary>
			/// <param name="ptrProxy">Pointer to proxy</param>
			/// <returns>true if allocation succeeded</returns>
			bool __internal__allocate(Memory::QuadTreePoolAllocator_AllocatioProxy<T>* ptrProxy) {
				// First
				for(unsigned int i = 0; i < 4 && !(*ptrProxy); i++){
					// Aquire
					if (__internal__flagCheckAndSet(1U << i)) {
						// Allocate and check if recursive set is required
						if (m_subTreeStructure[i].__internal__allocate(ptrProxy)) {
							MT::LockedObjectProxy<unsigned char> flag = m_treeFlags.get();
							if (*flag.ptr() == 0x0F) {
								return true;
							}
							return false;
						}
						// Else unset bit
						else {
							__internal__flagClear(1U << i);
						}
					}
				}
				
				// No recusiv set required
				return false;
			}

		private:
			/// <summary>
			/// Clear pointer
			/// </summary>
			QuadTreePoolAllocator_IClearable* m_ptrClearable;

			/// <summary>
			/// Clear mask
			/// </summary>
			const unsigned int m_clearMask;

			/// <summary>
			/// Sub tree structure of size N / 4 times 4
			/// </summary>
			QuadTreePoolAllocator<T, N / 4> m_subTreeStructure[4] = {
				{this, 0b00000001}, {this, 0b00000010}, {this, 0b00000100}, {this, 0b00001000}
			};

			/// <summary>
			/// Occupation mask
			/// </summary>
			MT::ObjectGuard<unsigned char, MT::SpinLock<>> m_treeFlags;
	};

	/// <summary>
	/// QuadTree accelerated pool allocator
	/// </summary>
	/// <typeparam name="T">Type of allocation element (4, 16, 64, 256, 1024, 4096, 16384, ...)</typeparam>
	/// <typeparam name="N">Size of sub elements</typeparam>
	template<typename T>
	class QuadTreePoolAllocator<T, 1> : public QuadTreePoolAllocator_IClearable {
		public:
			QuadTreePoolAllocator(QuadTreePoolAllocator_IClearable* ptrClear, unsigned int mask) :
				m_ptrClearable(ptrClear),
				m_clearMask(mask)
			{};

			/// <summary>
			/// Recursive clear function
			/// </summary>
			/// <param name="mask"></param>
			void clear(unsigned char mask = 0xFF) override {
				// Clear own
				m_allocated.clear(std::memory_order_release);

				// Recursive start
				m_ptrClearable->clear(m_clearMask);
			}

		protected:
			friend class QuadTreePoolAllocator<T, 4>;

			/// <summary>
			/// Allocation function
			/// </summary>
			/// <param name="ptrProxy">Pointer to proxy</param>
			/// <returns>true if allocation succeeded</returns>
			bool __internal__allocate(Memory::QuadTreePoolAllocator_AllocatioProxy<T>* ptrProxy) {
				// If aquestion succeeds
				if (m_allocated.test_and_set(std::memory_order_acquire) == 0) {
					// Set flag
					ptrProxy->init(&m_element, this);
					// Return true
					return true;
				}

				// Allocation failed
				return false;
			}

		private:
			/// <summary>
			/// Clear pointer
			/// </summary>
			QuadTreePoolAllocator_IClearable* m_ptrClearable;

			/// <summary>
			/// Clear mask
			/// </summary>
			const unsigned int m_clearMask;

			/// <summary>
			/// Indicates weather the member is currently allocated
			/// </summary>
			std::atomic_flag m_allocated = ATOMIC_FLAG_INIT;

			/// <summary>
			/// Allocator element (Default constructed)
			/// </summary>
			T m_element = T();
	};
}
