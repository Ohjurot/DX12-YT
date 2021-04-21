#include "demo.hlsli"
#include "rs_demo.hlsl"

// RS Constant
const float4 myCFloat : register(b0);

// Texture and sampler
Texture2D<float4> texture : register(t0);
sampler texSampler : register(s0);

/// <summary>
/// Pixel shader main function
/// </summary>
[RootSignature(ROOTSIG)]
OM_PixelData main(in PS_VertexData vertexIp){
    OM_PixelData psOut;

    psOut.color = float4(texture.Sample(texSampler, vertexIp.uv).rgb * myCFloat.rgb , 1.0f);

    return psOut;
}
