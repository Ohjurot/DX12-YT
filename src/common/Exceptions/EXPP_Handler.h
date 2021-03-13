#pragma once

#include <DefHeader.h>

#include <iostream>
#include <string>
#include <sstream>

namespace common {
	/// <summary>
	/// EXPP Default Handling / Reporting
	/// </summary>
	class EXPPHandler {
		public:
			/// <summary>
			/// Report know exceptions
			/// </summary>
			/// <typeparam name="T">Invocation return type</typeparam>
			/// <param name="refResult">Reference to result to be reported</param>
			template<typename T>
			static void reportExeption(EXPP::InvocationResult<T>& refResult) {
				// Build base exeption header
				std::wstringstream wss;
				wss << refResult.getException().what() << std::endl;
				wss << L"File: " << refResult.getException().file() << std::endl;
				wss << L"Line: " << refResult.getException().line() << std::endl;

				// Handle several errors
				// HRESULT
				refResult.handle<HRESULT>([](std::wstringstream* ptrWss, HRESULT* ptrHresult) {
					// Header
					*ptrWss << std::endl << L"Windows HRESULT exception:" << std::endl;

					// Convert HRESULT
					LPWSTR hrString = NULL;
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, *ptrHresult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&hrString, 0, NULL);
					if (hrString) {
						*ptrWss << "Message: " << hrString << std::endl;
						LocalFree(hrString);
					}

					// Error code
					*ptrWss << "Error Code: 0x" << std::hex << (unsigned int)*ptrHresult << std::dec;

					return true;
					}, &wss);

				// STD Exception
				refResult.handle<std::exception>([](std::wstringstream* ptrWss, std::exception* ptrEx) {
					// Header
					*ptrWss << std::endl << L"Generic C++ exception:" << std::endl;

					// Convert to wchar_t
					int newStrLen = MultiByteToWideChar(CP_UTF8, NULL, ptrEx->what(), -1, NULL, NULL);
					wchar_t* buffer = (wchar_t*)malloc(sizeof(wchar_t) * newStrLen);
					MultiByteToWideChar(CP_UTF8, NULL, ptrEx->what(), -1, buffer, newStrLen);

					// Write and free
					*ptrWss << L"Message: " << buffer;
					free(buffer);

					return true;
					}, &wss);


				// Show MessageBox
				MessageBoxW(NULL, wss.str().c_str(), L"Critical Exception", MB_OK | MB_ICONERROR);
			}
	};
}