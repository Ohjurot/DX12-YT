#include "DefHeader.h"
#include "UIRenderState.h"

engine::ui::UIRenderState::UIRenderState(ID3D12Device* ptrDevice, UINT width, UINT height) :
	// 1x CBV + 32x SRV
	m_descHeapSrvCbv(ptrDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1 + 32, true),
	m_heap(ptrDevice, MEM_MiB(512)),

	// Dimensions
	m_width(width),
	m_height(height)
{
	// Read and compile pso
	EXPP_ASSERT(dx::PsoFile::read(L"./source/pstates/ui.json", m_pso), "Failed to read pipeline state");
	EXPP_ASSERT(m_pso.compile(ptrDevice), "Failed to compile PSO");

	// Create vertex buffer
	m_vertexBuffer.init(ptrDevice, &m_heap);
	m_anchorBuffer.init(ptrDevice, &m_heap);

	// Create cbv
	ptrDevice->CreateConstantBufferView(m_anchorBuffer.createConstantBufferView(), m_descHeapSrvCbv.get(0).cpuHandle);

	// Name objects
	m_descHeapSrvCbv.name(L"UI Descriptor Heap");
	m_pso.namePSO(L"UI Pipeline State");
	m_pso.nameRS(L"UI Rootsignature");
	m_heap.name(L"UI Resource Heap");
	m_vertexBuffer.res().name(L"UI Quad Vertex Buffer");
	m_anchorBuffer.res().name(L"UI Anchor Buffer");
}

engine::ui::UIRenderState::~UIRenderState() {
	release();
}

void engine::ui::UIRenderState::release() {
	// Release memebers in order
	m_anchorBuffer.release();
	m_vertexBuffer.release();
	m_pso.release();
	m_descHeapSrvCbv.release();
	m_heap.release();
}

UINT engine::ui::UIRenderState::createAnchor() {
	// Get anchor index
	UINT anchorIndex = m_anchorCount++;
	
	// Create anchor
	m_anchorBuffer[anchorIndex].mulPart = { 1.0f, 1.0f, 1.0f, 1.0f};
	m_anchorBuffer[anchorIndex].addPart = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Return index
	return anchorIndex;
}

void engine::ui::UIRenderState::draw_setRect(RECT* rect) {
	// Check for root command
	if (rect == nullptr) {
		__insertRootCommand();
	}
	//  Insert proper node
	else {
		// Get element
		UIRenderState_drawcmd& element = m_drawCommands[m_drawCommandsCount++];
		element.vertexCount = 0;
		element.sicRect = *rect;

		// From old
		if (m_drawCommandsCount - 1 > 0) {
			element.vertexOffset = m_drawCommands[m_drawCommandsCount - 2].vertexOffset + m_drawCommands[m_drawCommandsCount - 2].vertexCount;
		}
		// Is first command
		else {
			element.vertexOffset = 0;
		}
	}
}

void engine::ui::UIRenderState::draw_addVertex(UIRenderState_vertex& refVertex) {
	// Insert first root commands
	if (m_drawCommandsCount == 0) {
		__insertRootCommand();
	}

	// Set vertex (Index from current offset + current count post incrment)
	m_vertexBuffer[m_drawCommands[m_drawCommandsCount - 1].vertexOffset + m_drawCommands[m_drawCommandsCount - 1].vertexCount++] = refVertex;
}

void engine::ui::UIRenderState::bind(ID3D12GraphicsCommandList* ptrCommandList) {
	// Root bindings
	static dx::RootBindings<1> rootBindings = {
		dx::ROOT_ELEMENT<DX_ROOT_TYPE_TABLE>(m_descHeapSrvCbv.get(0).gpuHandle),
	};

	// Bind static
	static ID3D12DescriptorHeap* heaps[] = {
		m_descHeapSrvCbv
	};
	m_pso.bind(ptrCommandList);
	ptrCommandList->SetDescriptorHeaps(1, heaps);
	rootBindings.bind(ptrCommandList);

	// Set IA
	static auto ptrVertexBufferView = m_vertexBuffer.createVertexBufferView();
	ptrCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	ptrCommandList->IASetVertexBuffers(0, 1, ptrVertexBufferView);

}

void engine::ui::UIRenderState::update(engine::GpuUploadStackBuffer& refUploadBuffer) {
	// Get command list
	DX::CommandListAccessObject lao(D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Anchor
	if(m_anchorBuffer.hasChanges()){
		m_anchorBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_COPY_DEST);
		EXPP_ASSERT(refUploadBuffer.queueCopy(m_anchorBuffer.res(), 0, m_anchorBuffer.ptr(), m_anchorBuffer.size()), "Anchor buffer copy failed");
	}

	// Vertex
	if (m_vertexBuffer.hasChanges()) {
		m_vertexBuffer.res().resourceTransition(lao, D3D12_RESOURCE_STATE_COPY_DEST);
		EXPP_ASSERT(refUploadBuffer.queueCopy(m_vertexBuffer.res(), 0, m_vertexBuffer.ptr(), m_vertexBuffer.size()), "Vertex buffer copy failed");
	}

	// Close and execute lao
	refUploadBuffer.addDependency(lao.executeClose());
}

void engine::ui::UIRenderState::render(ID3D12GraphicsCommandList* ptrCommandList) {
	// Bind
	bind(ptrCommandList);

	// Resource state
	m_vertexBuffer.res().resourceTransition(ptrCommandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	m_anchorBuffer.res().resourceTransition(ptrCommandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	// Set viewport
	const D3D12_VIEWPORT vp = { 0, 0, m_width, m_height, 1.0f, 0.0f };
	const RECT sicRect = { 0, 0, m_width, m_height };
	ptrCommandList->RSSetViewports(1, &vp);
	
	// Render each command
	for (unsigned int i = 0; i < m_drawCommandsCount; i++) {
		// Set rect
		if (m_drawCommands[i].sicRect.right != 0) {
			ptrCommandList->RSSetScissorRects(1, &m_drawCommands[i].sicRect);
		}
		else {
			ptrCommandList->RSSetScissorRects(1, &sicRect);
		}

		// Draw
		ptrCommandList->DrawInstanced(m_drawCommands[i].vertexCount, 1, m_drawCommands[i].vertexOffset, 0);
	}

	// Clear commands and vertices
	m_drawCommandsCount = 0;
	m_drawVertexCount = 0;
}

void engine::ui::UIRenderState::__insertRootCommand() {
	// Get element
	UIRenderState_drawcmd& element = m_drawCommands[m_drawCommandsCount++];
	
	// Auto rect
	element.sicRect = { 0,0,0,0 };
	
	// Currently no verticies
	element.vertexCount = 0;
	
	// From old
	if (m_drawCommandsCount - 1 > 0) {
		element.vertexOffset = m_drawCommands[m_drawCommandsCount - 2].vertexOffset + m_drawCommands[m_drawCommandsCount - 2].vertexCount;
	}
	// Is first root command
	else {
		element.vertexOffset = 0;
	}
}
