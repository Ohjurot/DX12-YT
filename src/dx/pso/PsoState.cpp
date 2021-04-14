#include "DefHeader.h"
#include "PsoState.h"

dx::PsoState::PsoState(PsoState&& other) noexcept {
    // Copy com pointer
    m_ptrPso = other.m_ptrPso;
    m_ptrRootSignature = other.m_ptrRootSignature;

    // Move shader
    p_vertexShader = std::move(other.p_vertexShader);
    p_hullShader = std::move(other.p_hullShader);
    p_domainShader = std::move(other.p_domainShader);
    p_geometryShader = std::move(other.p_geometryShader);
    p_pixelShader = std::move(other.p_pixelShader);

    // Move string allocator
    p_strAllocator = std::move(other.p_strAllocator);

    // Copy descriptors
    memcpy(&p_psoDescC, &other.p_psoDescC, sizeof(PsoDescContainer) * 16);
    memcpy(&p_soDescs, &other.p_soDescs, sizeof(D3D12_SO_DECLARATION_ENTRY) * 16);
    memcpy(&p_iaLayoutDescs, &other.p_iaLayoutDescs, sizeof(D3D12_INPUT_ELEMENT_DESC) * 16);
    memcpy(&p_soStrides, &other.p_soStrides, sizeof(UINT) * 16);

    // Valid
    m_bCompileDataValid = other.m_bCompileDataValid;
    other.m_bCompileDataValid = false;
}

dx::PsoState::~PsoState() {
    release();
}

void dx::PsoState::release() {
    m_ptrRootSignature.release();
    m_ptrPso.release();
}

void dx::PsoState::bind(ID3D12GraphicsCommandList* ptrCommandList) {
    EXPP_ASSERT_DBG(ptrCommandList, "Invalid command list pointer");
    EXPP_ASSERT(m_bCompileDataValid, "Invalid PSO Object");

    // Swtich type
    switch (ptrCommandList->GetType()) {
        // GFX
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
        case D3D12_COMMAND_LIST_TYPE_BUNDLE:
            EXPP_ASSERT(!p_psoDescC.isCompute, "Binding a compute PSO on a gfx command list is not supported!");
            ptrCommandList->SetGraphicsRootSignature(m_ptrRootSignature);
            break;
        
        // Compute
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            EXPP_ASSERT(!p_psoDescC.isCompute, "Binding a gfx PSO on a compute command list is not supported!");
            ptrCommandList->SetComputeRootSignature(m_ptrRootSignature);
            break;

        // 
        default:
            EXPP_ASSERT(false, "Unsupported command list");
            break;
    }

    // Bind PSO and RootSignature
    ptrCommandList->SetPipelineState(m_ptrPso);
}

bool dx::PsoState::compile(ID3D12Device* ptrDevice) {
    // Check
    if (!m_bCompileDataValid) {
        return false;
    }

    // Release old
    m_ptrPso.release();
    m_ptrRootSignature.release();

    // Compile new
    if (p_psoDescC.isCompute) {
        // TODO Compute
        return false;
    }
    else {
        // Get vertex shader
        void* ptrRootBlob = nullptr;
        size_t rootSize = 0;
        EXPP_ASSERT(p_vertexShader.setBuffer(&ptrRootBlob, &rootSize), "Vertex shader is required but not loaded!");

        // Create root signature
        EVALUATE_HRESULT(ptrDevice->CreateRootSignature(NULL, ptrRootBlob, rootSize, IID_PPV_ARGS(m_ptrRootSignature.to())), "ID3D12Device->CreateRootSignature(...) from vertex shader");

        // Compile gfx
        p_psoDescC.gfxDesc.pRootSignature = m_ptrRootSignature;
        EVALUATE_HRESULT(ptrDevice->CreateGraphicsPipelineState(&p_psoDescC.gfxDesc, IID_PPV_ARGS(m_ptrPso.to())), "ID3D12Device->CreateGraphicsPipelineState(...)");
    }

    // Pass
    return true;
}

dx::PsoState::operator bool() noexcept {
    return m_bCompileDataValid;
}
