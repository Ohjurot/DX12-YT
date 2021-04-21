/// <summary>
/// Vertex shader input
/// </summary>
struct VS_VertexData { 
    float2 pos : POSITION;
    float2 uv : TEXTCORDS;
};

/// <summary>
/// Pixel shader input
/// </summary>
struct PS_VertexData {
    float4 pos : SV_POSITION;
    float2 uv : TEXTCORDS;
};

/// <summary>
/// Pixel output
/// </summary>
struct OM_PixelData {
    float4 color : SV_TARGET; 
};
