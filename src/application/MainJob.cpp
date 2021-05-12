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

#include <engine/ui/UIRenderState.h>

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

		// Get window dimensions
		RECT cr;
		GetClientRect((HWND)window, &cr);
		UINT width = cr.right - cr.left;
		UINT height = cr.bottom - cr.top;

		// DEBUG UI
		engine::GpuStackHeap uplHeap(xDevice, MEM_MiB(64), D3D12_HEAP_TYPE_UPLOAD);
		HEAP_ALLOCATION upAlloc;
		uplHeap.alloc(upAlloc, MEM_MiB(64));
		engine::GpuUploadStackBuffer uploadStack(xDevice, upAlloc, MEM_MiB(64));

		engine::ui::UIRenderState* uiState = new engine::ui::UIRenderState(xDevice, width, height);
		const UINT anchor = uiState->createAnchor();

		// DEBUG UI

		// LAO
		DX::CommandListAccessObject lao(D3D12_COMMAND_LIST_TYPE_DIRECT);

		// Window job
		while (window) {
			// Begin frame
			window.beginFrame(lao);
			
			// UPDAT
			engine::ui::UIRenderState_vertex vtx;
			vtx.anchor = anchor;

			vtx.pos = { -1.0f, 1.0f };
			vtx.size = { 1.0f, -1.0f };
			uiState->draw_addVertex(vtx);

			vtx.pos = { 0.0f, 0.0f };
			vtx.size = { 0.5f, -0.5f };
			uiState->draw_addVertex(vtx);

			vtx.pos = { 0.5f, -0.5f };
			vtx.size = { 0.25f, -0.25f };
			uiState->draw_addVertex(vtx);

			vtx.pos = { 0.75f, -0.75f };
			vtx.size = { 0.125f, -0.125f };
			uiState->draw_addVertex(vtx);

			vtx.pos = { 0.875f, -0.875f };
			vtx.size = { 0.0625f, -0.0625f };
			uiState->draw_addVertex(vtx);
			
			vtx.pos = { 0.9375f, -0.9375f };
			vtx.size = { 0.03125f, -0.03125f };
			uiState->draw_addVertex(vtx);

			vtx.pos = { 0.96875f, -0.96875f };
			vtx.size = { 0.016125f, -0.016125f };
			uiState->draw_addVertex(vtx);

			vtx.pos = { 0.984875f, -0.984875f };
			vtx.size = { 0.0080625f, -0.0080625f };
			uiState->draw_addVertex(vtx);

			vtx.pos = { 0.9929375f, -0.9929375f };
			vtx.size = { 0.00403125f, -0.00403125f };
			uiState->draw_addVertex(vtx);

			uiState->update(uploadStack);
			auto uplWo = uploadStack.execute();

			// RENDER
			lao.addDependency(uplWo);
			uiState->render(lao);

			// End frame render
			window.endFrame(lao);
			lao.executeExchange().wait();

			// Dispatch
			window.present();
		}

		// Destroy lao
		lao.executeClose();

		// DBG
		uploadStack.release();
		uplHeap.release();
		uiState->release();
		delete uiState;
		// DBG

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
