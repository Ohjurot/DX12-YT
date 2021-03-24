#include <DefHeader.h>
#include <job/system/Job.h>

#ifdef _DEBUG
#include <dx/core/XDebug.h>
#include <dx/setup/GIDebug.h>
#endif

#include <dx/setup/GIFactory.h>
#include <dx/core/XDevice.h>
#include <dx/window/GfxWindow.h>
#include <dx/cmds/CommandQueueManager.h>
#include <dx/cmds/CommandListManager.h>
#include <dx/cmds/CommandListAccessObject.h>

//
#include <engine/resources/copyProviders/GpuUploadTexturePool.h>
#include <engine/resources/HeapTools/GpuStackHeap.h>
#include <application/jobs/loading/ImageLoadJob.h>
#include <common/Image/WICMeta.h>
#include <common/Image/WICImageProcessor.h>
//

MAIN_JOB(ytDirectXMain) {
	JOB_EXECUTE_FUNCTION(unsigned int index) {
		// Enable and validate debug interface
		DEBUG_ONLY_EXECUTE(DX::XDebug::getInstance().enable());
		DEBUG_ONLY_EXECUTE(DX::GIDebug::getInstance().validate());

		// Create Factory and adapter
		DX::GIFactory factory;
		DX::GIAdapter adapter = factory.getAdapter();

		// Create XDevice
		DX::XDevice xDevice(adapter);
		xDevice.name(L"Main Device");

		// Init command queues and lists
		DX::CommandQueueManager::getInstance().createInternalObjects(xDevice);
		DX::CommandListManager::getInstance().createInternalObjects(xDevice);

		// Create window class
		EasyHWND::WindowClass cls(L"MyWindowCls", CS_OWNDC, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, m_hInstance);
		EXPP_ASSERT(cls, "Window class not valid");

		// Get factory2
		ScopedComPointer<IDXGIFactory2> factory2;
		EXPP_ASSERT(factory.queryInterface(factory2), "IDXGIFactory1->QueryInterface(...) for IDXGIFactory2 failed");

		// Create window
		DX::GfxWindow window(cls, xDevice, factory2, L"DirectX 12", DX::GfxWindow_Stlye::BORDERLESS);
		window.setWindowVisibility(true);

		// ===
		// Production heap
		engine::GpuStackHeap m_heap(xDevice, MEM_MiB(64));
		m_heap.name(L"Production heap");

		// Heap UPL
		engine::GpuStackHeap m_uHeap(xDevice, MEM_MiB(8), D3D12_HEAP_TYPE_UPLOAD);
		m_uHeap.name(L"Upload heap");

		// WIC Load
		common::image::WIC_META imageMeta;
		EXPP_ASSERT(common::image::WicIO::open(L"./test.png", imageMeta), "common::image::WicIO::open(...)");

		// DX Texture
		D3D12_RESOURCE_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = imageMeta.width;
		desc.Height = imageMeta.height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = imageMeta.targetGiFormat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		HEAP_ALLOCATION textureAllocation;
		EXPP_ASSERT(m_heap.alloc(textureAllocation, DX::XResource::size(xDevice, &desc)), "Allocation failed");
		DX::XResource m_res(xDevice, textureAllocation, &desc, nullptr, D3D12_RESOURCE_STATE_COPY_DEST);
		m_res.name(L"Texture");

		// Upload pool
		HEAP_ALLOCATION uploadAllocation;
		EXPP_ASSERT(m_uHeap.alloc(uploadAllocation, MEM_MiB(1)), "Allocation failed");
		engine::GpuUploadTexturePool pool(xDevice, uploadAllocation);
		pool.name(L"Texture upload pool");
		
		// Texture load & copy
		app::ImageLoadJob load(imageMeta, pool, m_res);
		Job::prepare(load).stageExecution().wait();

		// Close image
		common::image::WicIO::close(imageMeta);

		// ===

		// AO
		DX::CommandListAccessObject lao(D3D12_COMMAND_LIST_TYPE_DIRECT);

		// Window job
		while (window) {
			window.beginFrame(lao);
			window.endFrame(lao);
			lao.executeExchange().wait();

			window.present();
		}
		lao.release();

		//
		pool.release();
		m_res.release();
		m_heap.release();
		m_uHeap.release();
		//

		// Destroy window
		window.release();
		factory2.release();

		// Release queues and device
		DX::CommandListManager::getInstance().destroyInternalObjects();
		DX::CommandQueueManager::getInstance().destroyInternalObjects();
		xDevice.release();

		// Debug RLO for DXGI
		DEBUG_ONLY_EXECUTE(DX::GIDebug::getInstance().reportLiveObjects());
		return Job::JobReturnValue::JOB_DONE;
	}
};
