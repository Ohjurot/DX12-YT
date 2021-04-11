#include "demo.hlsli"
#include "rs_demo.hlsl"

/// <summary>
/// Vertex shader main function
/// </summary>
[RootSignature(ROOTSIG)]
PS_VertexData main(in VS_VertexData vertex){
    PS_VertexData vertexOut;

    vertexOut.pos = float4(vertex.pos, 0.0f, 0.0f);
    vertexOut.color = vertex.color;

    return vertexOut;
}
