#pragma once

#include <DefHeader.h>

#include <dx/memory/XResource.h>

#define RX_PAGE_UNAVAILIBLE		RX_PAGE::PAGE_UNAVAILIBLE
#define RX_PAGE_READ_ONLY		RX_PAGE::PAGE_READ_ONLY
#define RX_PAGE_WRITE_ONLY		RX_PAGE::PAGE_WRITE_ONLY
#define RX_PAGE_READ_WRITE		RX_PAGE::PAGE_READ_WRITE

#define RX_PAGE_TYPE_INVALID	RX_PAGE_TYPE::PAGE_TYPE_INVALID
#define RX_PAGE_TYPE_DISK		RX_PAGE_TYPE::PAGE_TYPE_DISK
#define RX_PAGE_TYPE_CPU		RX_PAGE_TYPE::PAGE_TYPE_CPU
#define RX_PAGE_TYPE_GPU		RX_PAGE_TYPE::PAGE_TYPE_GPU

/// <summary>
/// Defines a resource
/// </summary>
typedef struct _RESOURCE {
	UINT64 refId;
	void* pReserved;
} RESOURCE;

/// <summary>
/// Resource access page
/// </summary>
enum class RX_PAGE : unsigned char {
	/// <summary>
	/// Unavailible
	/// </summary>
	PAGE_UNAVAILIBLE = 0x00,

	/// <summary>
	/// Read only
	/// </summary>
	PAGE_READ_ONLY = 0x01,

	/// <summary>
	/// Write only
	/// </summary>
	PAGE_WRITE_ONLY = 0x02,

	/// <summary>
	/// Read and Write
	/// </summary>
	PAGE_READ_WRITE = 0x03,
};

/// <summary>
/// Page type
/// </summary>
enum class RX_PAGE_TYPE {
	/// <summary>
	/// Type is INVALID
	/// </summary>
	PAGE_TYPE_INVALID,

	/// <summary>
	/// Type is DISK
	/// </summary>
	PAGE_TYPE_DISK,

	/// <summary>
	/// Type is CPU
	/// </summary>
	PAGE_TYPE_CPU,

	/// <summary>
	/// Type is GPU
	/// </summary>
	PAGE_TYPE_GPU,
};

// Predecl
struct __RX_DATA;

/// <summary>
/// Resource proc
/// </summary>
typedef bool(*RESOURCE_PROC)(__RX_DATA* ptrResource, RX_PAGE_TYPE source, RX_PAGE_TYPE target, void* context);

/// <summary>
/// Internal
/// </summary>
typedef struct __RX_DATA {
	/// <summary>
	/// Resource name
	/// </summary>
	WCHAR name[255];

	/// <summary>
	/// HDD / SSD Access Page
	/// </summary>
	RX_PAGE diskPage;

	/// <summary>
	/// Disk path
	/// </summary>
	WCHAR diskPath[MAX_PATH];

	/// <summary>
	/// CPU Access page
	/// </summary>
	RX_PAGE cpuPage;

	/// <summary>
	/// Size of the CPU Page
	/// </summary>
	UINT64 cpuSize;

	/// <summary>
	/// CPU Memory pointer
	/// </summary>
	void* cpuMemory;

	/// <summary>
	/// GPU Access page
	/// </summary>
	RX_PAGE gpuPage;

	/// <summary>
	/// GPU Resource
	/// </summary>
	DX::XResource gpuResource;

	/// <summary>
	/// Current itteration on the disk
	/// </summary>
	UINT64 diskVersion;

	/// <summary>
	/// Current itteration on the CPU
	/// </summary>
	UINT64 cpuVersion;

	/// <summary>
	/// Current itteration on the GPU
	/// </summary>
	UINT64 gpuVersion;

	/// <summary>
	/// Resource implementation CPU <--> DISK
	/// </summary>
	RESOURCE_PROC resourceProc_CPU_DISK;

	/// <summary>
	/// Resource implementation CPU <--> GPU
	/// </summary>
	RESOURCE_PROC resourceProc_CPU_GPU;

} _RX_DATA;
