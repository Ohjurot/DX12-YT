#include "DefHeader.h"
#include "PsoFile.h"

bool dx::PsoFile::read(LPCWSTR file, dx::PsoState& refPsoState) {
    // Open file
    ScopedHandle jsonFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (!jsonFile) {
        return false;
    }

    // Get file size
    DWORD fileSize = GetFileSize(jsonFile, NULL);
    if (!fileSize) {
        return false;
    }

    // Allocate file memory
    char* jsonString = (char*)malloc(fileSize + 1);
    EXPP_ASSERT(jsonString, "Failed to allocate JSON Data buffer");
    jsonString[fileSize] = 0x00;

    // Read json file
    if (!ReadFile(jsonFile, jsonString, fileSize, NULL, NULL)) {
        free(jsonString);
        return false;
    }

    // Parse json file and release memory
    json jsonNode = json::parse(jsonString);
    free(jsonString);

    // Check object
    if (!jsonNode.is_object() || jsonNode.find("type") == jsonNode.end()) {
        return false;
    }

    // Get type
    const std::string typeString = jsonNode["type"];

    // Switch on type
    if (typeString == "gfx") {
        return read_gfx(jsonNode, refPsoState);
    }
    // Default type not known
    else {
        return false;
    }
}

bool dx::PsoFile::read_gfx(json& refJson, dx::PsoState& refPsoState) {  
    // Write GFX Header and clear
    refPsoState.p_psoDescC.isCompute = false;
    refPsoState.p_strAllocator.reset();
    ZeroMemory(&refPsoState.p_psoDescC.gfxDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    // === Read Shaders ===
    auto itShaders = refJson.find("shaders");
    if (itShaders != refJson.end()) {
        // Check shader container
        if (!itShaders.value().is_object()) {
            return false;
        }

        // Convert to object
        auto jsonNodeShaders = itShaders.value();

        // Find VERTEX shader
        auto itVertexShader = jsonNodeShaders.find("vertex");
        if (itVertexShader != jsonNodeShaders.end()) {
            // Convert to wide string
            WCHAR shaderNameW[255] = {};
            common::String::Converter::ansiToWide<255>(((std::string)itVertexShader.value()).c_str(), shaderNameW);

            refPsoState.p_vertexShader = dx::Shader(shaderNameW, SHADER_TYPE_VERTEX);
            if (!refPsoState.p_vertexShader || !refPsoState.p_vertexShader.setBuffer((void**)&refPsoState.p_psoDescC.gfxDesc.VS.pShaderBytecode, &refPsoState.p_psoDescC.gfxDesc.VS.BytecodeLength)) {
                return false;
            }
        }

        // Find PIXEL shader
        auto itPixelShader = jsonNodeShaders.find("pixel");
        if (itPixelShader != jsonNodeShaders.end()) {
            // Convert to wide string
            WCHAR shaderNameW[255] = {};
            common::String::Converter::ansiToWide<255>(((std::string)itPixelShader.value()).c_str(), shaderNameW);

            refPsoState.p_pixelShader = dx::Shader(shaderNameW, SHADER_TYPE_PIXEL);
            if (!refPsoState.p_pixelShader || !refPsoState.p_pixelShader.setBuffer((void**)&refPsoState.p_psoDescC.gfxDesc.PS.pShaderBytecode, &refPsoState.p_psoDescC.gfxDesc.PS.BytecodeLength)) {
                return false;
            }
        }

        // Find HULL shader
        auto itHullShader = jsonNodeShaders.find("hull");
        if (itHullShader != jsonNodeShaders.end()) {
            // Convert to wide string
            WCHAR shaderNameW[255] = {};
            common::String::Converter::ansiToWide<255>(((std::string)itHullShader.value()).c_str(), shaderNameW);

            refPsoState.p_hullShader = dx::Shader(shaderNameW, SHADER_TYPE_HULL);
            if (!refPsoState.p_hullShader || !refPsoState.p_hullShader.setBuffer((void**)&refPsoState.p_psoDescC.gfxDesc.HS.pShaderBytecode, &refPsoState.p_psoDescC.gfxDesc.HS.BytecodeLength)) {
                return false;
            }
        }

        // Find DOMAIN shader
        auto itDomainShader = jsonNodeShaders.find("domain");
        if (itDomainShader != jsonNodeShaders.end()) {
            // Convert to wide string
            WCHAR shaderNameW[255] = {};
            common::String::Converter::ansiToWide<255>(((std::string)itDomainShader.value()).c_str(), shaderNameW);

            refPsoState.p_domainShader = dx::Shader(shaderNameW, SHADER_TYPE_DOMAIN);
            if (!refPsoState.p_domainShader || !refPsoState.p_domainShader.setBuffer((void**)&refPsoState.p_psoDescC.gfxDesc.DS.pShaderBytecode, &refPsoState.p_psoDescC.gfxDesc.DS.BytecodeLength)) {
                return false;
            }
        }

        // Find GEOMETRY shader
        auto itGeometryShader = jsonNodeShaders.find("geometry");
        if (itGeometryShader != jsonNodeShaders.end()) {
            // Convert to wide string
            WCHAR shaderNameW[255] = {};
            common::String::Converter::ansiToWide<255>(((std::string)itGeometryShader.value()).c_str(), shaderNameW);

            refPsoState.p_geometryShader = dx::Shader(shaderNameW, SHADER_TYPE_GEOMETRY);
            if (!refPsoState.p_geometryShader || !refPsoState.p_geometryShader.setBuffer((void**)&refPsoState.p_psoDescC.gfxDesc.GS.pShaderBytecode, &refPsoState.p_psoDescC.gfxDesc.GS.BytecodeLength)) {
                return false;
            }
        }
    }

    // === Read Stream Output ===
    auto itStreamOutput = refJson.find("streamOutput");
    if (itStreamOutput != refJson.end() && itStreamOutput.value().is_object()) {
        // Find semantics
        auto itSemantics = itStreamOutput.value().find("semantics");
        if (itSemantics != itStreamOutput.value().end() && itSemantics.value().is_array()) {
            // Itterate over semantics
            for (auto itSemanticEntry = itSemantics.value().begin(); itSemanticEntry != itSemantics.value().end(); itSemanticEntry++) {
                if (itSemanticEntry.value().is_object()) {
                    // Node
                    auto nodeSOSematicEntry = itSemanticEntry.value();
                    
                    // Get and set default
                    size_t semanticIndex = itSemanticEntry - itSemantics.value().begin();
                    EXPP_ASSERT(semanticIndex < 16, "To many Stream Output semantics");
                    D3D12_SO_DECLARATION_ENTRY* ptrSoEntry = &refPsoState.p_soDescs[semanticIndex];
                    ZeroMemory(ptrSoEntry, sizeof(D3D12_SO_DECLARATION_ENTRY));

                    // Count
                    refPsoState.p_psoDescC.gfxDesc.StreamOutput.NumEntries = (UINT)semanticIndex + 1;
                    refPsoState.p_psoDescC.gfxDesc.StreamOutput.pSODeclaration = refPsoState.p_soDescs;

                    // === Parse Semantic ===
                    // Stream
                    auto itSOStreamId = nodeSOSematicEntry.find("stream");
                    if (itSOStreamId != nodeSOSematicEntry.end() && itSOStreamId.value().is_number_unsigned()) {
                        ptrSoEntry->Stream = itSOStreamId.value();
                    }

                    // Semantic name
                    auto itSOSemanticName = nodeSOSematicEntry.find("name");
                    if (itSOSemanticName != nodeSOSematicEntry.end() && itSOSemanticName.value().is_string()) {
                        // Copy store name
                        CHAR* storedName = refPsoState.p_strAllocator.strAllocCopy(((std::string)itSOSemanticName.value()).c_str());
                        EXPP_ASSERT(storedName, "String allocator out of memory!");
                        ptrSoEntry->SemanticName = storedName;
                    }

                    // Index
                    auto itSOIndex = nodeSOSematicEntry.find("index");
                    if (itSOIndex != nodeSOSematicEntry.end() && itSOIndex.value().is_number_unsigned()) {
                        ptrSoEntry->SemanticIndex = itSOIndex.value();
                    }

                    // Slot
                    auto itSOSlot = nodeSOSematicEntry.find("slot");
                    if (itSOSlot != nodeSOSematicEntry.end() && itSOSlot.value().is_number_unsigned()) {
                        ptrSoEntry->OutputSlot = itSOSlot.value();
                    }

                    // Component
                    auto itSOComponent = nodeSOSematicEntry.find("component");
                    if (itSOComponent != nodeSOSematicEntry.end() && itSOComponent.value().is_object()) {
                        // Start
                        auto itSOComponentStart = itSOComponent.value().find("start");
                        if (itSOComponentStart != itSOComponent.value().end() && itSOComponentStart.value().is_number_unsigned()) {
                            ptrSoEntry->StartComponent = itSOComponentStart.value();
                        }

                        // Count
                        auto itSOComponentCount = itSOComponent.value().find("count");
                        if (itSOComponentCount != itSOComponent.value().end() && itSOComponentCount.value().is_number_unsigned()) {
                            ptrSoEntry->ComponentCount = itSOComponentCount.value();
                        }
                    }
                }
            }
        }

        // Find Strides
        auto itSOStrides = itStreamOutput.value().find("strides");
        if (itSOStrides != itStreamOutput.value().end() && itSOStrides.value().is_array()) {
            for (auto itSOStrideEntrys = itSOStrides.value().begin(); itSOStrideEntrys != itSOStrides.value().end(); itSOStrideEntrys++) {
                // Index
                size_t index = itSOStrideEntrys - itSOStrides.value().begin();

                // Check value
                if (itSOStrideEntrys.value().is_number_unsigned()) {
                    refPsoState.p_soStrides[index] = itSOStrideEntrys.value();
                    refPsoState.p_psoDescC.gfxDesc.StreamOutput.pBufferStrides = refPsoState.p_soStrides;
                    refPsoState.p_psoDescC.gfxDesc.StreamOutput.NumStrides = (UINT)index + 1;
                }
            }
        }

        // Find stream
        auto itSOStream = itStreamOutput.value().find("stream");
        if (itSOStream != itStreamOutput.value().end() && itSOStream.value().is_number_unsigned()) {
            refPsoState.p_psoDescC.gfxDesc.StreamOutput.RasterizedStream = itSOStream.value();
        }
    }

    // === Read Blend State  ====
    auto itBlendState = refJson.find("blend");
    if (itBlendState != refJson.end() && itBlendState.value().is_object()) {
        auto nodeBlendState = itBlendState.value();

        // alphaToCoverage
        auto itBAlphaToCoverage = nodeBlendState.find("alphaToCoverage");
        if (itBAlphaToCoverage != nodeBlendState.end() && itBAlphaToCoverage.value().is_boolean()) {
            refPsoState.p_psoDescC.gfxDesc.BlendState.AlphaToCoverageEnable = itBAlphaToCoverage.value();
        }

        // independentBlend
        auto itBIndependentBlend = nodeBlendState.find("independentBlend");
        if (itBIndependentBlend != nodeBlendState.end() && itBIndependentBlend.value().is_boolean()) {
            refPsoState.p_psoDescC.gfxDesc.BlendState.IndependentBlendEnable = itBIndependentBlend.value();
        }

        // blends
        auto itBBlends = nodeBlendState.find("blends");
        if (itBBlends != nodeBlendState.end() && itBBlends.value().is_array()) {
            for (auto itBBlendsBlend = itBBlends.value().begin(); itBBlendsBlend != itBBlends.value().end(); itBBlendsBlend++) {
                EXPP_ASSERT(itBBlendsBlend.value().is_object(), "Blend needs to be an object");

                // Index and type
                size_t index = itBBlendsBlend - itBBlends.value().begin();
                EXPP_ASSERT(index < 8, "Render target blends out of bound");
                D3D12_RENDER_TARGET_BLEND_DESC* ptrBlend = &refPsoState.p_psoDescC.gfxDesc.BlendState.RenderTarget[index];
                auto nodeBBlendsBlend = itBBlendsBlend.value();

                // Read enable
                auto itBEnable = nodeBBlendsBlend.find("enable");
                if (itBEnable != nodeBBlendsBlend.end() && itBEnable.value().is_boolean()) {
                    ptrBlend->BlendEnable = itBEnable.value();
                }

                // Read logicOpEnable
                auto itBLogicEnable = nodeBBlendsBlend.find("logicOpEnable");
                if (itBLogicEnable != nodeBBlendsBlend.end() && itBLogicEnable.value().is_boolean()) {
                    ptrBlend->LogicOpEnable = itBLogicEnable.value();
                }

                // Read srcBlend
                auto itBSrcBlend = nodeBBlendsBlend.find("srcBlend");
                if (itBSrcBlend != nodeBBlendsBlend.end() && itBSrcBlend.value().is_string()) {
                    std::string srcBlendStr = itBSrcBlend.value();
                    ptrBlend->SrcBlend = dx::strconverter::dx12_textToBlend(srcBlendStr);
                }

                // Read destBlend
                auto itBDestBlend = nodeBBlendsBlend.find("destBlend");
                if (itBDestBlend != nodeBBlendsBlend.end() && itBDestBlend.value().is_string()) {
                    std::string destBlendStr = itBDestBlend.value();
                    ptrBlend->DestBlend = dx::strconverter::dx12_textToBlend(destBlendStr);
                }

                // Read Blend OP
                auto itBBlendOp = nodeBBlendsBlend.find("blendOp");
                if (itBBlendOp != nodeBBlendsBlend.end() && itBBlendOp.value().is_string()) {
                    std::string blendOp = itBBlendOp.value();
                    ptrBlend->BlendOp = dx::strconverter::dx12_textToBlendOp(blendOp);
                }

                // Read srcBlendAlpha
                auto itBSrcBlendAlpha = nodeBBlendsBlend.find("srcBlendAlpha");
                if (itBSrcBlendAlpha != nodeBBlendsBlend.end() && itBSrcBlendAlpha.value().is_string()) {
                    std::string srcBlendAlphaStr = itBSrcBlendAlpha.value();
                    ptrBlend->SrcBlendAlpha = dx::strconverter::dx12_textToBlend(srcBlendAlphaStr);
                }

                // Read destBlendAlpha
                auto itBDestBlendAlpha = nodeBBlendsBlend.find("destBlendAlpha");
                if (itBDestBlendAlpha != nodeBBlendsBlend.end() && itBDestBlendAlpha.value().is_string()) {
                    std::string destBlendAlphaStr = itBDestBlendAlpha.value();
                    ptrBlend->DestBlendAlpha = dx::strconverter::dx12_textToBlend(destBlendAlphaStr);
                }

                // Read blendOpAlpha
                auto itBBlendOpAlpha = nodeBBlendsBlend.find("blendOpAlpha");
                if (itBBlendOpAlpha != nodeBBlendsBlend.end() && itBBlendOpAlpha.value().is_string()) {
                    std::string blendOpAlpha = itBBlendOpAlpha.value();
                    ptrBlend->BlendOpAlpha = dx::strconverter::dx12_textToBlendOp(blendOpAlpha);
                }

                // Read logicOp
                auto itBBlendLogicOp = nodeBBlendsBlend.find("logicOp");
                if (itBBlendLogicOp != nodeBBlendsBlend.end() && itBBlendLogicOp.value().is_string()) {
                    std::string logicOp = itBBlendLogicOp.value();
                    ptrBlend->LogicOp = dx::strconverter::dx12_textToLogicOp(logicOp);
                }

                // Read writeMask
                auto itBWriteMask = nodeBBlendsBlend.find("writeMask");
                if (itBWriteMask != nodeBBlendsBlend.end() && itBWriteMask.value().is_string()) {
                    // For each char
                    std::string writeMask = itBWriteMask.value();
                    for (auto itC = writeMask.begin(); itC != writeMask.end(); itC++) {
                        switch (*itC) {
                            // Red
                            case 'R':
                                ptrBlend->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_RED;
                                break;
                            // Green
                            case 'G':
                                ptrBlend->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
                                break;
                            // Blue
                            case 'B':
                                ptrBlend->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
                                break;
                            // Alpha
                            case 'A':
                                ptrBlend->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
                                break;
                            // All
                            case '*':
                                ptrBlend->RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALL;
                                break;
                        }
                    }
                }
            }
        }
    }

    // === Read sampleMask ===
    auto itSampleMask = refJson.find("sampleMask");
    if (itSampleMask != refJson.end() && itSampleMask.value().is_number_unsigned()) {
        refPsoState.p_psoDescC.gfxDesc.SampleMask = itSampleMask.value();
    }

    // === Read rasterizer ===
    auto itRasterizer = refJson.find("rasterizer");
    if (itRasterizer != refJson.end() && itRasterizer.value().is_object()) {
        // Get node
        auto nodeRasterizer = itRasterizer.value();

        // Read fillMode
        auto itRFillMode = nodeRasterizer.find("fillMode");
        if (itRFillMode != nodeRasterizer.end() && itRFillMode.value().is_string()) {
            std::string fillMode = itRFillMode.value();
            refPsoState.p_psoDescC.gfxDesc.RasterizerState.FillMode = dx::strconverter::dx12_textToFillMode(fillMode);
        }

        // Read cullMode
        auto itRCullMode = nodeRasterizer.find("cullMode");
        if (itRCullMode != nodeRasterizer.end() && itRCullMode.value().is_string()) {
            std::string cullMode = itRCullMode.value();
            refPsoState.p_psoDescC.gfxDesc.RasterizerState.CullMode = dx::strconverter::dx12_textToCullMode(cullMode);
        }

        // Read frontCounterClockwise
        auto itRFrontCounterClockwise = nodeRasterizer.find("frontCounterClockwise");
        if (itRFrontCounterClockwise != nodeRasterizer.end() && itRFrontCounterClockwise.value().is_boolean()) {
            refPsoState.p_psoDescC.gfxDesc.RasterizerState.FrontCounterClockwise = itRFrontCounterClockwise.value();
        }

        // Read bias
        auto itRBias = nodeRasterizer.find("bias");
        if (itRBias != nodeRasterizer.end() && itRBias.value().is_object()) {
            // Get node
            auto nodeRBias = itRBias.value();

            // Read depthBias
            auto itRBDepthBias = nodeRBias.find("depthBias");
            if (itRBDepthBias != nodeRBias.end() && itRBDepthBias.value().is_number_unsigned()) {
                refPsoState.p_psoDescC.gfxDesc.RasterizerState.DepthBias = itRBDepthBias.value();
            }

            // Read depthBiasClamp
            auto itRBDepthBiasClamp = nodeRBias.find("depthBiasClamp");
            if (itRBDepthBiasClamp != nodeRBias.end() && itRBDepthBiasClamp.value().is_number_float()) {
                refPsoState.p_psoDescC.gfxDesc.RasterizerState.DepthBiasClamp = itRBDepthBiasClamp.value();
            }

            // Read slopeScaledDepthBias
            auto itRBSlopeScaledDepthBias = nodeRBias.find("slopeScaledDepthBias");
            if (itRBSlopeScaledDepthBias != nodeRBias.end() && itRBSlopeScaledDepthBias.value().is_number_float()) {
                refPsoState.p_psoDescC.gfxDesc.RasterizerState.SlopeScaledDepthBias = itRBSlopeScaledDepthBias.value();
            }
        }

        // Read depthClipEnable
        auto itRDepthClipEnable = nodeRasterizer.find("depthClipEnable");
        if (itRDepthClipEnable != nodeRasterizer.end() && itRDepthClipEnable.value().is_boolean()) {
            refPsoState.p_psoDescC.gfxDesc.RasterizerState.DepthClipEnable = itRDepthClipEnable.value();
        }

        // Read multisampleEnable
        auto itRMultisampleEnable = nodeRasterizer.find("multisampleEnable");
        if (itRMultisampleEnable != nodeRasterizer.end() && itRMultisampleEnable.value().is_boolean()) {
            refPsoState.p_psoDescC.gfxDesc.RasterizerState.MultisampleEnable = itRMultisampleEnable.value();
        }

        // Read antialiasedLineEnable
        auto itRAntialiasedLineEnable = nodeRasterizer.find("antialiasedLineEnable");
        if (itRAntialiasedLineEnable != nodeRasterizer.end() && itRAntialiasedLineEnable.value().is_boolean()) {
            refPsoState.p_psoDescC.gfxDesc.RasterizerState.AntialiasedLineEnable = itRAntialiasedLineEnable.value();
        }

        // Read forcedSampleCount
        auto itRForcedSampleCount = nodeRasterizer.find("forcedSampleCount");
        if (itRForcedSampleCount != nodeRasterizer.end() && itRAntialiasedLineEnable.value().is_number_unsigned()) {
            refPsoState.p_psoDescC.gfxDesc.RasterizerState.ForcedSampleCount = itRForcedSampleCount.value();
        }

        // Read conservativeRasterizer
        auto itRConservativeRasterizer = nodeRasterizer.find("conservativeRasterizer");
        if (itRConservativeRasterizer != nodeRasterizer.end() && itRConservativeRasterizer.value().is_boolean()) {
            if ((bool)itRConservativeRasterizer.value()) {
                refPsoState.p_psoDescC.gfxDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
            } else {
                refPsoState.p_psoDescC.gfxDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
            }
        }
    }

    // === Read Depth ===
    auto itDepth = refJson.find("depth");
    if (itDepth != refJson.end() && itDepth.value().is_object()) {
        // Read Buffer
        auto itDepthBuffer = itDepth.value().find("buffer");
        if (itDepthBuffer != itDepth.value().end() && itDepthBuffer.value().is_object()) {
            auto depthBufferNode = itDepthBuffer.value();

            // Read enable
            auto itDBEnable = depthBufferNode.find("enable");
            if (itDBEnable != depthBufferNode.end() && itDBEnable.value().is_boolean()) {
                refPsoState.p_psoDescC.gfxDesc.DepthStencilState.DepthEnable = itDBEnable.value();
            }

            // Read depthWrite
            auto itDBDepthWrite = depthBufferNode.find("depthWrite");
            if (itDBDepthWrite != depthBufferNode.end() && itDBDepthWrite.value().is_boolean()) {
                if ((bool)itDBDepthWrite.value()) {
                    refPsoState.p_psoDescC.gfxDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
                } else {
                    refPsoState.p_psoDescC.gfxDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
                }
            }

            // Read function
            auto itDBFunction = depthBufferNode.find("function");
            if (itDBFunction != depthBufferNode.end() && itDBFunction.value().is_string()) {
                std::string depthFunction = itDBFunction.value();
                refPsoState.p_psoDescC.gfxDesc.DepthStencilState.DepthFunc = dx::strconverter::dx12_textToComparision(depthFunction);
            }
        }

        // Read Stencil
        auto itDepthStencil = itDepth.value().find("stencil");
        if (itDepthStencil != itDepth.value().end() && itDepthStencil.value().is_object()) {
            auto depthStencilNode = itDepthStencil.value();

            // Read enable
            auto itDSEnable = depthStencilNode.find("enable");
            if (itDSEnable != depthStencilNode.end() && itDSEnable.value().is_boolean()) {
                refPsoState.p_psoDescC.gfxDesc.DepthStencilState.StencilEnable = itDSEnable.value();
            }

            // Read readMask
            auto itDSReadMask = depthStencilNode.find("readMask");
            if (itDSReadMask != depthStencilNode.end() && itDSReadMask.value().is_number_unsigned()) {
                refPsoState.p_psoDescC.gfxDesc.DepthStencilState.StencilReadMask = itDSReadMask.value();
            }

            // Read writeMask
            auto itDSWriteMask = depthStencilNode.find("writeMask");
            if (itDSWriteMask != depthStencilNode.end() && itDSWriteMask.value().is_number_unsigned()) {
                refPsoState.p_psoDescC.gfxDesc.DepthStencilState.StencilWriteMask = itDSWriteMask.value();
            }

            // Read frontFaces
            auto itDSFrontFaces = depthStencilNode.find("frontFaces");
            if (itDSFrontFaces != depthStencilNode.end() && itDSFrontFaces.value().is_object()) {
                if (!readStencilOpDesc(itDSFrontFaces.value(), &refPsoState.p_psoDescC.gfxDesc.DepthStencilState.FrontFace)) {
                    return false;
                }
            }

            // Read backFacecs
            auto itDSBackFacecs = depthStencilNode.find("backFacecs");
            if (itDSBackFacecs != depthStencilNode.end() && itDSBackFacecs.value().is_object()) {
                if (!readStencilOpDesc(itDSBackFacecs.value(), &refPsoState.p_psoDescC.gfxDesc.DepthStencilState.BackFace)) {
                    return false;
                }
            }
        }
    }

    // === Read Input Layout ===
    auto itInputLayout = refJson.find("inputLayout");
    if (itInputLayout != refJson.end() && itInputLayout.value().is_array()) {
        refPsoState.p_psoDescC.gfxDesc.InputLayout.pInputElementDescs = refPsoState.p_iaLayoutDescs;

        for (auto itInputLayoutElement = itInputLayout.value().begin(); itInputLayoutElement != itInputLayout.value().end(); itInputLayoutElement++) {
            EXPP_ASSERT(itInputLayoutElement.value().is_object(), "Input layout need to be an object");
            
            // Retrive index
            auto IAElementNode = itInputLayoutElement.value();
            size_t index = itInputLayoutElement - itInputLayout.value().begin();
            refPsoState.p_psoDescC.gfxDesc.InputLayout.NumElements = (UINT)index + 1;
            D3D12_INPUT_ELEMENT_DESC* ptrIADesc = &refPsoState.p_iaLayoutDescs[index];

            // Read Name
            auto itIAName = IAElementNode.find("name");
            if (itIAName != IAElementNode.end() && itIAName.value().is_string()) {
                // Name string allocation
                CHAR* iaSemanticName = refPsoState.p_strAllocator.strAllocCopy(((std::string)itIAName.value()).c_str());
                EXPP_ASSERT(iaSemanticName, "String allocation failed");

                // Set name
                ptrIADesc->SemanticName = iaSemanticName;
            }

            // Read index
            auto itIAIndex = IAElementNode.find("index");
            if (itIAIndex != IAElementNode.end() && itIAIndex.value().is_number_unsigned()) {
                ptrIADesc->SemanticIndex = itIAIndex.value();
            }

            // Read format
            auto itIAFormat = IAElementNode.find("format");
            if (itIAFormat != IAElementNode.end() && itIAFormat.value().is_string()) {
                std::string formatString = itIAFormat.value();
                ptrIADesc->Format = dx::strconverter::dx12_textToDiFormat(formatString);
            }

            // Read InputSlot
            auto itIAInputSlot = IAElementNode.find("inputSlot");
            if (itIAInputSlot != IAElementNode.end() && itIAInputSlot.value().is_number_unsigned()) {
                ptrIADesc->InputSlot = itIAInputSlot.value();
            }

            // Read Offset
            auto itIAOffset = IAElementNode.find("offset");
            if (itIAOffset != IAElementNode.end() && itIAOffset.value().is_number_unsigned()) {
                ptrIADesc->AlignedByteOffset = itIAOffset.value();
            }

            // Read Class
            auto itIAClass = IAElementNode.find("class");
            if (itIAClass != IAElementNode.end() && itIAClass.value().is_string()) {
                std::string classString = itIAClass.value();
                ptrIADesc->InputSlotClass = dx::strconverter::dx12_textToInputClass(classString);
            }

            // Read step rate
            auto itIAStepRate = IAElementNode.find("stepRate");
            if (itIAStepRate != IAElementNode.end() && itIAStepRate.value().is_number_unsigned()) {
                ptrIADesc->InstanceDataStepRate = itIAStepRate.value();
            }
        }
    }

    // === Read Strip Cut ===
    auto itStripCut = refJson.find("stripCut");
    if (itStripCut != refJson.end() && itStripCut.value().is_string()) {
        std::string stripCut = itStripCut.value();
        refPsoState.p_psoDescC.gfxDesc.IBStripCutValue = dx::strconverter::dx12_textToStripCut(stripCut);
    }

    // === Read Primitive topology ===
    auto itTopology = refJson.find("primitiveTopology");
    if (itTopology != refJson.end() && itTopology.value().is_string()) {
        std::string topology = itTopology.value();
        refPsoState.p_psoDescC.gfxDesc.PrimitiveTopologyType = dx::strconverter::dx12_textToPrimitiveTopology(topology);
    }

    // === Read Render Target Count ===
    auto itRenderTargetCount = refJson.find("renderTargetCount");
    if (itRenderTargetCount != refJson.end() && itRenderTargetCount.value().is_number_unsigned()) {
        refPsoState.p_psoDescC.gfxDesc.NumRenderTargets = itRenderTargetCount.value();
    }

    // === Read Render Target formats ===
    auto itRenderTargetFormats = refJson.find("renderTargetFormats");
    if (itRenderTargetFormats != refJson.end() && itRenderTargetFormats.value().is_array()) {
        for (auto itRenderTargetFormat = itRenderTargetFormats.value().begin(); itRenderTargetFormat != itRenderTargetFormats.value().end(); itRenderTargetFormat++) {
            // Calulate index
            size_t index = itRenderTargetFormat - itRenderTargetFormats.value().begin();
            EXPP_ASSERT(itRenderTargetFormat.value().is_string(), "RTV Format needs to be an string");

            std::string rtvFormat = itRenderTargetFormat.value();
            refPsoState.p_psoDescC.gfxDesc.RTVFormats[index] = dx::strconverter::dx12_textToDiFormat(rtvFormat);
        }
    }

    // === Read Depth formats ===
    auto itDepthFormat = refJson.find("depthFormat");
    if (itDepthFormat != refJson.end() && itDepthFormat.value().is_string()) {
        std::string dFormat = itDepthFormat.value();
        refPsoState.p_psoDescC.gfxDesc.DSVFormat = dx::strconverter::dx12_textToDiFormat(dFormat);
    }

    // === Read Sample ===
    auto itSample = refJson.find("sample");
    if (itSample != refJson.end() && itSample.value().is_object()) {
        // Read count
        auto itSCount = itSample.value().find("count");
        if (itSCount != itSample.value().end() && itSCount.value().is_number_unsigned()) {
            refPsoState.p_psoDescC.gfxDesc.SampleDesc.Count = itSCount.value();
        }

        // Read quality
        auto itSQuality = itSample.value().find("quality");
        if (itSQuality != itSample.value().end() && itSQuality.value().is_number_unsigned()) {
            refPsoState.p_psoDescC.gfxDesc.SampleDesc.Quality = itSQuality.value();
        }
    }

    // Pass
    refPsoState.m_bCompileDataValid = true;
    return true;
}

bool dx::PsoFile::readStencilOpDesc(json& refJson, D3D12_DEPTH_STENCILOP_DESC* ptrStencilOpDesc) {
    // Read opFail
    auto itOpFail = refJson.find("opFail");
    if (itOpFail != refJson.end() && itOpFail.value().is_string()) {
        std::string op = itOpFail.value();
        ptrStencilOpDesc->StencilFailOp = dx::strconverter::dx12_textToStencilOp(op);
    }

    // Read opDepthFail
    auto itOpDepthFail = refJson.find("opDepthFail");
    if (itOpDepthFail != refJson.end() && itOpDepthFail.value().is_string()) {
        std::string op = itOpDepthFail.value();
        ptrStencilOpDesc->StencilDepthFailOp = dx::strconverter::dx12_textToStencilOp(op);
    }

    // Read opPass
    auto itOpPass = refJson.find("opPass");
    if (itOpPass != refJson.end() && itOpPass.value().is_string()) {
        std::string op = itOpPass.value();
        ptrStencilOpDesc->StencilPassOp = dx::strconverter::dx12_textToStencilOp(op);
    }

    // Read function
    auto itFunction = refJson.find("function");
    if (itFunction != refJson.end() && itFunction.value().is_string()) {
        std::string function = itFunction.value();
        ptrStencilOpDesc->StencilFunc = dx::strconverter::dx12_textToComparision(function);
    }

    // Pass
    return true;
}
