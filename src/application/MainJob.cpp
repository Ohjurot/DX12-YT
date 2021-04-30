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
#include <engine/resources/copyProviders/GpuUploadTexturePool.h>
#include <common/Image/WICMeta.h>
#include <common/Image/WICImageProcessor.h>
#include <common/Time/StopWatch.h>

struct Vertex {
	float pos[2];
	float tex[2];
};

struct ConstantBuffer {
	float scale[2];
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

		// Texture
		common::image::WIC_META fileMeta;
		EXPP_ASSERT(common::image::WicIO::open(L"./source/textures/auge/auge_2048_2048_BGR_24BPP.png", fileMeta), "Failed to open texture");

		// CPU Buffer
		size_t imageMemorySize = fileMeta.width * fileMeta.height * ((fileMeta.bpp + 7) / 8);
		void* memoryImage = malloc(imageMemorySize);
		EXPP_ASSERT(memoryImage, "Memory allocation for image failed!");

		// Load texture
		// @mem(memoryImage, UINT8, 4, 2048, 2048, 2048 * 4)
		EXPP_ASSERT(common::image::WICImageProcessor::wicToMemory(fileMeta, memoryImage, imageMemorySize), "Failed to load texture");

		// Create CPU Resource
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Width = fileMeta.width;
		texDesc.Height = fileMeta.height;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = fileMeta.targetGiFormat;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// Create resource
		HEAP_ALLOCATION textureAllocation;
		EXPP_ASSERT(stackHeap.alloc(textureAllocation, DX::XResource::size(xDevice, &texDesc)), "Texture memory allocation failed!");
		DX::XResource texture(xDevice, textureAllocation, &texDesc, nullptr, D3D12_RESOURCE_STATE_COPY_DEST);

		// Desriptor heap for texture
		DX::XDescHeap srvHeap(xDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

		// Textrue descriptor
		D3D12_SHADER_RESOURCE_VIEW_DESC textureView;
		ZeroMemory(&textureView, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		textureView.Format = fileMeta.targetGiFormat;
		textureView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		textureView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		textureView.Texture2D.MipLevels = 1;
		textureView.Texture2D.MostDetailedMip = 0;
		textureView.Texture2D.PlaneSlice = 0;
		textureView.Texture2D.ResourceMinLODClamp = 0.0f;

		xDevice->CreateShaderResourceView(texture, &textureView, srvHeap->GetCPUDescriptorHandleForHeapStart());

		// Close Texture File
		common::image::WicIO::close(fileMeta);

		// Vertex & Index buffer
		engine::rendering::FixedBuffer<Vertex, 3> vertexBuffer(xDevice, &stackHeap);

		vertexBuffer[0].pos[0] = -1.0f;
		vertexBuffer[0].pos[1] = -1.0f;
		vertexBuffer[0].tex[0] =  0.0f;
		vertexBuffer[0].tex[1] =  1.0f;

		vertexBuffer[1].pos[0] =  0.0f;
		vertexBuffer[1].pos[1] =  1.0f;
		vertexBuffer[1].tex[0] =  0.5f;
		vertexBuffer[1].tex[1] =  0.0f;

		vertexBuffer[2].pos[0] =  1.0f;
		vertexBuffer[2].pos[1] = -1.0f;
		vertexBuffer[2].tex[0] =  1.0f;
		vertexBuffer[2].tex[1] =  1.0f;

		engine::rendering::FixedBuffer<UINT32, 3> indexBuffer(xDevice, &stackHeap);
		indexBuffer[0] = 0;
		indexBuffer[1] = 1;
		indexBuffer[2] = 2;

		// Upload
		engine::GpuStackHeap stackHeapUpl(xDevice, MEM_MiB(32), D3D12_HEAP_TYPE_UPLOAD);
		
		// Buffer
		HEAP_ALLOCATION allocUpload;
		EXPP_ASSERT(stackHeapUpl.alloc(allocUpload, MEM_MiB(1)), "Allocation failed");
		engine::GpuUploadRingbuffer rb(xDevice, allocUpload, MEM_MiB(1));

		// Texture
		HEAP_ALLOCATION allocUploadTexture;
		EXPP_ASSERT(stackHeapUpl.alloc(allocUploadTexture, MEM_MiB(4)), "Allocation failed");
		engine::GpuUploadTexturePool texUploader(xDevice, allocUploadTexture, 32, 256);

		// Upload State
		DX::XCommandList::WaitObject woWaitStateS, woWaitStateR, woCopy, woCopy2;
		vertexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_COPY_DEST);
		indexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_COPY_DEST);
		woWaitStateS = lao.executeExchange();

		// Upload execute
		rb.queueUpload(vertexBuffer.res(), vertexBuffer.ptr(), 0, vertexBuffer.size(), woCopy, woWaitStateS);
		rb.queueUpload(indexBuffer.res(), indexBuffer.ptr(), 0, indexBuffer.size(), woCopy, woWaitStateS);
		rb.kickoff();
		
		texUploader.queueUploadTexture(texture, memoryImage, texDesc.Height, texDesc.Width, texDesc.Format, woCopy2, woWaitStateS);
		texUploader.kickoff();

		// Set state
		lao.addDependency(woCopy);
		lao.addDependency(woCopy2);
		
		//
		vertexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		indexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		texture.resourceTransition(lao, D3D12_RESOURCE_STATE_GENERIC_READ);
		lao.executeExchange().wait();

		// Delete local memory
		free(memoryImage);

		// Constant buffer
		engine::rendering::FixedBuffer<ConstantBuffer> constantBuffer(xDevice, &stackHeap);
		constantBuffer[0].scale[0] = 0.5f;
		constantBuffer[0].scale[1] = 0.5f;

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
		vPort.Width = (cr.right - cr.left);
		vPort.Height = (cr.bottom - cr.top);
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
		dx::RootBindings<3> bd = {
			dx::ROOT_ELEMENT<DX_ROOT_TYPE_CONSTANT>(sizeof(float) * 4, flt),
			dx::ROOT_ELEMENT<DX_ROOT_TYPE_CBV>(constantBuffer.res()->GetGPUVirtualAddress()),
			dx::ROOT_ELEMENT<DX_ROOT_TYPE_TABLE>(srvHeap->GetGPUDescriptorHandleForHeapStart()),
		};
		
		// TEMP

		// Frame timer
		Time::StopWatch frameTimer;
		frameTimer.start();

		// Window job
		while (window) {
			float deltaTMs = frameTimer.deltaUs() / 1000.0f;
			frameTimer.reset();

			// === LOGIC ===
			if (GetAsyncKeyState(VK_LEFT)) {
				constantBuffer[0].scale[0] = std::min<float>(constantBuffer[0].scale[0] + 0.2f * (deltaTMs / 1000.0f), 1.0f);
			}
			if (GetAsyncKeyState(VK_RIGHT)) {
				constantBuffer[0].scale[0] = std::max<float>(constantBuffer[0].scale[0] - 0.2f * (deltaTMs / 1000.0f), 0);
			}
			if (GetAsyncKeyState(VK_UP)) {
				constantBuffer[0].scale[1] = std::min<float>(constantBuffer[0].scale[1] + 0.2f * (deltaTMs / 1000.0f), 1.0f);
			}
			if (GetAsyncKeyState(VK_DOWN)) {
				constantBuffer[0].scale[1] = std::max<float>(constantBuffer[0].scale[1] - 0.2f * (deltaTMs / 1000.0f), 0);
			}

			// Upadate const buffer to gpu
			if (constantBuffer.hasChanges()) {
				// Copy dest
				constantBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_COPY_DEST);

				// IO / Wait object
				DX::XCommandList::WaitObject wo;
				rb.queueUpload(constantBuffer.res(), constantBuffer.ptr(), 0, constantBuffer.size(), wo, lao.createWaitObject());

				// Lao execute
				lao.executeExchange();

				// RB Kickoff
				rb.kickoff();

				// Lao back transition
				constantBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
				lao.addDependency(wo);
			}


			// === RENDERING ===
			// Begin frame
			window.beginFrame(lao);

			// TEMP
			// Bind PSO
			state.bind(lao);
			lao->SetDescriptorHeaps(1, srvHeap.to());
			bd.bind(lao);

			lao->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			lao->IASetVertexBuffers(0, 1, ptrVBView);
			lao->IASetIndexBuffer(ptrIBView);

			lao->RSSetViewports(1, &vPort);
			lao->RSSetScissorRects(1, &scRect);

			lao->DrawIndexedInstanced(3, 1, 0, 0, 0);

			// TEMP

			// End frame (present)
			window.endFrame(lao);
			lao.executeExchange().wait();
			window.present();
		}
		lao.release();

		// TEMP 
		constantBuffer.release();
		srvHeap.release();
		state.release();
		indexBuffer.release();
		vertexBuffer.release();
		texture.release();
		rb.release();
		texUploader.release();
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
