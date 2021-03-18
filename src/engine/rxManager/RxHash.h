#pragma once

#include <DefHeader.h>
#include <xxhash.h>

#include <engine/rxManager/Resource.h>

namespace engine {
	/// <summary>
	/// Hashing class for resources
	/// </summary>
	class RxHash {
		public:
			/// <summary>
			/// Sign a handle
			/// </summary>
			/// <param name="refResource">Reference to resource</param>
			/// <returns>true if succeeded</returns>
			static bool singHandle(RESOURCE& refResource) noexcept;
	};
}