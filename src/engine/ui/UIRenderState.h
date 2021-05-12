#pragma once

#include <DefHeader.h>

#include <dx/pso/PsoState.h>
#include <dx/pso/PsoFile.h>
#include <dx/descriptors/XDescHeap.h>
#include <dx/descriptors/RootBindings.h>

#include <engine/resources/copyProviders/GpuUploadStackBuffer.h>
#include <engine/resources/heapTools/GpuStackHeap.h>
#include <engine/rendering/RenderingTypes.h>
#include <engine/rendering/buffer/FixedBuffer.h>

namespace engine {
	namespace ui {
		/// <summary>
		/// Represents an UI Vertex
		/// </summary>
		struct UIRenderState_vertex {
			/// <summary>
			/// Position of ui quad tile
			/// </summary>
			Vec2<float> pos;

			/// <summary>
			/// Size of ui quad tile
			/// </summary>
			Vec2<float> size;

			/// <summary>
			/// Index to anchor for this ui quad tile
			/// </summary>
			UINT32 anchor;
		};

		/// <summary>
		/// Represents an UI Anchor
		/// </summary>
		struct UIRenderState_anchor {
			/// <summary>
			/// Anchor parts
			/// </summary>
			Vec4<float> mulPart, addPart;
		};

		/// <summary>
		/// Draw command descrition
		/// </summary>
		struct UIRenderState_drawcmd {
			/// <summary>
			/// Vertex offset
			/// </summary>
			UINT vertexOffset;

			/// <summary>
			/// Vertecies count
			/// </summary>
			UINT vertexCount;
			
			/// <summary>
			/// Sisore rect
			/// </summary>
			RECT sicRect;
		};

		/// <summary>
		/// Render state of the UI
		/// </summary>
		class UIRenderState {
			public:
				/// <summary>
				/// Load and compile UI Render State
				/// </summary>
				/// <param name="width">Window width</param>
				/// <param name="height">Window height</param>
				UIRenderState(ID3D12Device* ptrDevice, UINT width, UINT height);

				/// <summary>
				/// Destructor
				/// </summary>
				~UIRenderState();

				/// <summary>
				/// Release function
				/// </summary>
				void release();

				/// <summary>
				/// Create an anchor
				/// </summary>
				/// <returns>Index of anchor</returns>
				UINT createAnchor();

				/// <summary>
				/// Drawing set rect
				/// </summary>
				/// <param name="rect">Rect pointer or nullptr</param>
				void draw_setRect(RECT* rect);

				/// <summary>
				/// Add a vertex to render commands
				/// </summary>
				/// <param name="refVertex">Vertex</param>
				void draw_addVertex(UIRenderState_vertex& refVertex);

				/// <summary>
				/// Bind the UI render state
				/// </summary>
				/// <param name="ptrCommandList">DirectX command list</param>
				void bind(ID3D12GraphicsCommandList* ptrCommandList);

				/// <summary>
				/// GPU Update of the ui state
				/// </summary>
				/// <param name="refUploadBuffer">Upload buffer</param>
				void update(engine::GpuUploadStackBuffer& refUploadBuffer);

				/// <summary>
				/// Add render calls to command list
				/// </summary>
				/// <param name="ptrCommandList">Command list to queue render on</param>
				void render(ID3D12GraphicsCommandList* ptrCommandList);

			private:
				/// <summary>
				/// Inserts the root command
				/// </summary>
				void __insertRootCommand();

			private:
				/// <summary>
				/// Swap chain dimensions
				/// </summary>
				UINT m_width, m_height;

				/// <summary>
				/// Pipeline State
				/// </summary>
				dx::PsoState m_pso;

				/// <summary>
				/// ResourceDescritor heap
				/// </summary>
				DX::XDescHeap m_descHeapSrvCbv;

				/// <summary>
				/// Heap for UI
				/// </summary>
				engine::GpuStackHeap m_heap;

				/// <summary>
				/// Drawcommand buffer
				/// </summary>
				UIRenderState_drawcmd m_drawCommands[128];

				/// <summary>
				/// Draw commands count
				/// </summary>
				UINT m_drawCommandsCount = 0;

				/// <summary>
				/// Vertex count
				/// </summary>
				UINT m_drawVertexCount = 0;

				/// <summary>
				/// Anchors count
				/// </summary>
				UINT m_anchorCount = 0;

				/// <summary>
				/// UI Vertex buffer
				/// </summary>
				engine::rendering::FixedBuffer<UIRenderState_vertex, 2048> m_vertexBuffer;

				/// <summary>
				/// Anchor buffer
				/// </summary>
				engine::rendering::FixedBuffer<UIRenderState_anchor, 32> m_anchorBuffer;
		};
	}
}
