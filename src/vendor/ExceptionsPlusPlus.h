/*
*	===============================================================================
*	ExceptionsPlusPlus:
*	Easy advanced exception handling in a single templated C++ header file 
*	More info on GitHub https://github.com/Ohjurot/ExceptionsPlusPlus
*	===============================================================================
* 
*	This library is license under the following two licenses: "MIT License" and 
*	"The Unlicense" You can choose the license witch better fits your work! 
* 
*	===============================================================================
*	License A: MIT License (prefered)
*	===============================================================================
*	MIT License
*
*	Copyright (c) 2021 Ludwig Füchsl
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
* 
*	===============================================================================
*	License B: The Unlicense
*	===============================================================================
*	This is free and unencumbered software released into the public domain.
*
*	Anyone is free to copy, modify, publish, use, compile, sell, or
*	distribute this software, either in source code form or as a compiled
*	binary, for any purpose, commercial or non-commercial, and by any
*	means.
*
*	In jurisdictions that recognize copyright laws, the author or authors
*	of this software dedicate any and all copyright interest in the
*	software to the public domain. We make this dedication for the benefit
*	of the public at large and to the detriment of our heirs and
*	successors. We intend this dedication to be an overt act of
*	relinquishment in perpetuity of all present and future rights to this
*	software under copyright law.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
*	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*	OTHER DEALINGS IN THE SOFTWARE.
*
*	For more information, please refer to <https://unlicense.org>
*
*/

// Include guards
#ifndef EXCEPTIONSPLUSPLUS_H
#define EXCEPTIONSPLUSPLUS_H

// Definitions
#define EXPP_EXCEPTION(type, value, text)					EXPP::Exception<type>(value, text, __LINE__, __FILE__)
#define EXPP_EXCEPTION_T									EXPP::Exception

// Includes
#include <type_traits>
#include <string>
#include <typeinfo>
#include <memory>

// Memory
#define EXPP_MEMORY_ALLOC(type, bytes) ((type)std::malloc(bytes))
#define EXPP_MEMORY_FREE(pointer) std::free((void*)pointer)

// Internal defines
#if defined(EXPP_FORCE_STRING_TYPE_ASCII)
#define EXPP_STRING_TYPE char
#define EXPP_TEXT(text) text
#define EXPP_FORCE_STRING_TYPE
#define EXPP_NO_STR_CONVERT
#elif defined(EXPP_FORCE_STRING_TYPE_UNICODE)
#define EXPP_STRING_TYPE wchar_t
#define EXPP_TEXT(text) L##text
#define EXPP_FORCE_STRING_TYPE
#endif

#if !defined(EXPP_FORCE_STRING_TYPE)
#if defined(_UNICODE)
#define EXPP_STRING_TYPE wchar_t
#define EXPP_TEXT(text) L##text
#else
#define EXPP_STRING_TYPE char
#define EXPP_TEXT(text) text
#define EXPP_NO_STR_CONVERT
#endif
#endif

#define EXPP_TOSTRING(s) #s

#if defined(EXPP_OMMIT_SOURCE)
#define EXPP_NOMMIT_F(a, b) b
#define EXPP_NO_STR_CONVERT
#else
#define EXPP_NOMMIT_F(a, b) a
#endif

// Include OS if required (for char -> wchar_t conversion)
#if !defined(EXPP_NO_STR_CONVERT)
#if defined(_WIN32)
#include <Windows.h>
#else
#error "Your platform is currently not supported!"
#endif
#endif

/// <summary>
/// ExceptionsPlusPlus namespace
/// </summary>
namespace EXPP {
	// Forward declare
	template<typename T, typename ST>
	class Exception;
	template<typename T>
	class BaseException;
	template<typename ST>
	class ExceptionAccessObject;
	template<typename T, typename ST>
	class InvocationResult;

	/// <summary>
	/// Basic exception
	/// </summary>
	/// <typeparam name="T">String type char or wchar_t</typeparam>
	template<typename T = EXPP_STRING_TYPE>
	class BaseException {
		// Static assert string stype
		static_assert(std::is_same_v<std::decay_t<T>, char> || std::is_same_v<std::decay_t<T>, wchar_t>, "IBaseException can only hold string types");
	
		public:
			/// <summary>
			/// Create base exception
			/// </summary>
			/// <param name="line">Line of the exception</param>
			/// <param name="filename">Filename of the exception</param>
			/// <param name="type">Type of the exception</param>
			/// <param name="what">Exeption what</param>
			/// <param name="size">Size of exeption for copy</param>
			template<typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, char>>>
			BaseException(int line, const char* filename, size_t type, const char* what, size_t size = sizeof(BaseException<T>)) :
				m_line(EXPP_NOMMIT_F(line, -1)),
				m_filename((T*)EXPP_NOMMIT_F(filename, "ommited")),
				m_typeHash(type),
				m_converted(false),
				m_what(what),
				m_size(size)
			{}

			/// <summary>
			/// Create base exception
			/// </summary>
			/// <param name="line">Line of the exception</param>
			/// <param name="filename">Filename of the exception</param>
			/// <param name="type">Type of the exception</param>
			/// <param name="what">Exeption what</param>
			template<typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, wchar_t>>>
			BaseException(int line, const char* filename, size_t type, const wchar_t* what, size_t size = sizeof(BaseException<T>)) :
				m_line(EXPP_NOMMIT_F(line, -1)),
				m_filename((T*)EXPP_NOMMIT_F(filename, L"ommited")),
				m_typeHash(type),
				m_converted(EXPP_NOMMIT_F(true, false)),
				m_what(what),
				m_size(size)
			{
				// Convert to wchar
#if !defined(EXPP_OMMIT_SOURCE) && !defined(EXPP_NO_STR_CONVERT)
#if defined(_WIN32)
				// Compute new string length
				int wstrLength = MultiByteToWideChar(CP_UTF8, 0, filename, -1, nullptr, 0);
				// Allocate string
				m_filename = EXPP_MEMORY_ALLOC(wchar_t*, sizeof(wchar_t) * wstrLength);
				// Convert
				MultiByteToWideChar(CP_UTF8, 0, filename, -1, m_filename, wstrLength);
// ADD YOUR OS CHAR -> WCHAR CONVERSION HERE '#elif defined(YOUR_OS_DEFINE_MACRO)' Please considere a pull request with your OS inclusion
#endif
#endif
			}

			// Destruct
			~BaseException() {
				if (m_converted) {
#if !defined(EXPP_OMMIT_SOURCE) && !defined(EXPP_NO_STR_CONVERT)
					EXPP_MEMORY_FREE(m_filename);
#endif
				}
			}

			/// <summary>
			/// Retrive the type of the exeption as hash
			/// </summary>
			/// <returns>typeinfo</returns>
			size_t dynamicType() {
				return m_typeHash;
			}

			/// <summary>
			/// Retrive exeption size in memory
			/// </summary>
			/// <returns>Size of exeption</returns>
			size_t size() {
				return m_size;
			}

			/// <summary>
			/// What function for retriving exeption content
			/// </summary>
			/// <returns>String pointer</returns>
			const T* what() {
				return m_what;
			}

			/// <summary>
			/// Retrive file
			/// </summary>
			/// <returns>File</returns>
			const T* file(){
				return m_filename;
			}

			/// <summary>
			/// Retrive line
			/// </summary>
			/// <returns>Line number</returns>
			int line() {
				return m_line;
			}

		protected:
			// Friend
			template<typename T, typename ST>
			friend class InvocationResult;

			/// <summary>
			/// Copy function
			/// </summary>
			/// <param name="target">Ref to copy target</param>
			virtual void cpy(BaseException<T>& target) { 
				// Memory will be freed by new owner of pointer this will go out of scope soon
				m_converted = false;
			}

		protected:
			/// <summary>
			/// Line of the exeption
			/// </summary>
			int m_line;

			/// <summary>
			/// Indicates that m_filname is allocted by this class due to conversion
			/// </summary>
			bool m_converted;

			/// <summary>
			/// Filename of the exeption
			/// </summary>
			T* m_filename;

			/// <summary>
			/// Exeption inner pointer
			/// </summary>
			const T* m_what;

			/// <summary>
			/// Type of the object as hash
			/// </summary>
			size_t m_typeHash;

			/// <summary>
			/// Dynamic size of object
			/// </summary>
			size_t m_size;
	};

	/// <summary>
	/// Exeption class
	/// </summary>
	/// <typeparam name="T">Exeption content</typeparam>
	/// <typeparam name="ST">Exeption string type</typeparam>
	template<typename T, typename ST = EXPP_STRING_TYPE>
	class Exception : public BaseException<ST> {
		public:
			/// <summary>
			/// Create exeption
			/// </summary>
			/// <param name="value">Exeption value</param>
			/// <param name="text">Exeption string</param>
			/// <param name="line">Line of exeption</param>
			/// <param name="file">File of exeption</param>
			Exception(T& value, const ST* text, int line, const char* file) :
				BaseException<ST>::BaseException(line, file, typeid(T).hash_code(), text, sizeof(Exception<T, ST>)),
				m_value(std::move(value))
			{ }

			/// <summary>
			/// Create exeption
			/// </summary>
			/// <param name="value">Exeption value</param>
			/// <param name="text">Exeption string</param>
			/// <param name="line">Line of exeption</param>
			/// <param name="file">File of exeption</param>
			Exception(T value, const ST* text, int line, const char* file) :
				BaseException<ST>::BaseException(line, file, typeid(T).hash_code(), text, sizeof(Exception<T, ST>)),
				m_value(value)
			{ }

			/// <summary>
			/// Retrive inner value
			/// </summary>
			/// <returns>ref to inner value</returns>
			T& get() {
				return m_value;
			}

			/// <summary>
			/// Static type of a exeption
			/// </summary>
			/// <returns></returns>
			static constexpr size_t staticType() {
				// Compute hash
				return typeid(T).hash_code();
			}

		protected:
			/// <summary>
			/// Copy function
			/// </summary>
			/// <param name="target">Ref to copy target</param>
			virtual void cpy(BaseException<ST>& target) {
				// Cast to pointer
				Exception<T, ST>* ptrTarget = (Exception<T, ST>*)&target;
				// Create Empty data
				T eo = T();
				std::memcpy(&ptrTarget->m_value, &eo, sizeof(T));
				// Copy data
				ptrTarget->m_value = m_value;
				// Call super
				BaseException<ST>::cpy(target);
			}

		private:
			/// <summary>
			/// Exeception value
			/// </summary>
			T m_value;
	};

	/// <summary>
	/// Access Object for Exceptions
	/// </summary>
	/// <typeparam name="ST">Exeption string type</typeparam>
	template<typename ST = EXPP_STRING_TYPE>
	class ExceptionAccessObject {
		public:
			/// <summary>
			/// Create access object
			/// </summary>
			/// <param name="exceptionRef">Reference to exception to access</param>
			ExceptionAccessObject(BaseException<>& exceptionRef) :
				m_ptrExeption(&exceptionRef)
			{}

			/// <summary>
			/// Test if exception type is T
			/// </summary>
			/// <typeparam name="T">Desired type</typeparam>
			/// <returns>True if exeption type and T is the same type</returns>
			template<typename T>
			bool testExceptionType() {
				// Return type comparison
				return m_ptrExeption->dynamicType() == Exception<T>::staticType();
			}

			/// <summary>
			/// Retrive inner data
			/// </summary>
			/// <typeparam name="T">Type to retrive</typeparam>
			/// <returns>Pointer to inner data</returns>
			template<typename T>
			T* getExceptionType() {
				// If test failed return nullptr
				if (!testExceptionType<T>()) {
					return nullptr;
				}

				// Convert to pointer
				Exception<T>* ptr = (Exception<T>*)m_ptrExeption;
				// Return inner data
				return &ptr->get();
			}

		protected:
			/// <summary>
			/// Referenced exeption
			/// </summary>
			BaseException<ST>* m_ptrExeption;
	};

	/// <summary>
	/// Invocation result
	/// </summary>
	/// <typeparam name="T">Type of function return value</typeparam>
	/// <typeparam name="ST">Exeption string type</typeparam>
	template<typename T, typename ST = EXPP_STRING_TYPE>
	class InvocationResult : public ExceptionAccessObject<ST> {
		public:
			/// <summary>
			/// Empty invoation result
			/// </summary>
			InvocationResult() :
				m_returnValue(T()),
				m_exception(new BaseException<>(-1, "", 0, EXPP_TEXT(""))),
				m_succeed(true),
				ExceptionAccessObject<ST>(*m_exception)
			{}

			/// <summary>
			/// Invocation result container by return value --> invocation succeeded
			/// </summary>
			/// <param name="refReturnValue">Reference to return value</param>
			InvocationResult(T& refReturnValue) :
				m_returnValue(std::move(refReturnValue)),
				m_exception(new BaseException<>(-1, "", 0, EXPP_TEXT(""))),
				m_succeed(true),
				ExceptionAccessObject<ST>(*m_exception)
			{}

			/// <summary>
			/// Invocation result container by exception --> incovation failed
			/// </summary>
			/// <param name="refExeption">Refernce to exeption</param>
			InvocationResult(BaseException<ST>& refExeption) :
				m_returnValue(T()),
				m_succeed(false),
				ExceptionAccessObject<ST>(*m_exception)
			{
				// Allocate exeption memory
				m_exception = EXPP_MEMORY_ALLOC(BaseException<ST>*, refExeption.size());
				// Copy base
				std::memcpy(m_exception, &refExeption, sizeof(BaseException<ST>));
				// Copy exception
				refExeption.cpy(*m_exception);
				// Set AO
				ExceptionAccessObject<ST>::m_ptrExeption = m_exception;
			}

			// Destructor
			~InvocationResult() {
				// If succeed delete dummy expetion
				if (m_succeed) {
					delete m_exception;
				}
				// Else free memory
				else {
					EXPP_MEMORY_FREE(m_exception);
				}
			}

			/// <summary>
			/// Test for succeed
			/// </summary>
			/// <returns>true on succeed</returns>
			bool succeeded() {
				return m_succeed;
			}

			/// <summary>
			/// Test for failed
			/// </summary>
			/// <returns>true on fail</returns>
			bool failed() {
				return !m_succeed;
			}

			/// <summary>
			/// Get exception
			/// </summary>
			/// <returns>Referenct to exeption</returns>
			BaseException<ST>& getException() {
				return *m_exception;
			}

			/// <summary>
			/// Handle without any checking
			/// </summary>
			void handle() {
				// Handled
				m_handled = true;
			}

			/// <summary>
			/// Handle all exeption of ET
			/// </summary>
			/// <typeparam name="ET">Type</typeparam>
			/// <returns>if handle succeeds</returns>
			template<typename ET>
			bool handle() {
				// Check if already handled
				if (m_handled) {
					return false;
				}

				// Check if type match
				if (this->testExceptionType<ET>()) {
					// Handled and return true
					m_handled = true;
					return true;
				}

				// Handle failed
				return false;
			}

			/// <summary>
			/// Handle type with matching value
			/// </summary>
			/// <typeparam name="ET">Type of exception</typeparam>
			/// <typeparam name="AT">Type of value</typeparam>
			/// <param name="value">Value to test for</param>
			/// <returns>true if handle</returns>
			template<typename ET, typename AT, typename = std::enable_if_t<std::is_assignable_v<ET&, AT>>>
			bool handle(AT value) {
				// Check if already handled
				if (m_handled) {
					return false;
				}

				// Check if type match
				if (this->testExceptionType<ET>() && *this->getExceptionType<ET>() == (ET)value) {
					// Handled and return true
					m_handled = true;
					return true;
				}

				// Handle failed
				return false;
			}

			/// <summary>
			/// Handle type with handler function / lamda
			/// </summary>
			/// <typeparam name="ET">Type of exception</typeparam>
			/// <typeparam name="FT">Type of function pointer</typeparam>
			/// <typeparam name="...AT">Type of function arguments</typeparam>
			/// <param name="functionPointer">Function pointer or lambda of form 'bool fun(args..., T*)'</param>
			/// <param name="...args">Arguments for function pointer</param>
			/// <returns>true if handled</returns>
			template<typename ET, typename FT, typename... AT, typename = std::enable_if_t<!std::is_same_v<ET, FT>>>
			bool handle(FT functionPointer, AT... args) {
				// Check if already handled
				if (m_handled) {
					return false;
				}

				// Check if type match
				if (this->testExceptionType<ET>() && functionPointer(args..., this->getExceptionType<ET>())) {
					// Handled and return true
					m_handled = true;
					return true;
				}

				// Handle failed
				return false;
			}

			/// <summary>
			/// Handle type with handler function / lamda
			/// </summary>
			/// <typeparam name="ET">Type of exception</typeparam>
			/// <typeparam name="FT">Type of function pointer</typeparam>
			/// <typeparam name="...AT">Type of function arguments</typeparam>
			/// <param name="value">Value to test for</param>
			/// <param name="functionPointer">Function pointer or lambda of form 'bool fun(args...)'</param>
			/// <param name="...args">Arguments for function pointer</param>
			/// <returns>true if handled</returns>
			template<typename ET, typename FT, typename... AT, typename = std::enable_if_t<!std::is_same_v<ET, FT>>>
			bool handle(ET value, FT functionPointer, AT... args) {
				// Check if already handled
				if (m_handled) {
					return false;
				}

				// Check if type match
				if (this->testExceptionType<ET>() && functionPointer(args...)) {
					// Handled and return true
					m_handled = true;
					return true;
				}

				// Handle failed
				return false;
			}

			/// <summary>
			/// Check if got handled
			/// </summary>
			/// <returns>true if no exception is present or exception was handled</returns>
			bool handled(){
				// Return true on succeed or handled
				return m_succeed || m_handled;
			}

			/// <summary>
			/// Raises the exeption if not handled
			/// </summary>
			void rais() {
				// If not succed and not handled
				if (!m_succeed && !m_handled) {
					// Throw exeption
					throw m_exception;
				}
			}

			/// <summary>
			/// Retrive return value
			/// </summary>
			/// <returns>Function value</returns>
			T returnValue() {
				// If failed return empty T
				if (failed()) return T();
				// Else return inner value
				return m_returnValue;
			}

			/// <summary>
			/// Assig operator
			/// </summary>
			/// <param name="other">Other Invocation result</param>
			void operator=(const InvocationResult<T, ST>& other) {
				// If succeed delete dummy expetion
				if (m_succeed) {
					delete m_exception;
				}
				// Else free memory
				else {
					EXPP_MEMORY_FREE(m_exception);
				}

				// Assign new
				m_returnValue = other.m_returnValue;
				m_succeed = other.m_succeed;

				// On succeed use new
				if (m_succeed) {
					m_exception = new BaseException<>(-1, "", 0, EXPP_TEXT(""));
				}
				// Use delete
				else {
					// Copy exeption
					// Allocate exeption memory
					m_exception = EXPP_MEMORY_ALLOC(BaseException<ST>*, other.m_exception->size());
					// Copy base
					std::memcpy(m_exception, other.m_exception, sizeof(BaseException<ST>));
					// Copy exception
					other.m_exception->cpy(*m_exception);
					// Set AO
					ExceptionAccessObject<ST>::m_ptrExeption = m_exception;
				}
			}

		private:
			/// <summary>
			/// Return value of function invocation
			/// </summary>
			T m_returnValue;

			/// <summary>
			/// Incicates weather the invocation succeded
			/// </summary>
			bool m_succeed;

			/// <summary>
			/// Message handled
			/// </summary>
			bool m_handled = false;

			/// <summary>
			/// Internal exeption
			/// </summary>
			BaseException<ST>* m_exception;
	};

	/// <summary>
	/// Invoke function call with exeception guarding
	/// </summary>
	/// <typeparam name="T">Function return type</typeparam>
	/// <typeparam name="ST">String type</typeparam>
	/// <typeparam name="FT">Function pointer type / lambda</typeparam>
	/// <typeparam name="...AT">Function argumen type</typeparam>
	/// <param name="functionPtr">Function pointer</param>
	/// <param name="...args">Arguments</param>
	/// <returns>InvocationResult<T,ST></returns>
	template<typename T, typename ST = EXPP_STRING_TYPE, typename FT, typename... AT>
	InvocationResult<T, ST> invoke(FT functionPtr, AT... args) {
		try {
			// Invoke function
			T rv = functionPtr(args...);
			// All ok craft result from rv
			return InvocationResult<T, ST>(rv);
		}
		// Catch lib exception
		catch (BaseException<ST>& ex) {
			// Return base exception
			return InvocationResult<T, ST>(ex);
		}
		// Catch std::exception
		catch (std::exception& ex) {
			// Return STD Exeception
			auto rt_ex = EXPP_EXCEPTION(std::exception, std::move(ex), EXPP_TEXT("STD Exception"));
			return InvocationResult<T, ST>(rt_ex);
		}
		// Catch unknown exception 
		catch (...) {
			// Return void exeception
			auto rt_ex = EXPP_EXCEPTION(int, 0, EXPP_TEXT("Unknown exception"));
			return InvocationResult<T, ST>(rt_ex);
		}
	}
}

#endif // EXCEPTIONSPLUSPLUS_H
