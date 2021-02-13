#include "DefHeader.h"
#include "GISwapChain.h"

DX::GISwapChain::GISwapChain(ID3D12Device* ptrDevice, ID3D12CommandQueue* ptrCmdQueue, IDXGIFactory2* ptrFactory, EasyHWND::Window& refWindow, GISwapChain_BufferingMode bufferingMode, DXGI_FORMAT bufferFormat) :
	m_ptrDevice(ptrDevice)
{
	// Create heap descriptor
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

	// Desribe heap
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	
	rtvHeapDesc.NumDescriptors = (unsigned int)bufferingMode;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = NULL;

	// Create heap
	EVALUATE_HRESULT(m_ptrDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_ptrRtvHeap.to())), "ID3D12Device->CreateDescriptorHeap(...) for Heap type D3D12_DESCRIPTOR_HEAP_TYPE_RTV");

	// Get heap increment size
	m_uiRTVHeapIncrement = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	EXPP_ASSERT(m_uiRTVHeapIncrement, "Failed to get HeapHandleIncrementSize for heap type D3D12_DESCRIPTOR_HEAP_TYPE_RTV");

	// Retrive window client rect
	RECT windowClientRect = {};
	EXPP_ASSERT(GetClientRect((HWND)refWindow, &windowClientRect), "GISwapChain -> GetClientRect(...) for target window.");

	// Create Swap chain descriptor
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

	// Describe swap chain
	swapChainDesc.Width = windowClientRect.right - windowClientRect.left;
	swapChainDesc.Height = windowClientRect.bottom - windowClientRect.top;
	swapChainDesc.Format = bufferFormat;
	swapChainDesc.Stereo = false;		 // No 3D
	swapChainDesc.SampleDesc = { 1, 0 }; // No MSAA (not supported by D3D12 here)
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = (unsigned int)bufferingMode;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	// Create swap chain fullscreen mode descriptor
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFDesc;
	ZeroMemory(&swapChainFDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));

	// Describe swap chain fullscreen mode
	swapChainFDesc.Windowed = true;

	// Create swap chain
	EVALUATE_HRESULT(ptrFactory->CreateSwapChainForHwnd(ptrCmdQueue, (HWND)refWindow, &swapChainDesc, &swapChainFDesc, NULL, to()), "IDXGIFactory2->CreateSwapChainForHwnd(...)");

	// Store buffer count
	m_uiUsedBuffers = (unsigned int)bufferingMode;

	// Retrive buffer
	retriveBuffer();
}

DX::GISwapChain::~GISwapChain() {
	// Call release
	release();
}

void DX::GISwapChain::present(bool vsync) {
	// Assert pointer
	EXPP_ASSERT(m_comPointer, "GISwapChain -> Present was called on a null swapchain!");

	// Present
	EVALUATE_HRESULT(m_comPointer->Present(vsync ? 1 : 0, vsync ? NULL : DXGI_PRESENT_ALLOW_TEARING), "IDXGISwapChain1->Present(...)");

	// Increment buffer index
	m_uiCurrentBuffer = (m_uiCurrentBuffer + 1) % m_uiUsedBuffers;
}

void DX::GISwapChain::resize(unsigned int width, unsigned int height) {
	// Drop current buffers
	dropBuffer();

	// Resize swap chain
	EVALUATE_HRESULT(m_comPointer->ResizeBuffers(m_uiUsedBuffers, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH), "IDXGISwapChain1->ResizeBuffers(...)");

	// Recreate buffer
	retriveBuffer();
}

ID3D12Resource* DX::GISwapChain::getCurrentBuffer() {
	// Assert buffer and return
	EXPP_ASSERT(m_arrPtrBuffers[m_uiCurrentBuffer], "GISwapChain -> The requested buffer was not created!");
	return m_arrPtrBuffers[m_uiCurrentBuffer];
}

D3D12_CPU_DESCRIPTOR_HANDLE DX::GISwapChain::getCurrentRtvHandle() {
	// Assert buffer
	EXPP_ASSERT(m_arrPtrBuffers[m_uiCurrentBuffer], "GISwapChain -> The requested buffer was not created!");

	// Create handle and increment
	D3D12_CPU_DESCRIPTOR_HANDLE currentRtvHandle = m_ptrRtvHeap->GetCPUDescriptorHandleForHeapStart();
	currentRtvHandle.ptr += (size_t)m_uiCurrentBuffer * m_uiRTVHeapIncrement;

	// Return handle
	return currentRtvHandle;
}

const unsigned int DX::GISwapChain::numberOfFramesInFlight() noexcept{
	return m_uiUsedBuffers;
}

void DX::GISwapChain::release(){
	// Drop buffers
	dropBuffer();

	// Release heap and device
	m_ptrRtvHeap.release();
	m_ptrDevice.release();

	// Call super
	ScopedComPointer::release();
}

void DX::GISwapChain::operator=(GISwapChain& other){
	// Release own
	release();

	// Store ints
	m_uiUsedBuffers = other.m_uiUsedBuffers;
	m_uiRTVHeapIncrement = other.m_uiRTVHeapIncrement;
	m_uiCurrentBuffer = other.m_uiCurrentBuffer;

	// Move critical pointers
	m_comPointer = std::move(other.m_comPointer);
	m_ptrRtvHeap = other.m_ptrRtvHeap;
	m_ptrDevice = other.m_ptrDevice;
	for (unsigned int i = 0; i < other.m_uiUsedBuffers; i++) {
		m_arrPtrBuffers[i] = std::move(other.m_arrPtrBuffers[i]);
	}

	// Null other
	other.m_comPointer = NULL;
}

void DX::GISwapChain::dropBuffer() {
	// For all ocupied buffers
	for (unsigned int i = 0; i < m_uiUsedBuffers; i++) {
		// Release buffers
		m_arrPtrBuffers[i].release();
	}
}

void DX::GISwapChain::retriveBuffer(){
	// Check COM pointer
	EXPP_ASSERT(m_comPointer, "The internal SwapChain COM pointer was null. Cannot retrive buffers from nullptr");

	// Get buffers
	for (unsigned int i = 0; i < m_uiUsedBuffers; i++) {
		// Get buffer
		m_arrPtrBuffers[i].release();
		EVALUATE_HRESULT(m_comPointer->GetBuffer(i, IID_PPV_ARGS(m_arrPtrBuffers[i].to())), "IDXGISwapChain1->GetBuffer(...)");

		// Get heap buffer handle
		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCpu = m_ptrRtvHeap->GetCPUDescriptorHandleForHeapStart();
		heapHandleCpu.ptr += (size_t)m_uiRTVHeapIncrement * i;

		// Create RTV
		m_ptrDevice->CreateRenderTargetView(m_arrPtrBuffers[i], NULL, heapHandleCpu);
	}
}
