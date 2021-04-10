#include "DefHeader.h"
#include "StrHash.h"

XXH64_hash_t common::String::Hash::H64A(const CHAR* string) noexcept {
    // Null fallback
    if (!string) {
        return 0x0;
    }
    
    // Get length
    size_t strLength = strlen(string);
    if (strLength) {
        // Return hash
        return XXH3_64bits(string, strLength);
    }

    // Default null
    return 0x0;
}

XXH64_hash_t common::String::Hash::H64W(const WCHAR* string) noexcept {
    // Null fallback
    if (!string) {
        return 0x0;
    }

    // Get length
    size_t strLength = wcslen(string);
    if (strLength) {
        // Return hash
        return XXH3_64bits(string, strLength * sizeof(WCHAR));
    }

    // Default null
    return 0x0;
}
