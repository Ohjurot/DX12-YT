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

// DEBUG
#include <engine/rxManager/RxAllocator.h>
#include <engine/rxManager/Resource.h>
#include <engine/rxManager/RxHash.h>
#include <engine/rxManager/RxTools.h>
// DEBUG

bool resProc(_RX_DATA* ptrData, RX_PAGE_TYPE source, RX_PAGE_TYPE target, void* context) {

	return true;
}

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

		// Init engine
		engine::RxAllocator::init();

		// Create window class
		EasyHWND::WindowClass cls(L"MyWindowCls", CS_OWNDC, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, m_hInstance);
		EXPP_ASSERT(cls, "Window class not valid");

		// Get factory2
		ScopedComPointer<IDXGIFactory2> factory2;
		EXPP_ASSERT(factory.queryInterface(factory2), "IDXGIFactory1->QueryInterface(...) for IDXGIFactory2 failed");

		// Create window
		DX::GfxWindow window(cls, xDevice, factory2, L"DirectX 12", DX::GfxWindow_Stlye::BORDERLESS);
		window.setWindowVisibility(true);

		// === DEBUG

		DX::XHeap heap(xDevice, MEM_MiB(1));

		RESOURCE rx = engine::RxTools::createResource(L"Resource 1234", &resProc, &resProc, RX_PAGE_READ_ONLY, RX_PAGE_WRITE_ONLY, RX_PAGE_UNAVAILIBLE);
		MessageBox(NULL, RESOURCE_NAME(rx), L"Name of Resource", MB_OK);

		D3D12_RESOURCE_DESC buffer;
		ZeroMemory(&buffer, sizeof(D3D12_RESOURCE_DESC));
		buffer.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buffer.Width = sizeof(unsigned char) * 256;
		buffer.Height = 1;
		buffer.DepthOrArraySize = 1;
		buffer.MipLevels = 1;
		buffer.SampleDesc.Count = 1;
		buffer.SampleDesc.Quality = 0;
		buffer.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		engine::RxTools::resourceAllocGpu(rx, xDevice, heap, 0, &buffer);

		RX_UPDATE_RESULT res = engine::RxTools::resourcePageUpdate(rx, RX_PAGE_TYPE_DISK, RX_PAGE_TYPE_CPU);

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

		// === 
		engine::RxTools::resourceFreeGpu(rx);
		heap.release();
		// ===

		// Destroy window
		window.release();
		factory2.release();

		// Shutdown engine
		engine::RxAllocator::shutdown();

		// Release queues and device
		DX::CommandListManager::getInstance().destroyInternalObjects();
		DX::CommandQueueManager::getInstance().destroyInternalObjects();
		xDevice.release();

		// Debug RLO for DXGI
		DEBUG_ONLY_EXECUTE(DX::GIDebug::getInstance().reportLiveObjects());

		return Job::JobReturnValue::JOB_DONE;
	}
};
