#include "DefHeader.h"
#include "ScopedHandle.h"

ScopedHandle::ScopedHandle(HANDLE handle) {
	m_handle = handle;
}

ScopedHandle::ScopedHandle(ScopedHandle&& other) {
	close();
	m_handle = other.m_handle;
	other.m_handle = INVALID_HANDLE_VALUE;
}

ScopedHandle& ScopedHandle::operator=(ScopedHandle&& other) {
	close();
	m_handle = other.m_handle;
	other.m_handle = INVALID_HANDLE_VALUE;
	return *this;
}

ScopedHandle::~ScopedHandle(){
	// Close handle
	close();
}

HANDLE ScopedHandle::get() {
	return m_handle;
}

void ScopedHandle::close() {
	// If handle is in existance
	if (m_handle != INVALID_HANDLE_VALUE) {
		// Close and unset handle
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

void ScopedHandle::set(HANDLE handle){
	m_handle = handle;
}

ScopedHandle::operator bool() noexcept {
	return m_handle != INVALID_HANDLE_VALUE;
}

ScopedHandle::operator HANDLE() noexcept {
	return m_handle;
}
