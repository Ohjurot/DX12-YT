#include "DefHeader.h"
#include "StrConverter.h"

size_t common::String::Converter::wideToAnsi(LPCWSTR wideIn, size_t maxLen, CHAR* ansiOut) noexcept {
    return WideCharToMultiByte(CP_UTF8, NULL, wideIn, -1, ansiOut, (int)maxLen, NULL, NULL);
}

size_t common::String::Converter::ansiToWide(LPCSTR ansiIn, size_t maxLen, WCHAR* wideOut) noexcept {
    return MultiByteToWideChar(CP_UTF8, NULL, ansiIn, -1, wideOut, (int)maxLen);
}
