/// <summary>
/// Vertex shader input
/// </summary>
struct VS_VertexData { 
    float2 pos : POSITION;
    float3 color : COLOR;
};

/// <summary>
/// Pixel shader input
/// </summary>
struct PS_VertexData {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

/// <summary>
/// Pixel output
/// </summary>
struct OM_PixelData {
    float4 color : SV_TARGET; 
};
