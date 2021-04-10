#include "DefHeader.h"
#include "dx12str.h"

D3D12_BLEND dx::strconverter::dx12_textToBlend(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0x33e6022c2edaa5a7) {
        return D3D12_BLEND_ZERO;
    }
    else if (stringHash == 0x2a5784b5c98d472c) {
        return D3D12_BLEND_ONE;
    }
    else if (stringHash == 0xc4c2aa06f229c17a) {
        return D3D12_BLEND_SRC_COLOR;
    }
    else if (stringHash == 0x6029d9ec3584bbe3) {
        return D3D12_BLEND_INV_SRC_COLOR;
    }
    else if (stringHash == 0xd299a9549a46b9ce) {
        return D3D12_BLEND_SRC_ALPHA;
    }
    else if (stringHash == 0xdc16754d97818023) {
        return D3D12_BLEND_INV_SRC_ALPHA;
    }
    else if (stringHash == 0x6c7ff4a377f3928f) {
        return D3D12_BLEND_DEST_ALPHA;
    }
    else if (stringHash == 0x1a9ec953d22aaeef) {
        return D3D12_BLEND_INV_DEST_ALPHA;
    }
    else if (stringHash == 0x0bc94ee603d0f8c2) {
        return D3D12_BLEND_DEST_COLOR;
    }
    else if (stringHash == 0xcf2902c41bb2b86b) {
        return D3D12_BLEND_INV_DEST_COLOR;
    }
    else if (stringHash == 0x44f92468bd5bb356) {
        return D3D12_BLEND_SRC_ALPHA_SAT;
    }
    else if (stringHash == 0xfe21d0837ef08151) {
        return D3D12_BLEND_BLEND_FACTOR;
    }
    else if (stringHash == 0x837e38971a63f8ff) {
        return D3D12_BLEND_INV_BLEND_FACTOR;
    }
    else if (stringHash == 0x103614c7e1900bd1) {
        return D3D12_BLEND_SRC1_COLOR;
    }
    else if (stringHash == 0xdb5000a364d66357) {
        return D3D12_BLEND_INV_SRC1_COLOR;
    }
    else if (stringHash == 0x88766166d73108ad) {
        return D3D12_BLEND_SRC1_ALPHA;
    }
    else if (stringHash == 0xab2197448b14914d) {
        return D3D12_BLEND_INV_SRC1_ALPHA;
    }

    // Default
    return D3D12_BLEND_ZERO;
}

D3D12_BLEND_OP dx::strconverter::dx12_textToBlendOp(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0xc31388a07c2e4085) {
        return D3D12_BLEND_OP_ADD;
    }
    else if (stringHash == 0x373e1d513276c530) {
        return D3D12_BLEND_OP_SUBTRACT;
    }
    else if (stringHash == 0xaa07de28bfbce991) {
        return D3D12_BLEND_OP_REV_SUBTRACT;
    }
    else if (stringHash == 0xcfdd6a54eead83bf) {
        return D3D12_BLEND_OP_MIN;
    }
    else if (stringHash == 0x3b55cae0125b14a6) {
        return D3D12_BLEND_OP_MAX;
    }

    // Default
    return D3D12_BLEND_OP_ADD;
}

D3D12_LOGIC_OP dx::strconverter::dx12_textToLogicOp(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0x1c9b4f76e88ab787) {
        return D3D12_LOGIC_OP_CLEAR;
    }
    else if (stringHash == 0x89d0b44f97cf723e) {
        return D3D12_LOGIC_OP_SET;
    }
    else if (stringHash == 0xd7ab31b889b8afdf) {
        return D3D12_LOGIC_OP_COPY;
    }
    else if (stringHash == 0x4a0fbff19246eb9b) {
        return D3D12_LOGIC_OP_COPY_INVERTED;
    }
    else if (stringHash == 0xfe6028594d0cdb99) {
        return D3D12_LOGIC_OP_NOOP;
    }
    else if (stringHash == 0xed06224e951fa7cf) {
        return D3D12_LOGIC_OP_INVERT;
    }
    else if (stringHash == 0x690e2f20b5611ad4) {
        return D3D12_LOGIC_OP_AND;
    }
    else if (stringHash == 0x832ba229f2793202) {
        return D3D12_LOGIC_OP_NAND;
    }
    else if (stringHash == 0xd3709755c7f6ebdb) {
        return D3D12_LOGIC_OP_OR;
    }
    else if (stringHash == 0xdc1301f4020e1dd4) {
        return D3D12_LOGIC_OP_NOR;
    }
    else if (stringHash == 0xdf03e1a1c3c206eb) {
        return D3D12_LOGIC_OP_XOR;
    }
    else if (stringHash == 0xe87d45bb27a0aa89) {
        return D3D12_LOGIC_OP_EQUIV;
    }
    else if (stringHash == 0x044fcc7c5714b9c2) {
        return D3D12_LOGIC_OP_AND_REVERSE;
    }
    else if (stringHash == 0x954514807fb7ad83) {
        return D3D12_LOGIC_OP_AND_INVERTED;
    }
    else if (stringHash == 0xbba917566d4d1518) {
        return D3D12_LOGIC_OP_OR_REVERSE;
    }
    else if (stringHash == 0xe092c6365e17608f) {
        return D3D12_LOGIC_OP_OR_INVERTED;
    }

    // Default
    return D3D12_LOGIC_OP_NOOP;
}

D3D12_FILL_MODE dx::strconverter::dx12_textToFillMode(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0x5c2e70e1600ab9ec) {
        return D3D12_FILL_MODE_SOLID;
    }
    else if (stringHash == 0x3f64b7128bbf1554) {
        return D3D12_FILL_MODE_WIREFRAME;
    }

    // Default
    return D3D12_FILL_MODE_SOLID;
}

D3D12_CULL_MODE dx::strconverter::dx12_textToCullMode(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0xabceb22f2eb5161e) {
        return D3D12_CULL_MODE_NONE;
    }
    else if (stringHash == 0x23b53b228b9a5ba2) {
        return D3D12_CULL_MODE_FRONT;
    }
    else if (stringHash == 0x81066fccc09304bb) {
        return D3D12_CULL_MODE_BACK;
    }

    // Default
    return D3D12_CULL_MODE_NONE;
}

D3D12_COMPARISON_FUNC dx::strconverter::dx12_textToComparision(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0xda8cff958419c577) {
        return D3D12_COMPARISON_FUNC_NEVER;
    }
    else if (stringHash == 0x911867797af42a55) {
        return D3D12_COMPARISON_FUNC_LESS;
    }
    else if (stringHash == 0xf5685cf924ec3a56) {
        return D3D12_COMPARISON_FUNC_EQUAL;
    }
    else if (stringHash == 0xf583b35cfcec9d9c) {
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    }
    else if (stringHash == 0x2a4e5ced4ab15870) {
        return D3D12_COMPARISON_FUNC_GREATER;
    }
    else if (stringHash == 0xd13ca918091ea11f) {
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    }
    else if (stringHash == 0xf457c5ffe1c679af) {
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    }
    else if (stringHash == 0x59bf19439468a547) {
        return D3D12_COMPARISON_FUNC_ALWAYS;
    }

    // Default
    return D3D12_COMPARISON_FUNC_ALWAYS;
}

D3D12_STENCIL_OP dx::strconverter::dx12_textToStencilOp(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0xe23a924193fa5148) {
        return D3D12_STENCIL_OP_KEEP;
    }
    else if (stringHash == 0xa33013e3c2b8114a) {
        return D3D12_STENCIL_OP_ZERO;
    }
    else if (stringHash == 0x5c8d30193c218311) {
        return D3D12_STENCIL_OP_REPLACE;
    }
    else if (stringHash == 0xb69e35e311a9baa5) {
        return D3D12_STENCIL_OP_INCR_SAT;
    }
    else if (stringHash == 0x25d9d9ef7cf82f3f) {
        return D3D12_STENCIL_OP_DECR_SAT;
    }
    else if (stringHash == 0xb4d530a4f22b27d7) {
        return D3D12_STENCIL_OP_INVERT;
    }
    else if (stringHash == 0x9e6533a239873b50) {
        return D3D12_STENCIL_OP_INCR;
    }
    else if (stringHash == 0x809d4be5ee37c8ce) {
        return D3D12_STENCIL_OP_DECR;
    }

    // Default
    return D3D12_STENCIL_OP_KEEP;
}

D3D12_INPUT_CLASSIFICATION dx::strconverter::dx12_textToInputClass(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0xa4c32c94bf6f6eae) {
        return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    }
    else if (stringHash == 0xd6d98a531a0072e0) {
        return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
    }

    // Default
    return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
}

D3D12_INDEX_BUFFER_STRIP_CUT_VALUE dx::strconverter::dx12_textToStripCut(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0x428d4467e65b7fc3) {
        return D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    }
    else if (stringHash == 0x6cd16a1c1f3df031) {
        return D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
    }
    else if (stringHash == 0xdde0ee2e80c44206) {
        return D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
    }

    // Default
    return D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE dx::strconverter::dx12_textToPrimitiveTopology(std::string& refString) noexcept {
    // Hahs input string
    auto stringHash = common::String::Hash::H64A(refString.c_str());

    // Select
    if (stringHash == 0xf3989d03c657aa5e) {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
    else if (stringHash == 0xfdb1f9c89ff8c740) {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    }
    else if (stringHash == 0x2195e4f794e5a488) {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    }
    else if (stringHash == 0xf4f4278e79a40170) {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }
    else if (stringHash == 0xadf7d715d1978c4c) {
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    }

    // Default
    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
}
