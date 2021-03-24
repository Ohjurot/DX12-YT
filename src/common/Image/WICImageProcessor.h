#pragma once

#include <DefHeader.h>

#include <common/Image/WICMeta.h>

namespace common {
	namespace image {
		/// <summary>
		/// WIC Image processor (read / convert / write)
		/// </summary>
		class WICImageProcessor {
			public:
				/// <summary>
				/// Read wic stream to memory
				/// </summary>
				/// <param name="inputMeta">Input meta info</param>
				/// <param name="targetMemory">Target memory pointer</param>
				/// <param name="targetSize">Target size</param>
				/// <returns>true on suceed</returns>
				static bool wicToMemory(const WIC_META& inputMeta, void* targetMemory, UINT targetSize) noexcept;
		};
	}
}