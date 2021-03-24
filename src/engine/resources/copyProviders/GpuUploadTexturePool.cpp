#include "DefHeader.h"
#include "GpuUploadTexturePool.h"

engine::GpuUploadTexturePool::GpuUploadTexturePool(ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 offset, unsigned int bpp, unsigned int tileCount) :
	m_accessObject(D3D12_COMMAND_LIST_TYPE_COPY),
	m_bytesPerPixel((bpp + 7) / 8),
	m_tileCount(tileCount),
	m_tileSize(64ULL * 64ULL * ((bpp + 7) / 8))
{
	// Create resource descriptor
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));

	// Describe resource
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = m_tileSize * m_tileCount;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Create resource
	m_buffer = DX::XResource(ptrDevice, refHeap, offset, &desc, nullptr);
	m_bValid = true;
}

engine::GpuUploadTexturePool::GpuUploadTexturePool(ID3D12Device* ptrDevice, HEAP_ALLOCATION& allocation, unsigned int bpp, unsigned int tileCount) :
	m_accessObject(D3D12_COMMAND_LIST_TYPE_COPY),
	m_bytesPerPixel((bpp + 7) / 8),
	m_tileCount(tileCount),
	m_tileSize(64ULL * 64ULL * ((bpp + 7) / 8))
{
	// Create resource descriptor
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));

	// Describe resource
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = m_tileSize * m_tileCount;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Create resource
	m_buffer = DX::XResource(ptrDevice, allocation, &desc, nullptr);
	m_bValid = true;
}

engine::GpuUploadTexturePool::~GpuUploadTexturePool() {
	release();
}

void engine::GpuUploadTexturePool::release() {
	if (m_bValid) {
		m_bValid = false;

		// Wait if required
		if (m_waitObject) {
			m_waitObject.wait();
		}

		// Wait again if required
		m_waitObject = m_accessObject.executeClose();
		if (m_waitObject) {
			m_waitObject.wait();
		}

		// Release AO
		m_accessObject.release();
		
		// Release recource
		m_buffer.release();
	}
}

void engine::GpuUploadTexturePool::name(LPCWSTR name) {
	// Name heap
	m_buffer.name(name);
}

void engine::GpuUploadTexturePool::kickoff() {
	// Lock function
	MT::LockGuard janitor(m_lock);

	// Internal start
	if (usage) {
		_internal_execute();
	}
}

void engine::GpuUploadTexturePool::queueUploadTexture(ID3D12Resource* ptrTarget, void* ptrSource, UINT64 sourceHeight, UINT64 sourceWidth, DXGI_FORMAT sourceFormate, DX::XCommandList::WaitObject& refWaitObject, DX::XCommandList::WaitObject dependency) {
	EXPP_ASSERT(ptrSource && sourceHeight && sourceWidth, "Invalid input image");
	EXPP_ASSERT(ptrTarget, "Invalid target image");

	

	// Upload tiles
	for (UINT64 x = 0; x < sourceWidth; x = x + 64) {
		for (UINT64 y = 0; y < sourceHeight; y = y + 64) {
			// Compute copy size
			UINT64 copyWidth = std::min<UINT64>(sourceWidth, x + 64ULL) - x;
			UINT64 copyHeight = std::min<UINT64>(sourceHeight, y + 64ULL) - y;
			
			// Allocate tile
			unsigned int tileIndex = _internal_tileAllocator(refWaitObject, dependency);

			// Upload tile
			_internal_tileUpload(ptrTarget, tileIndex, x, y, copyWidth, copyHeight, sourceWidth, (BYTE*)ptrSource, sourceFormate);
		}
	}
}

void engine::GpuUploadTexturePool::_internal_tileUpload(ID3D12Resource* ptrTarget, unsigned int tile, UINT64 x, UINT64 y, UINT64 copyWidth, UINT64 copyHeight, UINT64 totalWidth, BYTE* ptrSource, DXGI_FORMAT targetFormat) {
	// Buffer Offset and line offset
	const UINT64 bufferStride = m_bytesPerPixel * 64ULL;
	const UINT64 bufferOffset = tile * bufferStride * 64ULL;
	
	// Prepare map
	BYTE* ptrBuffer;
	D3D12_RANGE mapRange;
	mapRange.Begin = bufferOffset;
	mapRange.End = bufferOffset + (bufferStride * 64ULL);

	// Map
	EVALUATE_HRESULT(m_buffer->Map(0, &mapRange, (void**)&ptrBuffer), "ID3D12Resource->Map(...)");

	// Copy tile
	for (unsigned int i = 0; i < copyHeight; i++) {
		memcpy(
			&ptrBuffer[bufferOffset + (i * bufferStride)],
			&ptrSource[(totalWidth * m_bytesPerPixel * (y + i)) + (m_bytesPerPixel * x)],
			copyWidth * m_bytesPerPixel
		);
	}

	// Unmap
	m_buffer->Unmap(0, &mapRange);

	// GPU Copy destination
	D3D12_TEXTURE_COPY_LOCATION locationTarget;
	ZeroMemory(&locationTarget, sizeof(D3D12_TEXTURE_COPY_LOCATION));
	locationTarget.pResource = ptrTarget;
	locationTarget.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	locationTarget.SubresourceIndex = 0;

	// GPU Copy source
	D3D12_TEXTURE_COPY_LOCATION locationSource;
	ZeroMemory(&locationSource, sizeof(D3D12_TEXTURE_COPY_LOCATION));
	locationSource.pResource = m_buffer;
	locationSource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	locationSource.PlacedFootprint.Offset = bufferOffset;
	locationSource.PlacedFootprint.Footprint.Format = targetFormat;
	locationSource.PlacedFootprint.Footprint.Width = 64;
	locationSource.PlacedFootprint.Footprint.Height = 64;
	locationSource.PlacedFootprint.Footprint.Depth = 1;
	locationSource.PlacedFootprint.Footprint.RowPitch = m_bytesPerPixel * 64;

	// GPU Copy bounds
	D3D12_BOX copyBox;
	copyBox.left = 0;
	copyBox.top = 0;
	copyBox.front = 0;
	copyBox.right = (UINT)copyWidth;
	copyBox.bottom = (UINT)copyHeight;
	copyBox.back = 1;

	// Start GPU side copy
	m_accessObject->CopyTextureRegion(&locationTarget, (UINT)x, (UINT)y, 0, &locationSource, &copyBox);
}

unsigned int engine::GpuUploadTexturePool::_internal_tileAllocator(DX::XCommandList::WaitObject& refWaitObject, DX::XCommandList::WaitObject dependency) {
	// Execute wait
	while (usage == m_tileCount) {
		_internal_execute();
	}

	// Set wait conditions
	refWaitObject = m_accessObject.createWaitObject();
	if (dependency) {
		m_accessObject.addDependency(dependency);
	}

	// Tile alloc increment
	unsigned int currentTile = m_writeHead;
	m_writeHead = (m_writeHead + 1) % m_tileCount;
	usage++;

	// Alloc tile index
	return currentTile;
}

void engine::GpuUploadTexturePool::_internal_execute() {
	// Wait for last operation if set
	if (m_waitObject) {
		m_waitObject.wait();
		m_waitObject = DX::XCommandList::WaitObject();

		// Update indexes
		usage = usage - m_numInFlight;
		m_numInFlight = 0;
	}
	else {
		// Dispatch new
		m_waitObject = m_accessObject.executeExchange();

		// Set head
		m_numInFlight = usage;
	}
}
