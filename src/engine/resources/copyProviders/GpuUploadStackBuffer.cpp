#include "DefHeader.h"
#include "GpuUploadStackBuffer.h"

engine::GpuUploadStackBuffer::GpuUploadStackBuffer(ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 heapOffset, UINT64 size) :
	m_accessObject(D3D12_COMMAND_LIST_TYPE_COPY),
	m_size(size)
{
	EXPP_ASSERT(ptrDevice, "Invalid device pointer");

	// Create buffer descritor
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));

	// Describe buffer
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Create resource
	m_buffer = DX::XResource(ptrDevice, refHeap, heapOffset, &desc, nullptr);
}

engine::GpuUploadStackBuffer::GpuUploadStackBuffer(ID3D12Device* ptrDevice, HEAP_ALLOCATION& refAllocation, UINT64 size) :
	m_accessObject(D3D12_COMMAND_LIST_TYPE_COPY),
	m_size(size)
{
	EXPP_ASSERT(ptrDevice, "Invalid device pointer");

	// Create buffer descritor
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));

	// Describe buffer
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Create resource
	m_buffer = DX::XResource(ptrDevice, refAllocation, &desc, nullptr);
}

engine::GpuUploadStackBuffer::~GpuUploadStackBuffer() {
	release();
}

void engine::GpuUploadStackBuffer::release() {
	// Wait if required
	if (m_waitObject) {
		m_waitObject.wait();
	}

	// Release resource and ao
	m_accessObject.executeClose();
	m_buffer.release();
}

void engine::GpuUploadStackBuffer::name(LPCWSTR name) {
	// Name buffer
	m_buffer.name(name);
}

bool engine::GpuUploadStackBuffer::queueCopy(ID3D12Resource* ptrTarget, UINT64 targetOffset, void* ptrSource, UINT64 copySize) {
	EXPP_ASSERT(ptrTarget, "Invalid target");
	EXPP_ASSERT(ptrSource && copySize, "Invalid source");

	
	
	// Wait if required
	if (m_waitObject) {
		m_waitObject.wait();
		m_writeHead = 0;
	}

	// Check if sufficent
	if (m_writeHead + copySize <= copySize) {
		// Prepare maping
		BYTE* ptrBuffer = nullptr;
		D3D12_RANGE mapRange;
		mapRange.Begin = m_writeHead;
		mapRange.End = m_writeHead + copySize;

		// Map and copy
		EVALUATE_HRESULT(m_buffer->Map(0, &mapRange, (void**)&ptrBuffer), "ID3D12Resource->Map(...)");
		memcpy(&ptrBuffer[m_writeHead], ptrSource, copySize);
		m_buffer->Unmap(0, &mapRange);

		// Copy on GPU side
		m_accessObject->CopyBufferRegion(ptrTarget, targetOffset, m_buffer, m_writeHead, copySize);

		// Increment write head
		m_writeHead += copySize;
	}

	// Fallback false
	return false;
}

DX::XCommandList::WaitObject engine::GpuUploadStackBuffer::execute(DX::XCommandList::WaitObject dependency) {
	// Lock
	MT::LockGuard janitor(m_lock);
	
	// Check if currently executing
	if (m_waitObject) {
		EXPP_ASSERT(!dependency, "Dependency cant be injected on an running stack upload buffer!");

		// Return current wait object
		return m_waitObject;
	}

	// Check if execute is required
	if (m_writeHead) {
		// Injetct depednecy
		if (dependency) {
			m_accessObject.addDependency(dependency);
		}

		// Execute exchange and return
		m_waitObject = m_accessObject.executeExchange();
		return m_waitObject;
	}

	// Return null object
	return DX::XCommandList::WaitObject();
}
