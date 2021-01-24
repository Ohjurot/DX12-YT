#include <DefHeader.h>

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

// ==== DEBUG END

// Safe Winmain
INT s_wWinMain(HINSTANCE hInstance, PWSTR cmdArgs, INT cmdShow) {
	// Enable and validate debug interface
	DEBUG_ONLY_EXECUTE(DX::XDebug::getInstance().enable());
	DEBUG_ONLY_EXECUTE(DX::GIDebug::getInstance().validate());

	// Create Factory
	DX::GIFactory factory;
	bool tearingSupport = factory.checkTearingSupport();

	// Get adapter and its description
	DX::GIAdapter adapter = factory.getAdapter();
	DXGI_ADAPTER_DESC3 ad;
	adapter.getDescription(&ad);

	// Create XDevice
	DX::XDevice xDevice(adapter);

	// Raytracing
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 op5;
	bool op5ok = xDevice.getFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, op5);

	// Debug Device
	DX::XDebugDevice xDbgDevice(xDevice);
	float dbgSlow = xDbgDevice.getDebugSlowdownFactor();

	// Get adapter video memory
	DXGI_QUERY_VIDEO_MEMORY_INFO memInfo;
	bool xres = adapter.setVideoMemoryReservation(MEM_GiB(1));
	adapter.getVideoMemory(&memInfo);

	// Get current monitor
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	HMONITOR monitor = MonitorFromPoint(mousePosition, MONITOR_DEFAULTTOPRIMARY);

	// Get output and descriton
	DX::GIOutput output = adapter.findMonitor(monitor);
	DXGI_OUTPUT_DESC1 od;
	output.getDescription(&od);

	// Release decive
	xDbgDevice.release();
	xDevice.release();

	// Debug RLO for DXGI
	DEBUG_ONLY_EXECUTE(DX::GIDebug::getInstance().reportLiveObjects());

	return 0;
}

// Unsafe (unhandled) winmain
INT WINAPI wWinMain(HINSTANCE _In_ hInstance, HINSTANCE _In_opt_ hPrevInstance, PWSTR _In_ cmdArgs, INT _In_ cmdShow) noexcept{
	// Invoke safe winmain
	EXPP::InvocationResult<INT> wWinMain_CallResult = EXPP::invoke<INT>(&s_wWinMain, hInstance, cmdArgs, cmdShow);
	// Return return value on succeed
	if (wWinMain_CallResult.succeeded())
		return wWinMain_CallResult.returnValue();

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
