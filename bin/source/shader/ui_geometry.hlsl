#include "ui_shared.hlsli"

// Vertex shader entry point
[RootSignature(UI_ROOTSIG)]
[maxvertexcount(4)]
void main(in point UI_VERTEX inVertex[1], inout TriangleStream<UI_PIXEL> outStream){
    // Create output vertices
    UI_PIXEL vtx0, vtx1, vtx2, vtx3;

    // VTX0
    vtx0.pos = float4(
        inVertex[0].posAndScale.x,
        inVertex[0].posAndScale.y,
        0.0f, 1.0f
    );
    vtx0.quv = float2(0.0f, 0.0f);
    vtx0.qnc = float2(-1.0, 1.0);
    outStream.Append(vtx0);

    // VTX1
    vtx1.pos = float4(
        inVertex[0].posAndScale.x + inVertex[0].posAndScale.z, 
        inVertex[0].posAndScale.y,
        0.0f, 1.0f
    );
    vtx1.quv = float2(1.0f, 0.0f);
    vtx1.qnc = float2(1.0, 1.0);
    outStream.Append(vtx1);

    // VTX2
    vtx2.pos = float4(
        inVertex[0].posAndScale.x, 
        inVertex[0].posAndScale.y + inVertex[0].posAndScale.w,
        0.0f, 1.0f
    );
    vtx2.quv = float2(0.0f, 1.0f);
    vtx2.qnc = float2(-1.0, -1.0);
    outStream.Append(vtx2);

    // VTX3
    vtx3.pos = float4(
        inVertex[0].posAndScale.x + inVertex[0].posAndScale.z,
        inVertex[0].posAndScale.y + inVertex[0].posAndScale.w,
        0.0f, 1.0f
    );
    vtx3.quv = float2(1.0f, 1.0f);
    vtx3.qnc = float2(1.0, -1.0);
    outStream.Append(vtx3);

    // Finish strip
    outStream.RestartStrip();
}
