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

// DEBUG
#include <dx/cmds/CommandListAccessObject.h>
#include <dx/memory/XHeap.h>
#include <dx/memory/XResource.h>
// DEBUG

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

		// === DEBUG HEAP

		// PRODUCTION
		DX::XHeap heap = DX::XHeap(xDevice, MEM_MiB(1));
		heap.name(L"Production Heap");
		DX::XFence::WaitObject woHeap;
		EVALUATE_HRESULT(heap.makeResident(woHeap), "DX::XHeap::makeResident(...)");
		woHeap.wait();

		// UPLOAD
		DX::XHeap uHeap = DX::XHeap(xDevice, MEM_MiB(1), D3D12_HEAP_TYPE_UPLOAD);
		uHeap.name(L"Upload Heap");
		EVALUATE_HRESULT(uHeap.makeResident(woHeap), "DX::XHeap::makeResident(...)");
		woHeap.wait();

		// RESOURCE DEF
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = sizeof(unsigned char) * 256;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// RESOURCE CPU / GPU
		DX::XResource resCpu = DX::XResource(xDevice, uHeap, 0, &desc, nullptr);
		resCpu.name(L"CPU Buffer");
		DX::XResource resGpu = DX::XResource(xDevice, heap, 0, &desc, nullptr, D3D12_RESOURCE_STATE_COPY_DEST);
		resGpu.name(L"GPU Buffer");

		// Fill cpu
		unsigned char* pChar;
		EVALUATE_HRESULT(resCpu->Map(0, nullptr, (void**)&pChar), "MAP");
		for (unsigned int i = 0; i < 256; i++) pChar[i] = i;
		resCpu->Unmap(0, nullptr);

		// Copy
		DX::CommandListAccessObject laoCopy(D3D12_COMMAND_LIST_TYPE_COPY);
		laoCopy->CopyResource(resGpu, resCpu);
		laoCopy.executeClose().wait();

		EVALUATE_HRESULT(resCpu->Map(0, nullptr, (void**)&pChar), "MAP");
		ZeroMemory(pChar, sizeof(unsigned int) * 256);
		resCpu->Unmap(0, nullptr);

		// ===

		// AO
		DX::CommandListAccessObject lao(D3D12_COMMAND_LIST_TYPE_DIRECT);
		resGpu.resourceTransition(lao, D3D12_RESOURCE_STATE_GENERIC_READ);

		// Window job
		while (window) {
			window.beginFrame(lao);
			window.endFrame(lao);
			lao.executeExchange().wait();

			window.present();
		}
		lao.release();

		resCpu.release();
		resGpu.release();
		heap.release();
		uHeap.release();

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
