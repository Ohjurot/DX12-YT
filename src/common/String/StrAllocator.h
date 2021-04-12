#pragma once

#include <DefHeader.h>

namespace common {
	namespace String {
		/// <summary>
		/// String allocator
		/// </summary>
		/// <typeparam name="T">Char type</typeparam>
		template<typename T = WCHAR, size_t S = 0>
		class Allocator {
			public:
				/// <summary>
				/// Allocator of size C
				/// </summary>
				template<typename = std::enable_if_t<S != 0>>
				Allocator() :
					m_size(S)
				{
					// Allocate
					m_ptrBuffer = (T*)malloc(S * sizeof(T));
					EXPP_ASSERT(m_ptrBuffer, "Memory allocator failed!");
				}

				/// <summary>
				/// Allocator of size size
				/// </summary>
				/// <param name="size">size</param>
				template<typename = std::enable_if_t<S == 0>>
				Allocator(size_t size) :
					m_size(size)
				{
					// Allocate
					m_ptrBuffer = (T*)malloc(size * sizeof(T));
					EXPP_ASSERT(m_ptrBuffer, "Memory allocator failed!");
				}

				/// <summary>
				/// Move constructor
				/// </summary>
				/// <param name="other">other</param>
				Allocator(Allocator&& other) {
					// Set own
					m_ptrBuffer = other.m_ptrBuffer;
					m_size = other.m_size;
					m_head = other.m_head;

					// Clear other
					other.m_ptrBuffer = nullptr;
				};

				// Delete
				Allocator(const Allocator&) = delete;

				/// <summary>
				/// Destructor
				/// </summary>
				~Allocator() {
					// Free memory
					if (m_ptrBuffer) {
						free(m_ptrBuffer);
					}
				}

				// Delete
				Allocator& operator=(const Allocator&) = delete;

				/// <summary>
				/// Move assign 
				/// </summary>
				/// <param name="other">Other</param>
				/// <returns>ref</returns>
				Allocator& operator=(Allocator&& other) {
					EXPP_ASSERT(other.m_size == m_size, "Size missmatch!");
					
					// Clear own
					if (m_ptrBuffer) {
						free(m_ptrBuffer);
					}

					// Set own
					m_ptrBuffer = other.m_ptrBuffer;
					m_head = other.m_head;

					// Clear other
					other.m_ptrBuffer = nullptr;

					return *this;
				}

				/// <summary>
				/// Reset the allocator
				/// </summary>
				void reset() noexcept {
					m_head = 0;
				}

				/// <summary>
				/// Allocate for string
				/// </summary>
				/// <param name="stringLength">Length of string (without null terminator)</param>
				/// <returns>Pointer to allocated string</returns>
				T* strAlloc(size_t stringLength) {
					if ((stringLength + 1) <= (m_size - m_head)) {
						// Store pointer and increment head
						T* allocPtr = &m_ptrBuffer[m_head];
						m_head += stringLength + 1;

						// Set null terminator and return
						allocPtr[stringLength] = 0x00;
						return allocPtr;
					}

					// Allocation failed
					return nullptr;
				}

				/// <summary>
				/// Copy allocate string
				/// </summary>
				/// <param name="inputString">Input string</param>
				/// <returns>pointer to new string</returns>
				template<typename = std::enable_if_t<std::is_same_v<T, WCHAR>>>
				WCHAR* strAllocCopy(const WCHAR* inputString) {
					// Allocate string
					WCHAR* strPtr = strAlloc(wcslen(inputString));
					if (strPtr) {
						// Copy 
						wcscpy_s(strPtr, wcslen(inputString) + 1, inputString);
					}

					// Return pointer
					return strPtr;
				}

				/// <summary>
				/// Copy allocate string
				/// </summary>
				/// <param name="inputString">Input string</param>
				/// <returns>pointer to new string</returns>
				template<typename = std::enable_if_t<std::is_same_v<T, CHAR>>>
				CHAR* strAllocCopy(const CHAR* inputString) {
					// Allocate string
					CHAR* strPtr = strAlloc(strlen(inputString));
					if (strPtr) {
						// Copy 
						strcpy_s(strPtr, strlen(inputString) + 1, inputString);
					}

					// Return pointer
					return strPtr;
				}

			private:
				/// <summary>
				/// Buffer pointer
				/// </summary>
				T* m_ptrBuffer;

				/// <summary>
				/// Write head
				/// </summary>
				size_t m_head = 0;

				/// <summary>
				/// Size
				/// </summary>
				const size_t m_size;
		};
	}
}
