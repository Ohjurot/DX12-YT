#pragma once

#include <DefHeader.h>
#include <xxhash.h>
#include <xxh3.h>

namespace common {
	namespace File {
		/// <summary>
		/// File hashing
		/// </summary>
		class Hashing {
			public:
				/// <summary>
				/// 128-Bit file hashing
				/// </summary>
				/// <param name="file">Path to target file</param>
				/// <param name="ptrHash">Pointer to target hash</param>
				/// <returns>true on succeed</returns>
				static bool hash128bit(LPCWSTR file, XXH128_hash_t* ptrHash);
		};
	}
}
