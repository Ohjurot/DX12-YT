#pragma once

#include <DefHeader.h>

#include <xxhash.h>
#include <xxh3.h>

namespace common {
	namespace String {
		/// <summary>
		/// String hash class
		/// </summary>
		class Hash {
			public:
				/// <summary>
				/// 64-Bit string hash
				/// </summary>
				/// <param name="string">Input string</param>
				/// <returns>64-Bit hash</returns>
				static XXH64_hash_t H64A(const CHAR* string) noexcept;

				/// <summary>
				/// 64-Bit string hash
				/// </summary>
				/// <param name="string">Input string</param>
				/// <returns>64-Bit hash</returns>
				static XXH64_hash_t H64W(const WCHAR* string) noexcept;

				/// <summary>
				/// 64-Bit string hash
				/// </summary>
				/// <param name="string">Input string</param>
				/// <returns>64-Bit hash</returns>
				static inline XXH64_hash_t H64(const CHAR* string) noexcept {
					return H64A(string);
				}

				/// <summary>
				/// 64-Bit string hash
				/// </summary>
				/// <param name="string">Input string</param>
				/// <returns>64-Bit hash</returns>
				static inline XXH64_hash_t H64(const WCHAR* string) noexcept {
					return H64W(string);
				}
		};
	}
}