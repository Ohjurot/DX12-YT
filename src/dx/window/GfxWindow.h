#pragma once
#include <DefHeader.h>

#include <dx/setup/GIFactory.h>

#include <dx/cmds/XFence.h>
#include <dx/cmds/FenceCounter.h>
#include <dx/cmds/CommandQueueManager.h>

#include <dx/window/GISwapChain.h>

namespace DX {
	/// <summary>
	/// Window style
	/// </summary>
	enum class GfxWindow_Stlye : DWORD {
		/// <summary>
		/// The window is a normal window
		/// </summary>
		WINDOWED = WS_OVERLAPPEDWINDOW,

		/// <summary>
		/// The window is a borderless fullscreen window
		/// </summary>
		BORDERLESS = WS_POPUP,
	};

	/// <summary>
	/// Graffics window
	/// </summary>
	class GfxWindow : public EasyHWND::Window{
		public:
			/// <summary>
			/// Create window
			/// </summary>
			/// <param name="windowClass">Class of the window</param>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="ptrFactory">Factory2 Pointer</param>
			/// <param name="windowTitle">Title of the window</param>
			/// <param name="style">Window style</param>
			/// <param name="monitor">Monitor to create window on</param>
			GfxWindow(EasyHWND::WindowClass& windowClass, ID3D12Device* ptrDevice, IDXGIFactory2* ptrFactory, LPCWSTR windowTitle, GfxWindow_Stlye style = GfxWindow_Stlye::WINDOWED, HMONITOR monitor = NULL);

			/// <summary>
			/// Destructor
			/// </summary>
			~GfxWindow();

			/// <summary>
			/// Releases all DX types
			/// </summary>
			void release();

			/// <summary>
			/// Flushs all pending operation on the GPU
			/// </summary>
			void flushGpu();

			/// <summary>
			/// Check if window is visible
			/// </summary>
			/// <returns>True is window is visable</returns>
			bool isVisble() noexcept;

			/// <summary>
			/// Presents a frame
			/// </summary>
			/// <param name="vsync">Indicates if VSYNC should be used</param>
			void present(bool vsync = false);

			/// <summary>
			/// Begin a frame on the window
			/// </summary>
			/// <param name="ptrGfxCmdList">Pointer to command list</param>
			void beginFrame(ID3D12GraphicsCommandList* ptrGfxCmdList);

			/// <summary>
			/// End a frame on the window
			/// </summary>
			/// <param name="ptrGfxCmdList">Pointer to command list</param>
			void endFrame(ID3D12GraphicsCommandList* ptrGfxCmdList);

			/// <summary>
			/// Window loop operator
			/// </summary>
			/// <returns>True if window is not closed</returns>
			operator bool();

			// Delete unspported
			GfxWindow(const GfxWindow&) = delete;
			void operator=(const GfxWindow&) = delete;
		protected:
			/// <summary>
			/// Window messages
			/// </summary>
			/// <param name="ptrLRESULT">Pointer to LRESULT set when return true</param>
			/// <param name="hwnd">Window handle</param>
			/// <param name="msg">Message code</param>
			/// <param name="wParam">Message parameter</param>
			/// <param name="lParam">Message parameter</param>
			/// <returns>True if this handled handled the message</returns>
			virtual bool handleWindowMessage(LRESULT* ptrLRESULT, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		private:
			/// <summary>
			/// Swap Chain
			/// </summary>
			GISwapChain m_swapChain;

			/// <summary>
			/// Command queue reference
			/// </summary>
			XCommandQueue& m_queue;

			/// <summary>
			/// Fence for the swap chain
			/// </summary>
			XFence m_fence;

			/// <summary>
			/// Windows fence counter
			/// </summary>
			FenceCounter m_fenceCounter;

			/// <summary>
			/// Fence counter frontend
			/// </summary>
			FenceCounter::Frontend m_fenceCounterFrontend;

			/// <summary>
			/// Indicates weather or not the class has been released
			/// </summary>
			bool m_bDXValid = true;

			/// <summary>
			/// Indicates weather the window is visible or not
			/// </summary>
			bool m_bVisible = false;

			/// <summary>
			/// Boolean indicating weather the swap chain requires resizing
			/// </summary>
			bool m_bNeedsResizing = false;

			/// <summary>
			/// Width / Height of resized window
			/// </summary>
			UINT m_uiNewWidth, m_uiNewHeight;
	};
}