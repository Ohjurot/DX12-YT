#pragma once
#include <DefHeader.h>

namespace common {
	namespace String {
		/// <summary>
		/// String converter
		/// </summary>
		class Converter {
			public:
				/// <summary>
				/// Convert wide to ansi string
				/// </summary>
				/// <param name="wideIn">Wide string in</param>
				/// <param name="maxLen">Maxium output length</param>
				/// <param name="ansiOut">Ansi output</param>
				/// <returns>conversion size</returns>
				static size_t wideToAnsi(LPCWSTR wideIn, size_t maxLen, CHAR* ansiOut) noexcept;

				/// <summary>
				/// Convert ansi to wide string
				/// </summary>
				/// <param name="ansiIn">Ansi input string</param>
				/// <param name="maxLen">Maximum length</param>
				/// <param name="wideOut">Output wide char string</param>
				/// <returns>conversion size</returns>
				static size_t ansiToWide(LPCSTR ansiIn, size_t maxLen, WCHAR* wideOut) noexcept;

				/// <summary>
				/// Templated wide to ansi
				/// </summary>
				/// <param name="wideIn">Wide string in</param>
				/// <param name="ansiOut">Ansi string out</param>
				/// <returns>Conversion target size</returns>
				template<size_t MAX>
				static size_t wideToAnsi(LPCWSTR wideIn, CHAR* ansiOut) noexcept {
					return wideToAnsi(wideIn, MAX, ansiOut);
				}

				/// <summary>
				/// Templated ansi to wide
				/// </summary>
				/// <param name="ansiIn">Ansi input string</param>
				/// <param name="wideOut">Output wide char string</param>
				/// <returns>Conversion target size</returns>
				template<size_t MAX>
				static size_t ansiToWide(LPCSTR ansiIn, WCHAR* wideOut) noexcept {
					return ansiToWide(ansiIn, MAX, wideOut);
				}
		};
	}
}