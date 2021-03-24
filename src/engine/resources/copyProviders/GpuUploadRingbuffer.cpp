#include "DefHeader.h"
#include "GpuUploadRingbuffer.h"

engine::GpuUploadRingbuffer::GpuUploadRingbuffer(ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 offset, UINT64 size) :
	m_ptrDevice(ptrDevice),
	m_accessObject(D3D12_COMMAND_LIST_TYPE_COPY),
	m_size(size)
{
	// Create resource
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));

	// Describe resource
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	
	// Create resource
	m_buffer = DX::XResource(ptrDevice, refHeap, offset, &desc, nullptr);

	// OK
	m_bValid = true;
}

engine::GpuUploadRingbuffer::GpuUploadRingbuffer(ID3D12Device* ptrDevice, HEAP_ALLOCATION& refAllocation, UINT64 size) :
	m_ptrDevice(ptrDevice),
	m_accessObject(D3D12_COMMAND_LIST_TYPE_COPY),
	m_size(size)
{
	// Create resource
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));

	// Describe resource
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Create resource
	m_buffer = DX::XResource(ptrDevice, refAllocation, &desc, nullptr);

	// OK
	m_bValid = true;
}

engine::GpuUploadRingbuffer::~GpuUploadRingbuffer() {
	release();
}

void engine::GpuUploadRingbuffer::release() {
	if (m_bValid) {
		m_bValid = false;

		// Execute last time and wait
		auto wo = m_accessObject.executeClose();
		if (wo) {
			wo.wait();
		}

		// Release resoure and heap
		m_buffer.release();

		// Release device pointer
		m_ptrDevice.release();
	}
}

bool engine::GpuUploadRingbuffer::queueUpload(ID3D12Resource* ptrTarge, void* memory, UINT64 offset, UINT64 size, DX::XCommandList::WaitObject& refWaitObject, DX::XCommandList::WaitObject dependency) {
	EXPP_ASSERT(m_bValid, "Invalid upload buffer");
	
	// Itterative Split 
	while (size > m_size) {
		// Upload lower
		if (!queueUpload(ptrTarge, memory, offset, m_size, refWaitObject)) {
			return false;
		}

		// Move next
		memory = (void*)((UINT64)memory + m_size);
		size = size - m_size;
		offset = offset + m_size;
	}
	
	// Class scope begins
	MT::LockGuard janitor(m_lock);

	// Check toal size
	if (!size) {
		return false;
	}

	// Wait until memory is sufficent
	while ( (m_size - m_writeHead + m_readHead) < size ) {
		_internal_execute();
	}

	// Determine copy HIGH
	UINT64 copyHigh = std::min<UINT64>(size, m_size - m_writeHead);
	UINT64 copyLow = size - copyHigh;

	// Add dependency
	if (dependency) {
		m_accessObject.addDependency(dependency);
	}

	// Do copy hight
	if (copyHigh) {
		// Mapping pointer
		void* memoryHigh = nullptr;

		// Compute map range
		D3D12_RANGE mapRange;
		mapRange.Begin = m_writeHead;
		mapRange.End = m_writeHead + copyHigh;

		// Map resource
		if (FAILED(m_buffer->Map(0, &mapRange, &memoryHigh))) {
			return false;
		}

		// Copy data
		void* pvoid = (void*)((UINT64)memoryHigh + m_writeHead);
		memcpy(pvoid, memory, copyHigh);

		// Unmap
		m_buffer->Unmap(0, &mapRange);

		// Queue copy
		m_accessObject->CopyBufferRegion(ptrTarge, offset, m_buffer, m_writeHead, copyHigh);
	}

	// Do copy low
	if (copyLow) {
		// Mapping pointer
		void* memoryLow = nullptr;

		// Compute map range
		D3D12_RANGE mapRange;
		mapRange.Begin = 0;
		mapRange.End = copyLow;

		// Map resource
		if (FAILED(m_buffer->Map(0, &mapRange, &memoryLow))) {
			return false;
		}

		// Copy data
		void* lowStart = (void*)((UINT64)memory + copyHigh);
		memcpy(memoryLow, lowStart, copyLow);

		// Unmap
		m_buffer->Unmap(0, &mapRange);

		// Queue copy
		m_accessObject->CopyBufferRegion(ptrTarge, offset + copyHigh, m_buffer, 0, copyLow);
	}

	// Compute read write increment
	m_writeHead = (m_writeHead + size) % m_size;

	// Set wait object and backlock
	refWaitObject = m_accessObject.createWaitObject();
	m_uiBacklock++;

	// Check if start required
	UINT64 usage = m_size - m_writeHead + m_readHeadInFlight;
	if (usage >= (m_size / 2)) {
		_internal_execute();
	}

	return true;
}

void engine::GpuUploadRingbuffer::kickoff() {
	MT::LockGuard janitor(m_lock);

	// Execute
	_internal_execute();
}

void engine::GpuUploadRingbuffer::name(LPCWSTR name) {
	// Name the buffer
	m_buffer.name(name);
}

void engine::GpuUploadRingbuffer::_internal_execute() {
	// Wait if required
	if (m_currentWaitObject) {
		m_currentWaitObject.wait();
		m_currentWaitObject = DX::XCommandList::WaitObject();

		// Update read head
		m_readHead = m_readHeadInFlight;
	}
	
	// Stop if nothing to do
	if (m_uiBacklock == 0) {
		return;
	}

	// Store new read head
	m_readHeadInFlight = m_writeHead;

	// Execute next
	m_currentWaitObject = m_accessObject.executeExchange();
	m_uiBacklock = 0;
}
