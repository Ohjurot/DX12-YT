#include "DefHeader.h"
#include "gistr.h"

DXGI_FORMAT dx::strconverter::dx12_textToDiFormat(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0x2d69cd4499718fd2) {
        return DXGI_FORMAT_UNKNOWN;
    }
    else if (stringHash == 0x381dd670c4806bd7) {
        return DXGI_FORMAT_R32G32B32A32_TYPELESS;
    }
    else if (stringHash == 0x9606ea0e6ecb2cee) {
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    else if (stringHash == 0x7a99288991ab2092) {
        return DXGI_FORMAT_R32G32B32A32_UINT;
    }
    else if (stringHash == 0x83e48ac98083b167) {
        return DXGI_FORMAT_R32G32B32A32_SINT;
    }
    else if (stringHash == 0xab0b44b4a714c991) {
        return DXGI_FORMAT_R32G32B32_TYPELESS;
    }
    else if (stringHash == 0x9d83930c3049e22a) {
        return DXGI_FORMAT_R32G32B32_FLOAT;
    }
    else if (stringHash == 0xfa28ee9e1d05d39a) {
        return DXGI_FORMAT_R32G32B32_UINT;
    }
    else if (stringHash == 0x6626e11e7bd2a89e) {
        return DXGI_FORMAT_R32G32B32_SINT;
    }
    else if (stringHash == 0x166bcf3597d4e488) {
        return DXGI_FORMAT_R16G16B16A16_TYPELESS;
    }
    else if (stringHash == 0xba85f25eb1a96f18) {
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    }
    else if (stringHash == 0x37dc8fc30d1e4e5f) {
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    }
    else if (stringHash == 0x6416f3327e4274c5) {
        return DXGI_FORMAT_R16G16B16A16_UINT;
    }
    else if (stringHash == 0x0135d4a331ccaa25) {
        return DXGI_FORMAT_R16G16B16A16_SNORM;
    }
    else if (stringHash == 0xf4aa59cd3ad8ffb3) {
        return DXGI_FORMAT_R16G16B16A16_SINT;
    }
    else if (stringHash == 0x1b18624f4a5e9bf2) {
        return DXGI_FORMAT_R32G32_TYPELESS;
    }
    else if (stringHash == 0x2ab5e0681c86996e) {
        return DXGI_FORMAT_R32G32_FLOAT;
    }
    else if (stringHash == 0x9b147c1b9cc9c89e) {
        return DXGI_FORMAT_R32G32_UINT;
    }
    else if (stringHash == 0x9faadd81180dd7a4) {
        return DXGI_FORMAT_R32G32_SINT;
    }
    else if (stringHash == 0x5cbf04102223ad0d) {
        return DXGI_FORMAT_R32G8X24_TYPELESS;
    }
    else if (stringHash == 0x6c469af8bf8246e0) {
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    }
    else if (stringHash == 0xce44a3e4a03fcc69) {
        return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    }
    else if (stringHash == 0x3e18eaa160c48940) {
        return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
    }
    else if (stringHash == 0x6f7820e4fe78a283) {
        return DXGI_FORMAT_R10G10B10A2_TYPELESS;
    }
    else if (stringHash == 0x4e5780c82528f1f1) {
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    }
    else if (stringHash == 0xc6add4f8305fd8e3) {
        return DXGI_FORMAT_R10G10B10A2_UINT;
    }
    else if (stringHash == 0xfbbfa0e2cb7254a0) {
        return DXGI_FORMAT_R11G11B10_FLOAT;
    }
    else if (stringHash == 0xe3c1e13b6efab519) {
        return DXGI_FORMAT_R8G8B8A8_TYPELESS;
    }
    else if (stringHash == 0x3f1b8e471f79cdaf) {
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    else if (stringHash == 0xb39c572890ff5bee) {
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    }
    else if (stringHash == 0x0af0b413c5ba3569) {
        return DXGI_FORMAT_R8G8B8A8_UINT;
    }
    else if (stringHash == 0x88c69d8862c63fab) {
        return DXGI_FORMAT_R8G8B8A8_SNORM;
    }
    else if (stringHash == 0xaf68e4ad2111735d) {
        return DXGI_FORMAT_R8G8B8A8_SINT;
    }
    else if (stringHash == 0x941ee2342cb7fce9) {
        return DXGI_FORMAT_R16G16_TYPELESS;
    }
    else if (stringHash == 0xa9e5d45ecc9f5a8c) {
        return DXGI_FORMAT_R16G16_FLOAT;
    }
    else if (stringHash == 0xfde01a9492c0db36) {
        return DXGI_FORMAT_R16G16_UNORM;
    }
    else if (stringHash == 0x9e1feb9b915bf262) {
        return DXGI_FORMAT_R16G16_UINT;
    }
    else if (stringHash == 0x2c2e701afbb9607f) {
        return DXGI_FORMAT_R16G16_SNORM;
    }
    else if (stringHash == 0xc0391a1778a13544) {
        return DXGI_FORMAT_R16G16_SINT;
    }
    else if (stringHash == 0x17e3c1704d3c86f3) {
        return DXGI_FORMAT_R32_TYPELESS;
    }
    else if (stringHash == 0xb4fe802dbf50fd4b) {
        return DXGI_FORMAT_D32_FLOAT;
    }
    else if (stringHash == 0x3cec52e3471efab2) {
        return DXGI_FORMAT_R32_FLOAT;
    }
    else if (stringHash == 0xda2c7bc98fa1c471) {
        return DXGI_FORMAT_R32_UINT;
    }
    else if (stringHash == 0xfe31ba3d4804b36a) {
        return DXGI_FORMAT_R32_SINT;
    }
    else if (stringHash == 0xa360fb62cedfdc4d) {
        return DXGI_FORMAT_R24G8_TYPELESS;
    }
    else if (stringHash == 0xdd8c8cc81a49bae8) {
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    }
    else if (stringHash == 0x3cf47d1b6d6f1059) {
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    }
    else if (stringHash == 0xa1bb9f421b9de1f3) {
        return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
    }
    else if (stringHash == 0x024680b96a682fbe) {
        return DXGI_FORMAT_R8G8_TYPELESS;
    }
    else if (stringHash == 0xe6789879dd880f39) {
        return DXGI_FORMAT_R8G8_UNORM;
    }
    else if (stringHash == 0x5c3805c42069c14d) {
        return DXGI_FORMAT_R8G8_UINT;
    }
    else if (stringHash == 0x4f169542ab9e492a) {
        return DXGI_FORMAT_R8G8_SNORM;
    }
    else if (stringHash == 0xa9d2660ec3296768) {
        return DXGI_FORMAT_R8G8_SINT;
    }
    else if (stringHash == 0x0a0734d21fd64700) {
        return DXGI_FORMAT_R16_TYPELESS;
    }
    else if (stringHash == 0xd0497102e02d332d) {
        return DXGI_FORMAT_R16_FLOAT;
    }
    else if (stringHash == 0xa2558580280f1d53) {
        return DXGI_FORMAT_D16_UNORM;
    }
    else if (stringHash == 0x291d33c0ce1fd605) {
        return DXGI_FORMAT_R16_UNORM;
    }
    else if (stringHash == 0x739c880663fe0bcd) {
        return DXGI_FORMAT_R16_UINT;
    }
    else if (stringHash == 0x7ae87e356434f7de) {
        return DXGI_FORMAT_R16_SNORM;
    }
    else if (stringHash == 0x564142f0358ae2c6) {
        return DXGI_FORMAT_R16_SINT;
    }
    else if (stringHash == 0xc025d487025aa48b) {
        return DXGI_FORMAT_R8_TYPELESS;
    }
    else if (stringHash == 0x0c5188024997dcf5) {
        return DXGI_FORMAT_R8_UNORM;
    }
    else if (stringHash == 0xaf7d1e34f10210a2) {
        return DXGI_FORMAT_R8_UINT;
    }
    else if (stringHash == 0x5ab0bbac460c2fc7) {
        return DXGI_FORMAT_R8_SNORM;
    }
    else if (stringHash == 0xd14594e4445f4ce9) {
        return DXGI_FORMAT_R8_SINT;
    }
    else if (stringHash == 0x147464af9bc17888) {
        return DXGI_FORMAT_A8_UNORM;
    }
    else if (stringHash == 0x196465fa1703c991) {
        return DXGI_FORMAT_R1_UNORM;
    }
    else if (stringHash == 0xdfda63fc7c0fb71a) {
        return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
    }
    else if (stringHash == 0x6e5512103f194759) {
        return DXGI_FORMAT_R8G8_B8G8_UNORM;
    }
    else if (stringHash == 0x233120b7381b25b9) {
        return DXGI_FORMAT_G8R8_G8B8_UNORM;
    }
    else if (stringHash == 0x5bbb4c8f0257de5e) {
        return DXGI_FORMAT_BC1_TYPELESS;
    }
    else if (stringHash == 0x74479dbd8c0abffa) {
        return DXGI_FORMAT_BC1_UNORM;
    }
    else if (stringHash == 0xba533c11f2c7da02) {
        return DXGI_FORMAT_BC1_UNORM_SRGB;
    }
    else if (stringHash == 0x6a2118c39ae670be) {
        return DXGI_FORMAT_BC2_TYPELESS;
    }
    else if (stringHash == 0x29726a606ee5e326) {
        return DXGI_FORMAT_BC2_UNORM;
    }
    else if (stringHash == 0x5cab3742c3ac41b4) {
        return DXGI_FORMAT_BC2_UNORM_SRGB;
    }
    else if (stringHash == 0x304508a95922e555) {
        return DXGI_FORMAT_BC3_TYPELESS;
    }
    else if (stringHash == 0xf0492cb6edc2b82d) {
        return DXGI_FORMAT_BC3_UNORM;
    }
    else if (stringHash == 0x48f519f597ee7ea2) {
        return DXGI_FORMAT_BC3_UNORM_SRGB;
    }
    else if (stringHash == 0xb43463c94c1abc66) {
        return DXGI_FORMAT_BC4_TYPELESS;
    }
    else if (stringHash == 0xe897c341e985d65b) {
        return DXGI_FORMAT_BC4_UNORM;
    }
    else if (stringHash == 0xba4a11fff4f6c17a) {
        return DXGI_FORMAT_BC4_SNORM;
    }
    else if (stringHash == 0xd7ec7b443a21109d) {
        return DXGI_FORMAT_BC5_TYPELESS;
    }
    else if (stringHash == 0x7530808ed2342630) {
        return DXGI_FORMAT_BC5_UNORM;
    }
    else if (stringHash == 0x805344032ff1fbad) {
        return DXGI_FORMAT_BC5_SNORM;
    }
    else if (stringHash == 0x8797771626bca76c) {
        return DXGI_FORMAT_B5G6R5_UNORM;
    }
    else if (stringHash == 0x06d121e54ae29489) {
        return DXGI_FORMAT_B5G5R5A1_UNORM;
    }
    else if (stringHash == 0x3321824d61958141) {
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    }
    else if (stringHash == 0x062123f730207a78) {
        return DXGI_FORMAT_B8G8R8X8_UNORM;
    }
    else if (stringHash == 0xe4fad24b58e5369e) {
        return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
    }
    else if (stringHash == 0xeb52a44a78a914f9) {
        return DXGI_FORMAT_B8G8R8A8_TYPELESS;
    }
    else if (stringHash == 0x56efbfa839b284a1) {
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    }
    else if (stringHash == 0xd2a08f52cdf21494) {
        return DXGI_FORMAT_B8G8R8X8_TYPELESS;
    }
    else if (stringHash == 0x57dca6beeb55a2ea) {
        return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
    }
    else if (stringHash == 0xba12014253b1194a) {
        return DXGI_FORMAT_BC6H_TYPELESS;
    }
    else if (stringHash == 0x6453a08832737d6b) {
        return DXGI_FORMAT_BC6H_UF16;
    }
    else if (stringHash == 0x8e7ab0bb69e57239) {
        return DXGI_FORMAT_BC6H_SF16;
    }
    else if (stringHash == 0x69f5652450a35836) {
        return DXGI_FORMAT_BC7_TYPELESS;
    }
    else if (stringHash == 0x44ad1b338583f240) {
        return DXGI_FORMAT_BC7_UNORM;
    }
    else if (stringHash == 0xdca094272709acf9) {
        return DXGI_FORMAT_BC7_UNORM_SRGB;
    }
    else if (stringHash == 0x4f3adef0d09fd83c) {
        return DXGI_FORMAT_AYUV;
    }
    else if (stringHash == 0x9ece666b1340a199) {
        return DXGI_FORMAT_Y410;
    }
    else if (stringHash == 0x2d72bc2830b52b40) {
        return DXGI_FORMAT_Y416;
    }
    else if (stringHash == 0x73c18f4b2e786e6d) {
        return DXGI_FORMAT_NV12;
    }
    else if (stringHash == 0xef6ed18632764d06) {
        return DXGI_FORMAT_P010;
    }
    else if (stringHash == 0xbc6e1106fedf430c) {
        return DXGI_FORMAT_P016;
    }
    else if (stringHash == 0x9332d80f49ae078b) {
        return DXGI_FORMAT_420_OPAQUE;
    }
    else if (stringHash == 0x9d49cf2e5ddc6d68) {
        return DXGI_FORMAT_YUY2;
    }
    else if (stringHash == 0x4e057be23ef28a74) {
        return DXGI_FORMAT_Y210;
    }
    else if (stringHash == 0x8e18f19f4a6ade67) {
        return DXGI_FORMAT_Y216;
    }
    else if (stringHash == 0x1573e9c674df4343) {
        return DXGI_FORMAT_NV11;
    }
    else if (stringHash == 0x76847e47a17c922e) {
        return DXGI_FORMAT_AI44;
    }
    else if (stringHash == 0x8ed8f5b747d404ee) {
        return DXGI_FORMAT_IA44;
    }
    else if (stringHash == 0xae9110f51e79a9fc) {
        return DXGI_FORMAT_P8;
    }
    else if (stringHash == 0x8fec1380b655e150) {
        return DXGI_FORMAT_A8P8;
    }
    else if (stringHash == 0xe41f1a1df90e72c9) {
        return DXGI_FORMAT_B4G4R4A4_UNORM;
    }
    else if (stringHash == 0x770a0ec413eb7c1e) {
        return DXGI_FORMAT_P208;
    }
    else if (stringHash == 0x94e117f8e9004343) {
        return DXGI_FORMAT_V208;
    }
    else if (stringHash == 0xbad741f48803b30b) {
        return DXGI_FORMAT_V408;
    }
    else if (stringHash == 0x99bfca274d2d08f1) {
        return DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
    }
    else if (stringHash == 0x5adb18f9a7bf28c3) {
        return DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;
    }
    else if (stringHash == 0x99efe8c93d6e7f70) {
        return DXGI_FORMAT_FORCE_UINT;
    }

    // Default
    return DXGI_FORMAT_UNKNOWN;
}
