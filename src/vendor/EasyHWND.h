/*
*	===============================================================================
*	EasyHWND:
*	Never ever write your windows windows from scratch
*	More info on GitHub https://github.com/Ohjurot/EasyHWND
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
*	Copyright (c) 2021 Ludwig Fuechsl
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
#ifndef EASYHWND_H
#define EASYHWND_H

// Check windows build target
#ifndef _WIN32
#error EasyHWDN requires windows!
#endif

// Include Windows
#ifndef _WINDOWS_
#include <Windows.h>
#endif

// Define string
#ifdef _UNICODE
#define EASYHWND_STR LPCWSTR
#else
#define EASYHWND_STR LPCSTR
#endif

#ifdef EASYHWND_VIRTUAL_MADNESS
#define EASYHWND_FUNCTION(type) virtual type
#else
#define EASYHWND_FUNCTION(type) type
#endif

/// <summary>
/// Easy HWND namespace
/// </summary>
namespace EasyHWND {
	// Predeclare
	class IWindowListener;
	class __WindowProc;
	class WindowClass;
	class Window;

	/// <summary>
	/// Window listener interface
	/// </summary>
	class IWindowListener {
	protected:
		// Window proc is friend
		friend class __WindowProc;

		/// <summary>
		/// Pure virtual window handle function
		/// </summary>
		/// <param name="ptrLRESULT">Pointer to LRESULT set when return true</param>
		/// <param name="hwnd">Window handle</param>
		/// <param name="msg">Message code</param>
		/// <param name="wParam">Message parameter</param>
		/// <param name="lParam">Message parameter</param>
		/// <returns>True if this handled handled the message</returns>
		virtual bool handleWindowMessage(LRESULT* ptrLRESULT, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	};

	/// <summary>
	/// Window proc container
	/// </summary>
	class __WindowProc {
	private:
		/// <summary>
		/// Window proc for production run
		/// </summary>
		/// <param name="hwnd">Window handle</param>
		/// <param name="msg">Message code</param>
		/// <param name="wParam">Message parameter</param>
		/// <param name="lParam">Message parameter</param>
		/// <returns>Window message return value</returns>
		static LRESULT CALLBACK wndProc_run(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			// Get userclas
			IWindowListener* ptrListener = (IWindowListener*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			// Call userclass
			LRESULT returnValue = NULL;
			if (ptrListener->handleWindowMessage(&returnValue, hwnd, msg, wParam, lParam)) {
				return returnValue;
			}

			// Return default proc
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

	protected:
		// Window class is friend
		friend class WindowClass;

		/// <summary>
		/// Window proc for seting up userdata from create pointer
		/// </summary>
		/// <param name="hwnd">Window handle</param>
		/// <param name="msg">Message code</param>
		/// <param name="wParam">Message parameter</param>
		/// <param name="lParam">Message parameter</param>
		/// <returns>Window message return value</returns>
		static LRESULT CALLBACK wndProc_setup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			// Only execute when message is WM_NCCREATE
			if (msg == WM_NCCREATE) {
				// Get create param
				CREATESTRUCT* ptrCrStruct = (CREATESTRUCT*)lParam;
				// Setup userdata
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ptrCrStruct->lpCreateParams);
				// Setup windowproc
				SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)&__WindowProc::wndProc_run);
				// Hand over message to run proc
				return wndProc_run(hwnd, msg, wParam, lParam);
			}
			// Return default window proc
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	};

	/// <summary>
	/// Window class container
	/// </summary>
	class WindowClass {
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="className">Name for the new window class</param>
		/// <param name="style">Style of the new window class</param>
		/// <param name="icon">Icon for the window</param>
		/// <param name="cursor">Cursor fot the window</param>
		/// <param name="brush">Backgound brush</param>
		/// <param name="menuName">Name of the menue</param>
		/// <param name="hInstance">Hinstance of the application</param>
		WindowClass(EASYHWND_STR className, UINT style, HICON icon = NULL, HCURSOR cursor = NULL, HBRUSH brush = NULL, EASYHWND_STR menuName = NULL, HINSTANCE hInstance = NULL) :
			m_strName(className)
		{
			// Check if HINSTANCE needs to be retrived
			if (!hInstance) {
				hInstance = GetModuleHandle(NULL);
			}

			// Create class description
			WNDCLASSEX wcex;
			ZeroMemory(&wcex, sizeof(WNDCLASSEX));

			// Describe class
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = style;
			wcex.lpfnWndProc = &__WindowProc::wndProc_setup;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInstance;
			wcex.hIcon = icon;
			wcex.hCursor = cursor;
			wcex.hbrBackground = brush;
			wcex.lpszMenuName = menuName;
			wcex.lpszClassName = className;
			wcex.hIconSm = icon;

			// Store hinstance
			m_hInstance = hInstance;

			// Register class
			m_bValid = (RegisterClassEx(&wcex) != NULL);
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~WindowClass() {
			// Only if class is valid
			if (m_bValid) {
				// Unregister class
				UnregisterClass(m_strName, m_hInstance);
			}
		}

		/// <summary>
		/// Cast to bool
		/// </summary>
		operator bool() {
			return m_bValid;
		}

		// Delete unsupported
		WindowClass(const WindowClass&) = delete;
		void operator=(const WindowClass&) = delete;

	protected:
		// Window is a friend
		friend class Window;

		/// <summary>
		/// Retrive the name of the class
		/// </summary>
		/// <returns>Name of the class or null</returns>
		EASYHWND_STR getClassName() {
			// Return name if valid
			if (m_bValid) {
				return m_strName;
			}

			// Else return NULL
			return NULL;
		}

	private:
		/// <summary>
		/// Indicates that the window class is valid
		/// </summary>
		bool m_bValid;

		/// <summary>
		/// Internal hInstance
		/// </summary>
		HINSTANCE m_hInstance;

		/// <summary>
		/// Onw class name
		/// </summary>
		EASYHWND_STR m_strName;
	};

	/// <summary>
	/// Representation of a window
	/// </summary>
	class Window : public IWindowListener {
	public:
		/// <summary>
		/// Create a window
		/// </summary>
		/// <param name="refWndClass">Class to create window on</param>
		/// <param name="windowTitle">Title / Name of the window</param>
		/// <param name="posX">X Position</param>
		/// <param name="posY">Y Position</param>
		/// <param name="width">Window width</param>
		/// <param name="height">Window height</param>
		/// <param name="style">Window style</param>
		/// <param name="exStyle">Window extended style</param>
		/// <param name="menue">Window menue</param>
		/// <param name="parentWindow">Parent window</param>
		/// <param name="hInstance">Hinstance</param>
		Window(WindowClass& refWndClass, EASYHWND_STR windowTitle, INT posX, INT posY, UINT width, UINT height, DWORD style, DWORD exStyle = NULL, HMENU menue = NULL, HWND parentWindow = NULL, HINSTANCE hInstance = NULL) {
			// Check if HINSTANCE needs to be retrived
			if (!hInstance) {
				hInstance = GetModuleHandle(NULL);
			}

			// Create window
			m_windowHandle = CreateWindowEx(
				exStyle,
				refWndClass.getClassName(),
				windowTitle,
				style,
				posX, posY,
				width, height,
				parentWindow,
				menue,
				hInstance,
				this
			);

			// Store window parames
			m_style = style;
			m_styleEx = exStyle;
			m_bHasMenue = (menue != NULL);
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~Window() {
			// Check if window handle is valid
			if (m_windowHandle) {
				// Destroy window
				DestroyWindow(m_windowHandle);
				m_windowHandle = NULL;
			}
		}

		/// <summary>
		/// Checks weather the window has recived a close event
		/// </summary>
		/// <param name="clear">If true close flag will be clear to false</param>
		/// <returns>Current close flag status</returns>
		bool checkWindowCloseFlag(bool clear = true) {
			// Read current value
			bool currentValue = m_bCloseFlag;

			// Clear if required
			if (clear) {
				m_bCloseFlag = false;
			}

			// Return old value
			return currentValue;
		}

		/// <summary>
		/// Adjusts an input width and height to fit the client area
		/// </summary>
		/// <param name="ptrWidth">Pointer t o width</param>
		/// <param name="ptrHeight">Pointer to height</param>
		void adjustSizeForWindow(UINT* ptrWidth, UINT* ptrHeight) {
			// Create window rect
			RECT windowRect = { 0, 0, (LONG)*ptrWidth, (LONG)*ptrHeight };

			// Adjust window rect
			AdjustWindowRectEx(&windowRect, m_style, m_bHasMenue, m_styleEx);

			// Set new values
			*ptrWidth = windowRect.right - windowRect.left;
			*ptrHeight = windowRect.bottom - windowRect.top;
		}

		/// <summary>
		/// Set window visability
		/// </summary>
		/// <param name="visibil">true if visable</param>
		EASYHWND_FUNCTION(void) setWindowVisibility(bool visibil) {
			if (m_windowHandle) {
				if (visibil) {
					ShowWindow(m_windowHandle, SW_SHOW);
				}
				else {
					ShowWindow(m_windowHandle, SW_HIDE);
				}
			}
		}

		/// <summary>
		/// Set window size
		/// </summary>
		/// <param name="width">New width</param>
		/// <param name="height">New height</param>
		EASYHWND_FUNCTION(void) setWindowSize(UINT width, UINT height) {
			// Set window pos if handle exists
			if (m_windowHandle) {
				SetWindowPos(m_windowHandle, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
			}
		};

		/// <summary>
		/// Set window position
		/// </summary>
		/// <param name="x">X Position</param>
		/// <param name="y">Y Position</param>
		EASYHWND_FUNCTION(void) setWindowPosition(INT x, INT y) {
			// Set window pos if handle exists
			if (m_windowHandle) {
				SetWindowPos(m_windowHandle, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}
		}

		/// <summary>
		/// Explicit cast to HWND
		/// </summary>
		explicit operator HWND() {
			return m_windowHandle;
		}

		/// <summary>
		/// Cast to bool (valid)
		/// </summary>
		operator bool() {
			return (m_windowHandle != NULL);
		}

		// Delete unsupported
		Window(const Window&) = delete;
		void operator=(const Window&) = delete;

	protected:
		/// <summary>
		/// Window handle function
		/// </summary>
		/// <param name="ptrLRESULT">Pointer to LRESULT set when return true</param>
		/// <param name="hwnd">Window handle</param>
		/// <param name="msg">Message code</param>
		/// <param name="wParam">Message parameter</param>
		/// <param name="lParam">Message parameter</param>
		/// <returns>True if this handled handled the message</returns>
		virtual bool handleWindowMessage(LRESULT* ptrLRESULT, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override {
			// Switch on message
			switch (msg) {
				// On window close
			case WM_CLOSE:
				// Set close flag and mark handled
				m_bCloseFlag = true;
				return true;
			}

			// Message got not handled
			return false;
		}

	private:
		/// <summary>
		/// Internal window handle
		/// </summary>
		HWND m_windowHandle;

		/// <summary>
		/// Styles of window
		/// </summary>
		DWORD m_style, m_styleEx;

		/// <summary>
		/// Indicates weather the window has a menue
		/// </summary>
		bool m_bHasMenue;

		/// <summary>
		/// Close flag
		/// </summary>
		bool m_bCloseFlag = false;
	};
}

#endif
