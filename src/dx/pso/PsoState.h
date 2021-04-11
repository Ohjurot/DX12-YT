#pragma once
#include <DefHeader.h>

#include <dx/shader/Shader.h>
#include <common/String/StrAllocator.h>

namespace dx {
	// Predelc
	class PsoFile;

	/// <summary>
	/// PSO Container
	/// </summary>
	struct PsoDescContainer {
		/// <summary>
		/// True if is compute
		/// </summary>
		bool isCompute = false;

		union {
			/// <summary>
			/// GFX Descriptor
			/// </summary>
			D3D12_GRAPHICS_PIPELINE_STATE_DESC gfxDesc;

			/// <summary>
			/// Compute Descriptor
			/// </summary>
			D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc;
		};
	};

	/// <summary>
	/// Pipelinestate
	/// </summary>
	class PsoState {
		public:

			/// <summary>
			/// Compile root signature
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <returns>true if compilation suceeded</returns>
			bool compile(ID3D12Device* ptrDevice);

		protected:
			friend class PsoFile;

			/// <summary>
			/// Inidcates that the all protected data is ready for compilation / initialized
			/// </summary>
			bool m_bCompileDataValid = false;

			/// <summary>
			/// Pipeline State desc
			/// </summary>
			PsoDescContainer p_psoDescC = {};

			/// <summary>
			/// Stream output declarations
			/// </summary>
			D3D12_SO_DECLARATION_ENTRY p_soDescs[16];

			/// <summary>
			/// Input layout descriptors
			/// </summary>
			D3D12_INPUT_ELEMENT_DESC p_iaLayoutDescs[16];

			/// <summary>
			/// Stream output strides
			/// </summary>
			UINT p_soStrides[16];

			/// <summary>
			/// PSO Shaders
			/// </summary>
			dx::Shader p_vertexShader, p_pixelShader, p_hullShader, p_domainShader, p_geometryShader;

			/// <summary>
			/// String allocator
			/// </summary>
			common::String::Allocator<CHAR, 2048> p_strAllocator;

		private:
			/// <summary>
			/// Root signature
			/// </summary>
			ScopedComPointer<ID3D12RootSignature> m_ptrRootSignature;

			/// <summary>
			/// DX Pipelinestate
			/// </summary>
			ScopedComPointer<ID3D12PipelineState> m_ptrPso;
	};
}