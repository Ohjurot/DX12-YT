#include "DefHeader.h"
#include "LocalStorageProvider.h"

// Implement instance
Threading::LocalStorageProvider Threading::LocalStorageProvider::s_instance;

void* Threading::LocalStorageProvider::getTLSData() {
    return TlsGetValue(m_dwTlsCookie);
}

void Threading::LocalStorageProvider::setTLSData(void* data) {
    EXPP_ASSERT(m_dwTlsCookie != TLS_OUT_OF_INDEXES, "TLS Allocate failed!");

    TlsSetValue(m_dwTlsCookie, data);
}

void* Threading::LocalStorageProvider::getFLSData() {
    return FlsGetValue(m_dwFlsCookie);
}

void Threading::LocalStorageProvider::setFLSData(void* data) {
    EXPP_ASSERT(m_dwFlsCookie != FLS_OUT_OF_INDEXES, "TLS Allocate failed!");

    FlsSetValue(m_dwFlsCookie, data);
}

Threading::LocalStorageProvider& Threading::LocalStorageProvider::getInstance() {
    // Return static instance
    return s_instance;
}

Threading::LocalStorageProvider::LocalStorageProvider() {
    // Allocate TLS
    m_dwTlsCookie = TlsAlloc();

    // Allocate FLS
    m_dwFlsCookie = FlsAlloc(NULL);
}

Threading::LocalStorageProvider::~LocalStorageProvider() {
    // Free FLS
    FlsFree(m_dwFlsCookie);

    // Free TLS
    TlsFree(m_dwTlsCookie);
}
