#pragma once

#include <DefHeader.h>
#include <engine/rxManager/Resource.h>
#include <engine/rxManager/RxHash.h>
#include <engine/rxManager/RxAllocator.h>

#define RESOURCE_REFID(res) res.refId

#define RESOURCE_NAME(res)						engine::RxTools::getResourceName(res)
#define RESOURCE_CPU_POINTER(res)				engine::RxTools::getCPUPointer(res)
#define RESOURCE_CPU_SIZE(res)					engine::RxTools::getCPUSize(res)
#define RESOURCE_GPU_POINTER(res)				engine::RxTools::getGPUPointer(res)
#define RESOURCE_GPU_VIRTUAL_ADDRESS(res)		engine::RxTools::getGPUVirtualAddress(res)

#define RX_UPDATE_RESULT_UPDATE_CALL_OK			RX_UPDATE_RESULT::UPDATE_CALL_OK 
#define RX_UPDATE_RESULT_UPDATE_CALL_FAIL		RX_UPDATE_RESULT::UPDATE_CALL_FAIL
#define RX_UPDATE_RESULT_INVALID_HANDLE			RX_UPDATE_RESULT::INVALID_HANDLE
#define RX_UPDATE_RESULT_INVALID_PAGE			RX_UPDATE_RESULT::INVALID_PAGE
#define RX_UPDATE_RESULT_NO_IMPLEMENTATION		RX_UPDATE_RESULT::NO_IMPLEMENTATION

/// <summary>
/// Result of resource update
/// </summary>
enum class RX_UPDATE_RESULT {
	/// <summary>
	/// Update OK
	/// </summary>
	UPDATE_CALL_OK,

	/// <summary>
	/// Update Failed
	/// </summary>
	UPDATE_CALL_FAIL,

	/// <summary>
	/// Invalid handle
	/// </summary>
	INVALID_HANDLE,

	/// <summary>
	/// Source or target page does not allow this operation
	/// </summary>
	INVALID_PAGE,

	/// <summary>
	/// No implementation present
	/// </summary>
	NO_IMPLEMENTATION,
};

namespace engine {
	/// <summary>
	/// Tools for handling resource
	/// </summary>
	class RxTools {
		public:
			/// <summary>
			/// Get the resource name
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <returns>name of the resource</returns>
			inline static LPCWSTR getResourceName(RESOURCE& refHandle) noexcept {
				// Only on valid handle
				if (refHandle.pReserved) {
					// Cast return name
					return ((__RX_DATA*)(refHandle.pReserved))->name;
				}

				// Return default empty string
				return L"";
			}

			/// <summary>
			/// Get the resource cpu pointer
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <returns>cpu pointer</returns>
			inline static void* getCPUPointer(RESOURCE& refHandle) noexcept {
				// Only on valid handle
				if (refHandle.pReserved) {
					// Return pointer
					return ((__RX_DATA*)(refHandle.pReserved))->cpuMemory;
				}

				// Fallback nullptr
				return nullptr;
			}

			/// <summary>
			/// Get CPU buffer size
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <returns>Size of CPU buffer</returns>
			inline static UINT64 getCPUSize(RESOURCE& refHandle) noexcept {
				// Only on valid handle
				if (refHandle.pReserved) {
					// Return pointer
					return ((__RX_DATA*)(refHandle.pReserved))->cpuSize;
				}

				// Fallback 0
				return 0;
			}

			/// <summary>
			/// Retrive gpu pointer
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <returns>DirectX resource pointer</returns>
			inline static ID3D12Resource* getGPUPointer(RESOURCE& refHandle) noexcept {
				// Only on valid handle
				if (refHandle.pReserved) {
					// Return pointer
					return ((__RX_DATA*)(refHandle.pReserved))->gpuResource;
				}

				// Fallback nullptr
				return nullptr;
			}

			/// <summary>
			/// Extract the GPU virtual address
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <returns>D3D12_GPU_VIRTUAL_ADDRESS</returns>
			inline static D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress(RESOURCE& refHandle) noexcept {
				// Check handle
				if (refHandle.pReserved) {
					// Get data
					__RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

					// Return GPU Address if valid
					if (ptrData->gpuResource) {
						return ptrData->gpuResource->GetGPUVirtualAddress();
					}
				}

				// Return null
				return {0};
			}

			/// <summary>
			/// Resource creation function
			/// </summary>
			/// <param name="resName">Resource name</param>
			/// <param name="procDiskCpu">Resource proc DISK <--> CPU</param>
			/// <param name="procCpuGpu">Resource proc GPU <--> CPU</param>
			/// <param name="diskPage">Disk Page</param>
			/// <param name="cpuPage">CPU Page</param>
			/// <param name="gpuPage">GPU Page</param>
			/// <returns>Resource HANDLE</returns>
			static RESOURCE createResource(LPCWSTR resName, RESOURCE_PROC procDiskCpu, RESOURCE_PROC procCpuGpu, RX_PAGE diskPage = RX_PAGE_READ_ONLY, RX_PAGE cpuPage = RX_PAGE_READ_WRITE, RX_PAGE gpuPage = RX_PAGE_UNAVAILIBLE);

			/// <summary>
			/// Add disk path to the gpu
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="path">Resource path</param>
			/// <returns>true on succeed</returns>
			static bool resourceAddDiskOrigine(RESOURCE& refHandle, LPCWSTR path) noexcept;

			/// <summary>
			/// Allocate resource on the gpu
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="refHeap">Heap ref</param>
			/// <param name="offset">Heap offset</param>
			/// <param name="ptrResDesc">Resource descriptor pointer</param>
			/// <param name="initialState">Inital state</param>
			/// <param name="ptrClearDesc">Clear descriptor pointer</param>		
			static bool resourceAllocGpu(RESOURCE& refHandle, ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 offset, D3D12_RESOURCE_DESC* ptrResDesc, D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_CLEAR_VALUE* ptrClearDesc = nullptr);
	
			/// <summary>
			/// Allocate memory on the CPU side (will call malloc(...) )
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="size">Size of memory block</param>
			/// <returns>true on succeed</returns>
			static bool resourceAllocCpu(RESOURCE& refHandle, UINT64 size) noexcept;

			/// <summary>
			/// Set CPU memory
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="memory">Memory pointer</param>
			/// <param name="size">Memory size</param>
			/// <returns>true on succeed</returns>
			static bool resourceSetCpu(RESOURCE& refHandle, void* memory, UINT64 size) noexcept;

			/// <summary>
			/// Free GPU Resources
			/// </summary>
			/// <param name="refHandle">Reference handle</param>
			static void resourceFreeGpu(RESOURCE& refHandle);

			/// <summary>
			/// Free CPU Resource (will call free(...) )
			/// </summary>
			/// <param name="refHandle">Reference to handle</param>
			static void resoruceFreeCpu(RESOURCE& refHandle) noexcept;

			/// <summary>
			/// Get the most recent page
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <returns>Most recent page type</returns>
			static RX_PAGE_TYPE resourceMostRecentPageType(RESOURCE& refHandle) noexcept;

			/// <summary>
			/// Get resource version number
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="type">Resource page type</param>
			/// <returns>Version number</returns>
			static UINT64 resourceGetVersion(RESOURCE& refHandle, RX_PAGE_TYPE type) noexcept;

			/// <summary>
			/// Compare typeA and typeB
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="typeA">Input A</param>
			/// <param name="typeB">Input B</param>
			/// <returns>True if A is later or eqal to B</returns>
			static bool resourceComparePageVersions(RESOURCE& refHandle, RX_PAGE_TYPE typeA, RX_PAGE_TYPE typeB) noexcept;

			/// <summary>
			/// Increment version counter
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="type">Page type to increment</param>
			/// <returns>true on succeed</returns>
			static bool resourceIncrementVersion(RESOURCE& refHandle, RX_PAGE_TYPE type) noexcept;

			/// <summary>
			/// Check resource page mask
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="type">Page type</param>
			/// <param name="mask">Mask to check</param>
			/// <returns>true if mask matches set permissions</returns>
			static bool resourceCheckPageMask(RESOURCE& refHandle, RX_PAGE_TYPE type, UCHAR mask) noexcept;

			/// <summary>
			/// Update resource page
			/// </summary>
			/// <param name="refHandle">Resource handle</param>
			/// <param name="src">Source page type</param>
			/// <param name="dest">Destination page type</param>
			/// <param name="ctx">Context</param>
			/// <returns>result of call</returns>
			static RX_UPDATE_RESULT resourcePageUpdate(RESOURCE& refHandle, RX_PAGE_TYPE src, RX_PAGE_TYPE dest, void* ctx = nullptr);
	};
}
