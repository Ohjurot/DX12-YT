#pragma once
#include <DefHeader.h>

#include <common/String/StrConverter.h>
#include <common/String/StrHash.h>

#include <dx/pso/PsoState.h>
#include <dx/strconverter/dx12str.h>
#include <dx/strconverter/gistr.h>

namespace dx {
	class PsoFile {
		public:
			/// <summary>
			/// Dont construct me
			/// </summary>
			PsoFile() = delete;

			/// <summary>
			/// Read file
			/// </summary>
			/// <param name="file">File path</param>
			/// <param name="refPsoState">Ref to pso</param>
			/// <returns>true on succeed</returns>
			static bool read(LPCWSTR file, dx::PsoState& refPsoState);

		private:
			/// <summary>
			/// Read GFX json
			/// </summary>
			/// <param name="refJson">Input JSON</param>
			/// <param name="refPsoState">Output state</param>
			/// <returns>true on succeed</returns>
			static bool read_gfx(json& refJson, dx::PsoState& refPsoState);

			/// <summary>
			/// Read stencil op descritor from json
			/// </summary>
			/// <param name="refJson">Input json</param>
			/// <param name="ptrStencilOpDesc">Pointer to target descriptor</param>
			/// <returns>true on succeed</returns>
			static bool readStencilOpDesc(json& refJson, D3D12_DEPTH_STENCILOP_DESC* ptrStencilOpDesc);
	};
}
