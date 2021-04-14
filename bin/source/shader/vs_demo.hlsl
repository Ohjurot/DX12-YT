#include "demo.hlsli"
#include "rs_demo.hlsl"

// RS Constant
const float4 myCFloat : register(b0);

/// <summary>
/// Vertex shader main function
/// </summary>
[RootSignature(ROOTSIG)]
PS_VertexData main(in VS_VertexData vertex){
    PS_VertexData vertexOut;

    vertexOut.pos = float4(vertex.pos, 0.0f, 0.0f);
    vertexOut.color = float4(vertex.color, 1.0f) * myCFloat;

    return vertexOut;
}
