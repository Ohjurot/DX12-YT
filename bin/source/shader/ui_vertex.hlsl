#include "ui_shared.hlsli"

// Vertex tranform info
struct VertexTranform{
    // Build vector (anchor)
    float4 addComponent;

    // Change scale (scaling)
    float4 mulComponent;
};

// Array of vertex tranforms
ConstantBuffer<VertexTranform> vtxTranforms[32] : register(b0);

// Vertex shader entry point
[RootSignature(UI_ROOTSIG)]
UI_VERTEX main(in UI_VERTEX inVertex){
    // Create output from input
    UI_VERTEX outVertex = inVertex;
    
    // MAD operation for vertex transform
    // Step 1: Multiply with anchor based scaling --> xy in right scale --> zw in right scale and dimension (+/-)
    // Step 2: Add to anchor point --> xy on right position --> zw not changed because addComonent.zw = 0.0f 
    outVertex.posAndScale = (inVertex.posAndScale * vtxTranforms[inVertex.referenceScale].mulComponent) + vtxTranforms[inVertex.referenceScale].addComponent; 

    return inVertex;
}
