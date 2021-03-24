#pragma once

/// <summary>
/// Scoped pointer
/// </summary>
/// <typeparam name="T">Type of pointer</typeparam>
template<typename T>
class ScopedPointer {
	public:
		/// <summary>
		/// Create pointer
		/// </summary>
		ScopedPointer() {
			m_pointer = nullptr;
		}

		/// <summary>
		/// Construct from exiting pointer
		/// </summary>
		/// <param name="ptr">Existing pointer</param>
		ScopedPointer(T* ptr) {
			m_pointer = ptr;
		}

		// Destruct
		~ScopedPointer() {
			deletePointer();
		}
		
		/// <summary>
		/// Deletes the pointer
		/// </summary>
		void deletePointer() {
			// Check if pointer is in existance
			if (m_pointer) {
				// Delete and unset pointer
				delete m_pointer;
				m_pointer = nullptr;
			}
		}

		/// <summary>
		/// Set the pointer
		/// </summary>
		/// <param name="pointer">New pointer</param>
		void set(T* pointer){
			// Delete if exists and differ
			if (m_pointer && pointer != m_pointer) {
				deletePointer();
			}

			// Assign pointer
			m_pointer = pointer;
		}

		/// <summary>
		/// Retrive pointer
		/// </summary>
		/// <returns>Pointer to T</returns>
		T* ptr() {
			return m_pointer;
		}

		/// <summary>
		/// Retrive pointer to inernal pointer 
		/// </summary>
		/// <returns>Pointer to inernal pointer</returns>
		T** to() {
			return &m_pointer;
		}

		/// <summary>
		/// Operator for accessing the pointer
		/// </summary>
		/// <returns>Pointer</returns>
		T* operator->() {
			return m_pointer;
		}

		/// <summary>
		/// Compare with null
		/// </summary>
		/// <returns>Is not nullptr</returns>
		operator bool() {
			return (m_pointer != nullptr);
		}

		/// <summary>
		/// Converter to T*
		/// </summary>
		operator T* () {
			return m_pointer;
		}

		// Delete copy and override
		ScopedPointer(const ScopedPointer&) = delete;
		void operator=(const ScopedPointer&) = delete;
	private:
		/// <summary>
		/// Retrive com pointer
		/// </summary>
		T* m_pointer;
};