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

    vertexOut.pos = float4(vertex.pos, 0.0f, 1.0f);
    vertexOut.color = vertex.color * myCFloat.rgb;

    return vertexOut;
}
