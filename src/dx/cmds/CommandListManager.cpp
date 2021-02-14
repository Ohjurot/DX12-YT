#include "DefHeader.h"
#include "CommandListManager.h"

// Instance
DX::CommandListManager DX::CommandListManager::s_instance;

void DX::CommandListManager::createInternalObjects(ID3D12Device* ptrDevice) {
    for (unsigned int ti = 0; ti < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; ti++) {
        // Create fence and counter
        XFence ft(ptrDevice);
        m_fences[ti] = ft;
        m_fenceCounters[ti] = ft;
        
        // Retrive type and queu
        const D3D12_COMMAND_LIST_TYPE type = DX::CommandQueueManager::getInstance().typeAtIndex(ti);
        DX::XCommandQueue& queue = DX::CommandQueueManager::getInstance().getCommandQueue(type);

        // Create lists
        for (unsigned int li = 0; li < CLS_DX_CommandListManager__NUM_COMMAND_LISTS_PER_TYPE; li++) {
            m_commandList2DArr[ti][li].commandList.init(ptrDevice, queue, m_fenceCounters[ti]);
        }
    }
}

void DX::CommandListManager::destroyInternalObjects() noexcept {
    for (unsigned int ti = 0; ti < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; ti++) {
        // Release lists
        for (unsigned int li = 0; li < CLS_DX_CommandListManager__NUM_COMMAND_LISTS_PER_TYPE; li++) {
            m_commandList2DArr[ti][li].commandList.release();
        }

        // Release counter and fences
        m_fenceCounters[ti].release();
        m_fences[ti].release();
    }
}

void DX::CommandListManager::refreshInternal() {
    // Assert first
    EXPP_ASSERT(m_commandList2DArr[0][0].commandList, "Invalid call on null object");
    
    // For all lists
    for (unsigned int ti = 0; ti < CLS_DX_CommandQueueManager__NUM_COMMAND_QUEUES; ti++) {
        for (unsigned int li = 0; li < CLS_DX_CommandListManager__NUM_COMMAND_LISTS_PER_TYPE; li++) {
            // Try to lock
            if (m_commandList2DArr[ti][li].locked_flag.test_and_set(std::memory_order_acquire) == 0) {
                // Refresh and unlock
                m_commandList2DArr[ti][li].commandList.refresh();
                m_commandList2DArr[ti][li].locked_flag.clear(std::memory_order_release);
            }
        }
    }
        
}

DX::XCommandList* DX::CommandListManager::lockList(D3D12_COMMAND_LIST_TYPE type) {
    // Get type index
    int typeIndex = DX::CommandQueueManager::getInstance().indexAtType(type);
    EXPP_ASSERT(typeIndex >= 0, "Illegal or Invalid command list type");
    
    // As long as time goes on
    while (true) {
        // Find list
        for (unsigned int i = 0; i < CLS_DX_CommandListManager__NUM_COMMAND_LISTS_PER_TYPE; i++) {
            // Try to lock list
            if (m_commandList2DArr[typeIndex][i].locked_flag.test_and_set(std::memory_order_acquire) == 0) {
                if (m_commandList2DArr[typeIndex][i].commandList.getState() == XCommandList_State::OPEN) {
                    // Return command list pointer
                    return &m_commandList2DArr[typeIndex][i].commandList;
                }

                // Unlock object again
                m_commandList2DArr[typeIndex][i].locked_flag.clear(std::memory_order_release);
            }
        }
    }
}

void DX::CommandListManager::unlockList(XCommandList* ptrList, bool execute) {
    // Assert open state
    EXPP_ASSERT(ptrList->getState() == XCommandList_State::CLOSED || ptrList->getState() == XCommandList_State::OPEN, "Invalid state of command list");

    // Check if execute is required
    if (execute) {
        // Close if required
        if (ptrList->getState() == XCommandList_State::OPEN) {
            ptrList->close();
        }

        // TODO: RESOURCE MANGEMEN RESOLVE EXECUTE

        ptrList->execute();

        // TODO: POST EXECUTE RESOURCE MANAGEMENT 
    }

    // Get type index
    int typeIndex = DX::CommandQueueManager::getInstance().indexAtType(ptrList->ptr()->GetType());
    EXPP_ASSERT(typeIndex >= 0, "Illegal or Invalid command list type");

    // Unlock list
    for (int i = 0; i < CLS_DX_CommandListManager__NUM_COMMAND_LISTS_PER_TYPE; i++) {
        if (&m_commandList2DArr[typeIndex][i].commandList == ptrList) {
            m_commandList2DArr[typeIndex][i].locked_flag.clear(std::memory_order_release);
        }
    }
}

DX::CommandListManager& DX::CommandListManager::getInstance() noexcept {
    return s_instance;
}
