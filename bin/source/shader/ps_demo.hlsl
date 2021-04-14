#include "demo.hlsli"
#include "rs_demo.hlsl"

/// <summary>
/// Pixel shader main function
/// </summary>
[RootSignature(ROOTSIG)]
OM_PixelData main(in PS_VertexData vertexIp){
    OM_PixelData psOut;

    psOut.color = float4(vertexIp.color.rgb, 1.0f);

    return psOut;
}
