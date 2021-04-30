#include "demo.hlsli"
#include "rs_demo.hlsl"

struct TriangleParameters{
    float2 scaling;
};

ConstantBuffer<TriangleParameters> parameters : register(b1);

/// <summary>
/// Vertex shader main function
/// </summary>
[RootSignature(ROOTSIG)]
PS_VertexData main(in VS_VertexData vertex){
    PS_VertexData vertexOut;

    vertexOut.pos = float4(vertex.pos * parameters.scaling, 0.0f, 1.0f);
    vertexOut.uv = vertex.uv;

    return vertexOut;
}
