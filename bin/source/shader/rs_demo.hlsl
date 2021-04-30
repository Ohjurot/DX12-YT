/*  --- Root Signature ---
 *  
 *  Flags:
 *  Content:
 *  0: float4 constant
 *  1: RootTable
 *  X: Static sampler
 */

#define ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
"RootConstants(num32BitConstants=4, b0),"\
"CBV(b1, visibility = SHADER_VISIBILITY_VERTEX),"\
"DescriptorTable("\
"   SRV(t0, numDescriptors = 1)"\
", visibility = SHADER_VISIBILITY_PIXEL),"\
"StaticSampler(s0, filter=FILTER_ANISOTROPIC)"
