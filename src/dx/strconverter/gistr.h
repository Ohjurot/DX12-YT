#pragma once

#include <DefHeader.h>
#include <common/String/StrHash.h>

namespace dx {
	namespace strconverter {
		/// <summary>
		/// Convert string to DXGI Format
		/// </summary>
		/// <param name="refString">Ref to input string</param>
		/// <returns>DXGI Format</returns>
		DXGI_FORMAT dx12_textToDiFormat(std::string& refString) noexcept;
	}
}
