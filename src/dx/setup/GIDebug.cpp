#include "DefHeader.h"
#include "GIDebug.h"

#ifdef _DEBUG

// Static instance
DX::GIDebug DX::GIDebug::s_instance;

DX::GIDebug& DX::GIDebug::getInstance() {
    // Return static instance
    return s_instance;
}

void DX::GIDebug::validate() {
    // If pointer is invalid
    if (!m_comPointer) {
        // Assert creation state
        EVALUATE_HRESULT(DXGIGetDebugInterface1(NULL, IID_PPV_ARGS(to())), "DXGIGetDebugInterface1(...) for IDXGIDebug**");
    }
}

void DX::GIDebug::reportLiveObjects() {
    // Validate object
    validate();

    // RLO Call
    EVALUATE_HRESULT(m_comPointer->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL)), "IDXGIDebug->ReportLiveObjects(...)");
}

#endif
