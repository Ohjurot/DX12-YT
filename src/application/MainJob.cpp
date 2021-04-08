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

#include <dx/shader/Shader.h>
#include <json.h>

MAIN_JOB(ytDirectXMain) {
	JOB_EXECUTE_FUNCTION(unsigned int index) {
		dx::Shader s;
		s = std::move(dx::Shader(L"test1234", SHADER_TYPE_VERTEX));

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
