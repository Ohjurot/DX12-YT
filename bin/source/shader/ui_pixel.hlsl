#include "ui_shared.hlsli"

// Textures
Texture2D<float4> textures[32] : register(t0);

// Pixel shader entry point
[RootSignature(UI_ROOTSIG)]
TG_PIXEL main(in UI_PIXEL inPixel){
    TG_PIXEL px;
    
    // TODO: Call shading functions
    px.color = float4(abs(inPixel.qnc), 1.0f, 1.0f);
    
    return px;
}
