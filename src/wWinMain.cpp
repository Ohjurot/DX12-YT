#include "DefHeader.h"

#include <string>
#include <sstream>

// ==== DEBUG START

#include <dx/setup/GIDebug.h>
#include <dx/setup/GIFactory.h>
#include <dx/setup/GIAdapter.h>

#include <dx/core/XDebug.h>
#include <dx/core/XDebugDevice.h>
#include <dx/core/XInfoQueue.h>
#include <dx/core/XDevice.h>

#include <dx/window/GfxWindow.h>

#include <dx/cmds/CommandQueueManager.h>
#include <dx/cmds/CommandListManager.h>
#include <dx/cmds/CommandListAccessObject.h>

// ==== DEBUG END

// Safe Winmain
INT s_wWinMain(HINSTANCE hInstance, PWSTR cmdArgs, INT cmdShow) {
	// Enable and validate debug interface
	DEBUG_ONLY_EXECUTE(DX::XDebug::getInstance().enable());
	DEBUG_ONLY_EXECUTE(DX::GIDebug::getInstance().validate());

	// Create Factory and adapter
	DX::GIFactory factory;
	DX::GIAdapter adapter = factory.getAdapter();

	// Create XDevice
	DX::XDevice xDevice(adapter);

	// Init command queues and lists
	DX::CommandQueueManager::getInstance().createInternalObjects(xDevice);
	DX::CommandListManager::getInstance().createInternalObjects(xDevice);

	// Get factory2
	ScopedComPointer<IDXGIFactory2> factory2;
	EXPP_ASSERT(factory.queryInterface(factory2), "IDXGIFactory1->QueryInterface(...) for IDXGIFactory2 failed");

	// Create window class
	EasyHWND::WindowClass cls(L"MyWindowCls", CS_OWNDC, NULL, LoadCursor(NULL, IDC_ARROW));
	EXPP_ASSERT(cls, "Window class not valid");

	// Create window
	DX::GfxWindow window(cls, xDevice, factory2, L"DirectX 12", DX::GfxWindow_Stlye::WINDOWED);
	window.setWindowVisibility(true);

	// AO
	DX::CommandListAccessObject lao(D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Loop
	while (window) {
		// Check if tread needs to be paused
		if (!window.isVisble()) {
			THREAD_PAUSE_FUNCTION();
			continue;
		}

		window.beginFrame(lao);
		window.endFrame(lao);
		lao.executeExchange().wait();

		// Present frame
		window.present(false);
	}
	lao.release();
	window.release();

	// Release queues and device
	DX::CommandListManager::getInstance().destroyInternalObjects();
	DX::CommandQueueManager::getInstance().destroyInternalObjects();
	xDevice.release();

	// Debug RLO for DXGI
	DEBUG_ONLY_EXECUTE(DX::GIDebug::getInstance().reportLiveObjects());

	return 0;
}

// Unsafe (unhandled) winmain
INT WINAPI wWinMain(HINSTANCE _In_ hInstance, HINSTANCE _In_opt_ hPrevInstance, PWSTR _In_ cmdArgs, INT _In_ cmdShow){
	// Invoke safe winmain
	EXPP::InvocationResult<INT> wWinMain_CallResult = EXPP::invoke<INT>(&s_wWinMain, hInstance, cmdArgs, cmdShow);
	// Return return value on succeed
	if (wWinMain_CallResult.succeeded()) {
		return wWinMain_CallResult.returnValue();
	}

	// Build base exeption header
	std::wstringstream wss;
	wss << wWinMain_CallResult.getException().what() << std::endl;
	wss << L"File: " << wWinMain_CallResult.getException().file() << std::endl;
	wss << L"Line: " << wWinMain_CallResult.getException().line() << std::endl;

	// Handle several errors
	// HRESULT
	wWinMain_CallResult.handle<HRESULT>([](std::wstringstream* ptrWss, HRESULT* ptrHresult) {
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
	wWinMain_CallResult.handle<std::exception>([](std::wstringstream* ptrWss, std::exception* ptrEx) {
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

	return 0;
}
