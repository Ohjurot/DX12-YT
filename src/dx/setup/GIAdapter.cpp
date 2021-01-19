#include "GIAdapter.h"

DX::GIAdapter::GIAdapter() noexcept :
	ScopedComPointer<IDXGIAdapter>()
{
	// -
}

DX::GIAdapter::GIAdapter(IDXGIAdapter* ptrAdapter) noexcept :
	ScopedComPointer<IDXGIAdapter>(ptrAdapter)
{
	// -
}

DX::GIOutput DX::GIAdapter::findMonitor(const HMONITOR monitor) {
	// If own pointer is invalid return empty output
	if (!m_comPointer)	{
		return DX::GIOutput();
	}

	// Enum outputs
	ScopedComPointer<IDXGIOutput> ptrOutput;
	UINT index = 0;
	while (SUCCEEDED(m_comPointer->EnumOutputs(index, ptrOutput.to()))) {
		// Prepare description
		DXGI_OUTPUT_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_OUTPUT_DESC));

		// Get description
		EVALUATE_HRESULT(ptrOutput->GetDesc(&desc), "IDXGIOutput->GetDesc(...)");

		// Compare monitor handle
		if (desc.Monitor == monitor) {
			// Return new ouptput
			return DX::GIOutput(ptrOutput);
		}

		// Unlock output and increment index
		ptrOutput.release();
		index++;
	}

	// Default return null output
	return DX::GIOutput();
}

bool DX::GIAdapter::getVideoMemory(DXGI_QUERY_VIDEO_MEMORY_INFO* ptrMemory) noexcept {
	// Temp interface
	ScopedComPointer<IDXGIAdapter3> ptrAdapter3;
	// Check if interface is supported
	if (!queryInterface(ptrAdapter3)) {
		return false;
	}

	// Return query call
	return SUCCEEDED(ptrAdapter3->QueryVideoMemoryInfo(NULL, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, ptrMemory));
}

bool DX::GIAdapter::setVideoMemoryReservation(UINT64 reservatioMemoryInBytes) noexcept {
	// Temp interface
	ScopedComPointer<IDXGIAdapter4> ptrAdapter3;
	// Check if interface is supported
	if (!queryInterface(ptrAdapter3)) {
		return false;
	}

	// Return reservation call result 
	return SUCCEEDED(ptrAdapter3->SetVideoMemoryReservation(NULL, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, reservatioMemoryInBytes));
}

bool DX::GIAdapter::getDescription(DXGI_ADAPTER_DESC* ptrDesc) noexcept {
	// Pointer check
	if (!m_comPointer) {
		return false;
	}

	// Return descritpion query result
	return SUCCEEDED(m_comPointer->GetDesc(ptrDesc));
}

bool DX::GIAdapter::getDescription(DXGI_ADAPTER_DESC1* ptrDesc1) noexcept {
	// If not supported return false
	ScopedComPointer<IDXGIAdapter1> adapter1;
	if (!queryInterface(adapter1)) {
		return false;
	}

	// Return descritpion query result
	return SUCCEEDED(adapter1->GetDesc1(ptrDesc1));
}

bool DX::GIAdapter::getDescription(DXGI_ADAPTER_DESC2* ptrDesc2) noexcept {
	// If not supported return false
	ScopedComPointer<IDXGIAdapter2> adapter2;
	if (!queryInterface(adapter2)) {
		return false;
	}

	// Return descritpion query result
	return SUCCEEDED(adapter2->GetDesc2(ptrDesc2));
}

bool DX::GIAdapter::getDescription(DXGI_ADAPTER_DESC3* ptrDesc3) noexcept {
	// If not supported return false
	ScopedComPointer<IDXGIAdapter4> adapter4;
	if (!queryInterface(adapter4)) {
		return false;
	}

	// Return descritpion query result
	return SUCCEEDED(adapter4->GetDesc3(ptrDesc3));
}
