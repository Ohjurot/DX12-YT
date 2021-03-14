#include "DefHeader.h"
#include "CommandQueueManager.h"

// Definition of singelton
DX::CommandQueueManager DX::CommandQueueManager::s_instance;

void DX::CommandQueueManager::createInternalObjects(ID3D12Device* ptrDevice) {
    // Assert pointer
    EXPP_ASSERT(ptrDevice, "Device pointer was null");
    // Destroy existing reference
    destroyInternalObjects();

    // Create all queues
    for (int i = 0; i < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; i++) {
        // Create queue and assign it
        XCommandQueue queue(ptrDevice, mc_arrCommandQueueTypes[i]);
        m_arrCommandQueues[i].operator=(queue);
        
        // Build name
        std::wstringstream wss;
        wss << L"Managed Command Queue #" << i;
        m_arrCommandQueues[i].name(wss.str().c_str());
    }
}

void DX::CommandQueueManager::destroyInternalObjects() noexcept {
    // Release all queues
    for (int i = 0; i < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; i++) {
        m_arrCommandQueues[i].release();
    }
}

DX::XCommandQueue& DX::CommandQueueManager::getCommandQueue(D3D12_COMMAND_LIST_TYPE type) {
    // Find command list
    int idx = indexAtType(type);

    // Assert index and command queue
    EXPP_ASSERT_DBG(idx >= 0, "No command queue for given command list type");
    EXPP_ASSERT(m_arrCommandQueues[idx], "Command queue is not valid");

    // Return list reference
    return m_arrCommandQueues[idx];
}

D3D12_COMMAND_LIST_TYPE DX::CommandQueueManager::typeAtIndex(unsigned int idx) {
    EXPP_ASSERT(idx < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES, "Index out of range");
    
    return mc_arrCommandQueueTypes[idx];
}

int DX::CommandQueueManager::indexAtType(D3D12_COMMAND_LIST_TYPE type) {
    int idx = -1;
    for (int i = 0; i < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; i++) {
        // If type matches set idx to i
        if (mc_arrCommandQueueTypes[i] == type) {
            idx = i;
        }
    }

    return idx;
}

DX::CommandQueueManager& DX::CommandQueueManager::getInstance() {
    // Return instance
    return s_instance;
}
