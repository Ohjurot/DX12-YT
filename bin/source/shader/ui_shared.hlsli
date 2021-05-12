// Represents an UI vertex coming from the CPU and processed by the vertex shader
struct UI_VERTEX{
    // float2: Position X/Y, float2: Scale X/Y
    float4 posAndScale : SV_POSITION;

    // Reference scale to use for conversion
    uint referenceScale : REF_SCALE;
};

// Represents an UI pixel intto and from the rasterizer
struct UI_PIXEL{
    // Position of the pixel
    float4 pos : SV_POSITION;

    // UV Cords of UI Quad
    float2 quv : QUAD_UV;

    // NC Cords of UI Quad
    float2 qnc : QUAD_NC;
};

// Target pixel
struct TG_PIXEL {
    // Color
    float4 color : SV_TARGET;
};

// Root signature for UI draw calls
#define UI_ROOTSIG ""\
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
"DescriptorTable("\
"   CBV(b0, numDescriptors = 1),"\
"   SRV(t0, numDescriptors = 32, flags = DESCRIPTORS_VOLATILE)"\
", visibility = SHADER_VISIBILITY_ALL )"\

// RootSignature Layout:
// 0    : RootTable 
//      --> 32x CBV = Tranforms
//      --> 32x SRV = Textures
