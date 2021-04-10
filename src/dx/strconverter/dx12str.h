#pragma once

#include <DefHeader.h>
#include <common/String/StrHash.h>

namespace dx {
	namespace strconverter {
		/// <summary>
		/// Convert string to blend
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 Blend</returns>
		D3D12_BLEND dx12_textToBlend(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to blend op
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 Blend operation</returns>
		D3D12_BLEND_OP dx12_textToBlendOp(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to logic op
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 Logic operation</returns>
		D3D12_LOGIC_OP dx12_textToLogicOp(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to fill mode
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 Fill Mode</returns>
		D3D12_FILL_MODE dx12_textToFillMode(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to cull mode
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 cull Mode</returns>
		D3D12_CULL_MODE dx12_textToCullMode(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to comparison function
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 comparison function</returns>
		D3D12_COMPARISON_FUNC dx12_textToComparision(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to stencil op
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 stencil op</returns>
		D3D12_STENCIL_OP dx12_textToStencilOp(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to input class
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 input class</returns>
		D3D12_INPUT_CLASSIFICATION dx12_textToInputClass(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to strip cut
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 strip cut</returns>
		D3D12_INDEX_BUFFER_STRIP_CUT_VALUE dx12_textToStripCut(std::string& refString) noexcept;

		/// <summary>
		/// Convert string to primitive topology
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>D3D12 primitive topology</returns>
		D3D12_PRIMITIVE_TOPOLOGY_TYPE dx12_textToPrimitiveTopology(std::string& refString) noexcept;
	}
}
