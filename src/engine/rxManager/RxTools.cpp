#include "DefHeader.h"
#include "RxTools.h"

RESOURCE engine::RxTools::createResource(LPCWSTR resName, RESOURCE_PROC procDiskCpu, RESOURCE_PROC procCpuGpu, RX_PAGE diskPage, RX_PAGE cpuPage, RX_PAGE gpuPage) {
    // Allocate resource
    RESOURCE rHandle = engine::RxAllocator::getInstance()->alloc();
    if (rHandle.pReserved) {
        // Get pointer
        __RX_DATA* ptrData = (__RX_DATA*)rHandle.pReserved;

        // Fill data
        wcscpy_s(ptrData->name, 255, resName);
        ptrData->diskPage = diskPage;
        ptrData->cpuPage = cpuPage;
        ptrData->gpuPage = gpuPage;
        ptrData->resourceProc_CPU_DISK = procDiskCpu;
        ptrData->resourceProc_CPU_GPU = procCpuGpu;

        // Zero Members
        ptrData->diskPath[0] = L'\0';
        ptrData->cpuSize = 0;
        ptrData->cpuMemory = nullptr;
        DX::XResource empty;
        memcpy(&ptrData->gpuResource, &empty, sizeof(DX::XResource));

        // Zero versions
        ptrData->diskVersion = 0;
        ptrData->cpuVersion = 0;
        ptrData->gpuVersion = 0;

        // Sign handle
        engine::RxHash::singHandle(rHandle);
    }

    // Return handle
    return rHandle;
}

bool engine::RxTools::resourceAddDiskOrigine(RESOURCE& refHandle, LPCWSTR path) noexcept {
    // Check handle
    if (refHandle.pReserved) {
        // Get data pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Copy path
        wcscpy_s(ptrData->diskPath, MAX_PATH, path);
        return true;
    }

    // Invalid arguments
    return false;
}

bool engine::RxTools::resourceAllocGpu(RESOURCE& refHandle, ID3D12Device* ptrDevice, DX::XHeap& refHeap, UINT64 offset, D3D12_RESOURCE_DESC* ptrResDesc, D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE* ptrClearDesc) {
    // Check handle
    if (refHandle.pReserved) {
        // Get data pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Return if resource is already valid
        if (ptrData->gpuResource) {
            return false;
        }

        // Create resource
        ptrData->gpuResource = DX::XResource(ptrDevice, refHeap, offset, ptrResDesc, ptrClearDesc, initialState);
        return true;
    }
    
    // Invalid arguments
    return false;
}

bool engine::RxTools::resourceAllocCpu(RESOURCE& refHandle, UINT64 size) noexcept {
    // Check handle
    if (refHandle.pReserved && size) {
        // Get pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Allocate
        ptrData->cpuMemory = malloc(size);

        // On succeed set size and return
        if (ptrData->cpuMemory) {
            ptrData->cpuSize = size;
            return true;
        }
    }

    // Fallback false
    return false;
}

bool engine::RxTools::resourceSetCpu(RESOURCE& refHandle, void* memory, UINT64 size) noexcept {
    // Check handle
    if (refHandle.pReserved) {
        // Get pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Set values
        ptrData->cpuMemory = memory;
        ptrData->cpuSize = size;
        return true;
    }

    // Fallback false
    return false;
}

void engine::RxTools::resourceFreeGpu(RESOURCE& refHandle) {
    // Check handle
    if (refHandle.pReserved) {
        // Get data pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Release resource
        ptrData->gpuResource.release();
    }
}

void engine::RxTools::resoruceFreeCpu(RESOURCE& refHandle) noexcept {
    // Check handle
    if (refHandle.pReserved) {
        // Get pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Free and unset if valid
        if (ptrData->cpuMemory) {
            free(ptrData->cpuMemory);
            ptrData->cpuMemory = nullptr;
            ptrData->cpuSize = 0;
        }
    }
}

RX_PAGE_TYPE engine::RxTools::resourceMostRecentPageType(RESOURCE& refHandle) noexcept {
    // Store 
    RX_PAGE_TYPE mostRecentPage = RX_PAGE_TYPE_INVALID;
    UINT64 mostRecent = 0;

    // Check handle
    if (refHandle.pReserved) {
        // Get pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Find lates 
        // DISK
        if (mostRecent < ptrData->diskVersion) {
            mostRecent = ptrData->diskVersion;
            mostRecentPage = RX_PAGE_TYPE_DISK;
        }
        // CPU
        if (mostRecent < ptrData->cpuVersion) {
            mostRecent = ptrData->cpuVersion;
            mostRecentPage = RX_PAGE_TYPE_CPU;
        }
        // GPU
        if (mostRecent < ptrData->gpuVersion) {
            mostRecent = ptrData->gpuVersion;
            mostRecentPage = RX_PAGE_TYPE_GPU;
        }
    }
    
    return mostRecentPage;
}

UINT64 engine::RxTools::resourceGetVersion(RESOURCE& refHandle, RX_PAGE_TYPE type) noexcept {
    // Check handle
    if (refHandle.pReserved) {
        // Get pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Return base on type
        switch (type) {
            // DISK
            case RX_PAGE_TYPE_DISK:
                return ptrData->diskVersion;

            // CPU
            case RX_PAGE_TYPE_CPU:
                return ptrData->cpuVersion;

            // GPU
            case RX_PAGE_TYPE_GPU:
                return ptrData->gpuVersion;
        }
    }

    // Fallback zero
    return 0;
}

bool engine::RxTools::resourceComparePageVersions(RESOURCE& refHandle, RX_PAGE_TYPE typeA, RX_PAGE_TYPE typeB) noexcept {
    return resourceGetVersion(refHandle, typeA) >= resourceGetVersion(refHandle, typeB);
}

bool engine::RxTools::resourceIncrementVersion(RESOURCE& refHandle, RX_PAGE_TYPE type) noexcept {
    // Check handle
    if (refHandle.pReserved) {
        // Get data pointer
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Switch type
        switch (type) {
            // DISK
            case RX_PAGE_TYPE_DISK:
                ptrData->diskVersion++;
                return true;

            // CPU
            case RX_PAGE_TYPE_CPU:
                ptrData->cpuVersion++;
                return true;

            // GPU
            case RX_PAGE_TYPE_GPU:
                ptrData->gpuVersion++;
                return true;
        }
    }

    // Fallback false
    return false;
}

bool engine::RxTools::resourceCheckPageMask(RESOURCE& refHandle, RX_PAGE_TYPE type, UCHAR mask) noexcept {
    // Check handle
    if (refHandle.pReserved) {
        // Extract data
        __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

        // Empty mask
        UCHAR givenMask = 0x00;

        // Get mask by cpu
        switch (type) {
            // CPU
            case RX_PAGE_TYPE_DISK:
                givenMask = (UCHAR)ptrData->diskPage;
                break;

            // CPU
            case RX_PAGE_TYPE_CPU:
                givenMask = (UCHAR)ptrData->cpuPage;
                break;

            // GPU
            case RX_PAGE_TYPE_GPU:
                givenMask = (UCHAR)ptrData->gpuPage;
                break;
        }

        // Evalue mask
        return (givenMask & mask) == mask;
    }
    
    // Fallback false
    return false;
}

RX_UPDATE_RESULT engine::RxTools::resourcePageUpdate(RESOURCE& refHandle, RX_PAGE_TYPE src, RX_PAGE_TYPE dest, void* ctx) {
    // Catch invalid handle
    if (!refHandle.pReserved) {
        return RX_UPDATE_RESULT_INVALID_HANDLE;
    }

    // Get resource pointer
    __RX_DATA* ptrData = (__RX_DATA*)refHandle.pReserved;

    // Check page
    if (!resourceCheckPageMask(refHandle, src, (UCHAR)RX_PAGE_READ_ONLY) || !resourceCheckPageMask(refHandle, dest, (UCHAR)RX_PAGE_WRITE_ONLY)) {
        return RX_UPDATE_RESULT_INVALID_PAGE;
    }

    // Proc
    RESOURCE_PROC proc = nullptr;

    // Find implementation
    // CPU <--> DISK
    if ( 
        (src == RX_PAGE_TYPE_CPU && dest == RX_PAGE_TYPE_DISK) ||
        (src == RX_PAGE_TYPE_DISK && dest == RX_PAGE_TYPE_CPU)
    ) {
        proc = ptrData->resourceProc_CPU_DISK;
    }
    // CPU <--> GPU
    if ( 
        (src == RX_PAGE_TYPE_CPU && dest == RX_PAGE_TYPE_GPU) ||
        (src == RX_PAGE_TYPE_GPU && dest == RX_PAGE_TYPE_CPU)
    ) {
        proc = ptrData->resourceProc_CPU_GPU;
    }

    // Check proc
    if (proc == nullptr) {
        return RX_UPDATE_RESULT_NO_IMPLEMENTATION;
    }

    // Call implementation
    if (proc(ptrData, src, dest, ctx)) {
        return RX_UPDATE_RESULT_UPDATE_CALL_OK;
    }

    // Default fail
    return RX_UPDATE_RESULT_UPDATE_CALL_FAIL;
}
