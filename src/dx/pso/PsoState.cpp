#include "DefHeader.h"
#include "PsoState.h"

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
