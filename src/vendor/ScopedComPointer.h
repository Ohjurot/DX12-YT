#pragma once

#include <string>
#include <sstream>

template <typename T>
inline const wchar_t* typeNameW();

/// <summary>
/// Scoped com pointer
/// </summary>
/// <typeparam name="T">Type of pointer</typeparam>
template<typename T>
class ScopedComPointer {
	public:
		/// <summary>
		/// Create compointer
		/// </summary>
		ScopedComPointer() = default;

		/// <summary>
		/// Create compointer
		/// </summary>
		/// <param name="comPointer">Source pointer</param>
		ScopedComPointer(T* comPointer) {
			if (comPointer) {
				m_comPointer = comPointer;
				m_comPointer->AddRef();
			}
		}

		/// <summary>
		/// Copy constructor
		/// </summary>
		/// <param name="refPointer">Ref to other pointer</param>
		ScopedComPointer(const ScopedComPointer<T>& refPointer) {
			// If other pointer is valid
			if (refPointer.m_comPointer) {
				// Copy and add ref
				m_comPointer = refPointer.m_comPointer;
				m_comPointer->AddRef();
			}
		}

		// Destruct
		~ScopedComPointer() {
			release();
		}

		/// <summary>
		/// Query interface into other ScopedComPointer
		/// </summary>
		/// <typeparam name="TX">Type of requested interface</typeparam>
		/// <param name="refPointer">Reference to target COM pointer</param>
		/// <returns>True if query succeeded</returns>
		template<typename TX>
		bool queryInterface(ScopedComPointer<TX>& refPointer) {
			// Check pointer and try query
			if (!m_comPointer || FAILED(m_comPointer->QueryInterface(IID_PPV_ARGS(refPointer.to())))) {
				// Return false on fail
				return false;
			}
			
			// Return true on succeed
			return true;
		}

		/// <summary>
		/// Retrive pointer
		/// </summary>
		/// <returns>Pointer to T</returns>
		T* ptr() {
			return m_comPointer;
		}

		/// <summary>
		/// Retrive pointer to inernal pointer 
		/// </summary>
		/// <returns>Pointer to inernal com pointer</returns>
		T** to() {
			return &m_comPointer;
		}

		/// <summary>
		/// Releases the com pointer
		/// </summary>
		virtual void release() {
			if (m_comPointer) {
				m_comPointer->Release();
				m_comPointer = nullptr;
			}
		}

		/// <summary>
		/// Name the com pointer
		/// </summary>
		/// <param name="name">Input name</param>
		void name(LPCWSTR name) {
			// Early return
			if (!m_comPointer) {
				return;
			}

			// Build name
			std::wstringstream wss;
			wss << typeNameW<T>() << L" (" << name << L")";

			// Set name
			m_comPointer->SetName(wss.str().c_str());
		}

		/// <summary>
		/// Operator for accessing the pointer
		/// </summary>
		/// <returns></returns>
		T* operator->() {
			return m_comPointer;
		}

		/// <summary>
		/// Compare with null
		/// </summary>
		/// <returns>Is not nullptr</returns>
		operator bool() {
			return (m_comPointer != nullptr);
		}

		/// <summary>
		/// Converter to T*
		/// </summary>
		operator T*() {
			return m_comPointer;
		}

		/// <summary>
		/// Copy operator
		/// </summary>
		/// <param name="other"></param>
		void operator=(ScopedComPointer& other) {
			if (other.m_comPointer) {
				// Set pointer
				m_comPointer = other.m_comPointer;
				// Increment ref
				m_comPointer->AddRef();
			}
		}

		/// <summary>
		/// Assign for pointer
		/// </summary>
		/// <param name="comPtr">Other com pointer</param>
		void operator=(T* comPtr) {
			// Release old
			if (m_comPointer) {
				m_comPointer->Release();
			}

			// Setup new
			m_comPointer = comPtr;

			// Increment ref if not null
			if (m_comPointer) {
				m_comPointer->AddRef();
			}
		}

	protected:
		/// <summary>
		/// Retrive com pointer
		/// </summary>
		T* m_comPointer = nullptr;
};
