#include "DefHeader.h"
#include "XDebug.h"


// Instance of debug interface
DX::XDebug DX::XDebug::s_instance;

DX::XDebug& DX::XDebug::getInstance() {
    // Return instance
    return s_instance;
}

void DX::XDebug::validate(){
    // If pointer is invalid
    if (!m_comPointer) {
        // Assert creation state
        EVALUATE_HRESULT(D3D12GetDebugInterface(IID_PPV_ARGS(to())), "D3D12GetDebugInterface(...)");
    }
}

void DX::XDebug::enable(){
    // Validate object
    validate();

    // Enable interface
    m_comPointer->EnableDebugLayer();
    m_bIsEnabled = true;
}

bool DX::XDebug::isEnabled() noexcept {
    return m_bIsEnabled;
}

void DX::XDebug::setGpuBasedValidation(bool enabled, D3D12_GPU_BASED_VALIDATION_FLAGS flags){
    // Get ID3D12Debug3
    ScopedComPointer<ID3D12Debug3> ptrDebug3;
    if (!queryInterface(ptrDebug3)) {
        throw EXPP_EXCEPTION(std::exception, std::exception("ID3D12Debug3 not supported"), EXPP_TEXT("ScopedComPointer<ID3D12Debug>::queryInterface<ID3D12Debug3>(...)"));
    }

    // Set enable
    ptrDebug3->SetEnableGPUBasedValidation(enabled);

    // Set flag
    ptrDebug3->SetGPUBasedValidationFlags(flags);
}

void DX::XDebug::setEnableSyncCommandQueueValidation(bool enabled) {
    // Get ID3D12Debug3
    ScopedComPointer<ID3D12Debug3> ptrDebug3;
    if (!queryInterface(ptrDebug3)) {
        throw EXPP_EXCEPTION(std::exception, std::exception("ID3D12Debug3 not supported"), EXPP_TEXT("ScopedComPointer<ID3D12Debug>::queryInterface<ID3D12Debug3>(...)"));
    }

    // Set enabled
    ptrDebug3->SetEnableSynchronizedCommandQueueValidation(enabled);
}
