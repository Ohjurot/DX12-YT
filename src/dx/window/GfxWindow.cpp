#include "DefHeader.h"
#include "GfxWindow.h"

DX::GfxWindow::GfxWindow(EasyHWND::WindowClass& windowClass, ID3D12Device* ptrDevice, IDXGIFactory2* ptrFactory, LPCWSTR windowTitle, GfxWindow_Stlye style, HMONITOR monitor) :
	EasyHWND::Window(windowClass, windowTitle, 0, 0, 1920, 1080, (DWORD)style),
	m_queue(DX::CommandQueueManager::getInstance().getCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)),
	m_fence(ptrDevice)
{
	// Count on queue
	m_queue.incrementRef();

	// Swap chain
	GISwapChain tempChain = GISwapChain(ptrDevice, m_queue, ptrFactory, *this);
	m_swapChain = tempChain;

	// Set counters fence
	m_fenceCounter = m_fence;

	// New frontend for counter
	m_fenceCounterFrontend = m_fenceCounter.newFrontend();

	// Adjust monitor
	if (!monitor) {
		// Get cursor position
		POINT cursorPosition;
		EXPP_ASSERT(GetCursorPos(&cursorPosition), "GetCursorPos(...) failed");

		// Get monitor from point
		monitor = MonitorFromPoint(cursorPosition, MONITOR_DEFAULTTONEAREST);
	}

	// Create monitor info struct
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);

	// Get monitor info
	EXPP_ASSERT(monitor, "Invalid monitor handle");
	EXPP_ASSERT(GetMonitorInfo(monitor, &monitorInfo), "GetMonitorInfo(...) failed");

	// Setup window according to style
	switch (style) {
		// Windowed window
		case GfxWindow_Stlye::WINDOWED:
			this->setWindowPosition(monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top);
			this->setWindowSize(monitorInfo.rcWork.right - monitorInfo.rcWork.left, monitorInfo.rcWork.bottom - monitorInfo.rcWork.top);
			break;

		// Borderless window
		case GfxWindow_Stlye::BORDERLESS:
			this->setWindowPosition(monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top);
			this->setWindowSize(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
			break;

		default:
			EXPP_ASSERT(false, "Unknow window mode");
	}

	// Get client rect
	RECT clientArea;
	EXPP_ASSERT(GetClientRect(this->operator HWND(), &clientArea), "GetClientRect(...) failed");

	// New size
	m_bNeedsResizing = true;
	m_uiNewWidth = clientArea.right - clientArea.left;
	m_uiNewHeight = clientArea.bottom - clientArea.top;
}

DX::GfxWindow::~GfxWindow() {
	release();
}

void DX::GfxWindow::release() {
	// Flush GPU operations
	if (m_bDXValid) {
		flushGpu();
	}

	// Release stuff
	m_fenceCounterFrontend.release();
	m_fenceCounter.release();
	m_fence.release();
	m_swapChain.release();

	// Decount on queu
	if (m_bDXValid) {
		m_queue.decrementRef();
	}

	// Clear valid flag
	m_bDXValid = false;
}

void DX::GfxWindow::flushGpu() {
	EXPP_ASSERT(m_bDXValid, "Windows DirectX context is not valid");

	// Wait for execution
	for (int i = 0; i < m_swapChain.numberOfFramesInFlight(); i++) {
		m_queue->Signal(m_fenceCounterFrontend, m_fenceCounterFrontend.next());
		m_fenceCounterFrontend.getCurrentWaitObject().wait();
	}
}

bool DX::GfxWindow::isVisble() noexcept {
	return m_bVisible;
}

void DX::GfxWindow::present(bool vsync) {
	EXPP_ASSERT(m_bDXValid, "Windows DirectX context is not valid");

	// Check for resizing
	if (m_bNeedsResizing) {
		// Flush and resize
		flushGpu();
		m_swapChain.resize(m_uiNewWidth, m_uiNewHeight);

		// Unset bool
		m_bNeedsResizing = false;
	}
	// Wait for last operation
	else {
		m_fenceCounterFrontend.getCurrentWaitObject().wait();
	}
	
	// Present frame
	m_swapChain.present(vsync);

	// Signal fence
	m_queue->Signal(m_fenceCounterFrontend, m_fenceCounterFrontend.next());
}

DX::GfxWindow::operator bool() {
	// Window loop
	MSG msg = {};
	while (PeekMessage(&msg, this->operator HWND(), 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Return close flag evaluation
	return !this->checkWindowCloseFlag();
}

bool DX::GfxWindow::handleWindowMessage(LRESULT* ptrLRESULT, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Switch on message
	switch (msg) {
		case WM_SIZE:
			// Check is minimized
			if (wParam == SIZE_MINIMIZED) {
				m_bVisible = false;
			}
			// Else was maximized
			else {
				m_bVisible = true;
			}

			// Read size
			m_uiNewWidth = LOWORD(lParam);
			m_uiNewHeight = HIWORD(lParam);
			m_bNeedsResizing = true;
			break;
	}

	// Call super
	return EasyHWND::Window::handleWindowMessage(ptrLRESULT, hwnd, msg, wParam, lParam);
}
