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

#include <dx/pso/PsoFile.h>
#include <dx/descriptors/XDescHeap.h>
#include <dx/descriptors/RootBindings.h>
#include <engine/rendering/buffer/FixedBuffer.h>
#include <engine/resources/copyProviders/GpuUploadRingbuffer.h>

struct Vertex {
	float pos[2];
	float color[3];
};

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

		// AO
		DX::CommandListAccessObject lao(D3D12_COMMAND_LIST_TYPE_DIRECT);

		// TEMP

		// Heap
		engine::GpuStackHeap stackHeap(xDevice, MEM_MiB(32));

		// Vertex & Index buffer
		engine::rendering::FixedBuffer<Vertex, 3> vertexBuffer(xDevice, &stackHeap);

		vertexBuffer[0].pos[0] = -1.0f;
		vertexBuffer[0].pos[1] = -1.0f;
		vertexBuffer[0].color[0] = 1.0f;
		vertexBuffer[0].color[1] = 0.0f;
		vertexBuffer[0].color[2] = 0.0f;

		vertexBuffer[1].pos[0] =  0.0f;
		vertexBuffer[1].pos[1] =  1.0f;
		vertexBuffer[1].color[0] = 0.0f;
		vertexBuffer[1].color[1] = 1.0f;
		vertexBuffer[1].color[2] = 0.0f;

		vertexBuffer[2].pos[0] =  1.0f;
		vertexBuffer[2].pos[1] = -1.0f;
		vertexBuffer[2].color[0] = 0.0f;
		vertexBuffer[2].color[1] = 0.0f;
		vertexBuffer[2].color[2] = 1.0f;

		engine::rendering::FixedBuffer<UINT32, 3> indexBuffer(xDevice, &stackHeap);
		indexBuffer[0] = 0;
		indexBuffer[1] = 1;
		indexBuffer[2] = 2;

		// Upload
		engine::GpuStackHeap stackHeapUpl(xDevice, MEM_MiB(32), D3D12_HEAP_TYPE_UPLOAD);
		HEAP_ALLOCATION allocUpload;
		EXPP_ASSERT(stackHeapUpl.alloc(allocUpload, MEM_MiB(1)), "Allocation failed");
		engine::GpuUploadRingbuffer rb(xDevice, allocUpload, MEM_MiB(1));

		// Upload State
		DX::XCommandList::WaitObject woWaitStateS, woWaitStateR, woCopy;
		vertexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_COPY_DEST);
		indexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_COPY_DEST);
		woWaitStateS = lao.executeExchange();

		// Upload execute
		rb.queueUpload(vertexBuffer.res(), vertexBuffer.ptr(), 0, vertexBuffer.size(), woCopy, woWaitStateS);
		rb.queueUpload(indexBuffer.res(), indexBuffer.ptr(), 0, indexBuffer.size(), woCopy, woWaitStateS);
		rb.kickoff();

		// Set state
		lao.addDependency(woCopy);
		
		//
		vertexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		indexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		lao.executeExchange().wait();

		// Views
		D3D12_VERTEX_BUFFER_VIEW* ptrVBView = vertexBuffer.createVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW* ptrIBView = indexBuffer.createIndexBufferView();

		// Rasterizer
		RECT cr;
		EXPP_ASSERT(GetClientRect((HWND)window, &cr), "Failed to get window dimensions");

		D3D12_RECT scRect;
		D3D12_VIEWPORT vPort;

		vPort.TopLeftX = 0;
		vPort.TopLeftY = 0;
		vPort.Width = cr.right - cr.left;
		vPort.Height = cr.bottom - cr.top;
		vPort.MinDepth = 0.0f;
		vPort.MaxDepth = 1.0f;

		scRect.left = 0;
		scRect.right = vPort.Width;
		scRect.top = 0;
		scRect.bottom = vPort.Height;

		// PSO
		dx::PsoState state;
		EXPP_ASSERT(dx::PsoFile::read(L"./source/pstates/demo.json", state), "Failed to read pipeline state");
		EXPP_ASSERT(state.compile(xDevice), "Failed to compile PSO");

		float flt[] = {1.0f, 1.0f, 1.0f, 1.0f};
		dx::RootBindings<1> bd = {
			dx::ROOT_ELEMENT<DX_ROOT_TYPE_CONSTANT>(sizeof(float) * 4, flt)
		};
		
		// TEMP

		// Window job
		while (window) {
			// Begin frame
			window.beginFrame(lao);

			// TEMP
			// Bind PSO
			state.bind(lao);
			bd.bind(lao);

			lao->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			lao->IASetVertexBuffers(0, 1, ptrVBView);
			lao->IASetIndexBuffer(ptrIBView);

			lao->RSSetScissorRects(1, &scRect);
			lao->RSSetViewports(1, &vPort);

			lao->DrawIndexedInstanced(3, 1, 0, 0, 0);

			// TEMP

			// End frame (present)
			window.endFrame(lao);
			lao.executeExchange().wait();
			window.present();
		}
		lao.release();

		// TEMP 
		state.release();
		indexBuffer.release();
		vertexBuffer.release();
		rb.release();
		stackHeapUpl.release();
		stackHeap.release();
		// TEMP

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
