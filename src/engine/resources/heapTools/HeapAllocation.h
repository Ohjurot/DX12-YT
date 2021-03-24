#pragma once

#include <DefHeader.h>

#include <dx/memory/XHeap.h>

/// <summary>
/// Represents an allocation on a GPU heap
/// </summary>
typedef struct _HEAP_ALLOCATION {
	/// <summary>
	/// Heap pointer
	/// </summary>
	DX::XHeap* ptrHeap;
	
	/// <summary>
	/// Offset into heap
	/// </summary>
	UINT64 offset;

	/// <summary>
	/// Size of allocation
	/// </summary>
	UINT64 size;

} HEAP_ALLOCATION;