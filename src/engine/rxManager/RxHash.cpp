#include "DefHeader.h"
#include "RxHash.h"

bool engine::RxHash::singHandle(RESOURCE& refResource) noexcept {
    // Check pointer
    if (refResource.pReserved) {
        // Get pointer
        __RX_DATA* ptrData = (__RX_DATA*)refResource.pReserved;
        
        // Sign handle
        refResource.refId = XXH64(ptrData->name, wcsnlen_s(ptrData->name, 255) * 2, 0);
        return true;
    }

    // Default return false
    return false;
}
