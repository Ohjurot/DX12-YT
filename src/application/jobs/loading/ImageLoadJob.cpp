#include "DefHeader.h"
#include "ImageLoadJob.h"

app::ImageLoadJob::ImageLoadJob(common::image::WIC_META& refMeta, engine::GpuUploadTexturePool& refPool, ID3D12Resource* ptrResource) noexcept :
    m_ptrMeta(&refMeta),
    m_ptrPool(&refPool),
    m_ptrResource(ptrResource)
{
    // ...
}

app::ImageLoadJob::ImageLoadJob(const ImageLoadJob& other) noexcept :
    m_ptrMeta(other.m_ptrMeta),
    m_ptrPool(other.m_ptrPool),
    m_ptrResource(other.m_ptrResource)
{ 
    // ...
}

app::ImageLoadJob& app::ImageLoadJob::operator=(const ImageLoadJob& other) noexcept {
    // Assign
    m_ptrMeta = other.m_ptrMeta;
    m_ptrPool = other.m_ptrPool;
    m_ptrResource = other.m_ptrResource;

    return *this;
}

Job::JobPrepareDesc app::ImageLoadJob::prepare(void* args) {
    // Desribe job
    Job::JobPrepareDesc desc;
    desc.priority = Job::JobPriority::NORMAL;
    desc.numThreads = m_ptrMeta ? 1 : 0;
    
    // Return description
    return desc;
}

Job::JobReturnValue app::ImageLoadJob::execute(unsigned int index) {
    // Allocate memory
    UINT memorySize = m_ptrMeta->stride * m_ptrMeta->height;
    void* texturePointer = malloc(memorySize);
    
    // Load wic image from DISK (to CPU)
    if (!common::image::WICImageProcessor::wicToMemory(*m_ptrMeta, texturePointer, memorySize)) {
        free(texturePointer);
        EXPP_ASSERT(false, "common::image::WICImageProcessor::wicToMemory");
    }

    // Upload to gpu
    DX::XCommandList::WaitObject wo;
    m_ptrPool->queueUploadTexture(m_ptrResource, texturePointer, m_ptrMeta->height, m_ptrMeta->width, m_ptrMeta->targetGiFormat, wo);

    // Wait 
    m_ptrPool->kickoff(); // May be done externaly in the future
    wo.wait();

    // Done
    free(texturePointer);
    return JOB_RETURN_DONE;
}
