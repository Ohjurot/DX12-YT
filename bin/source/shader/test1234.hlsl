#include "input1234.hlsl"

// Shader entry point
PSData main(VSData input) {
    PSData output;
    output.position = input.position;
    output.color = input.color;
    
    return output;
}
