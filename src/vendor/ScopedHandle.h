#pragma once
#include <WinInclude.h>

/// <summary>
/// Scoped handle
/// </summary>
class ScopedHandle {
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		ScopedHandle() = default;

		/// <summary>
		/// Create scoped handle from handle
		/// </summary>
		/// <param name="handle">Input handle</param>
		ScopedHandle(HANDLE handle);

		/// <summary>
		/// Assign construct
		/// </summary>
		/// <param name=""></param>
		ScopedHandle(ScopedHandle&& other);

		/// <summary>
		/// Assign operator
		/// </summary>
		/// <param name=""></param>
		ScopedHandle& operator=(ScopedHandle&& other);

		// Destructor
		~ScopedHandle();

		/// <summary>
		/// Retrive handle
		/// </summary>
		/// <returns>Handle</returns>
		HANDLE get();

		/// <summary>
		/// Close handle
		/// </summary>
		void close();

		/// <summary>
		/// Set function
		/// </summary>
		/// <param name="handle">Input handle</param>
		void set(HANDLE handle);

		/// <summary>
		/// Bool operator
		/// </summary>
		operator bool() noexcept;

		/// <summary>
		/// Handle conversion
		/// </summary>
		operator HANDLE() noexcept;
	private:
		/// <summary>
		/// Handle
		/// </summary>
		HANDLE m_handle = INVALID_HANDLE_VALUE;
};