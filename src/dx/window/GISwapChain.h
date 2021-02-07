#pragma once
#include <DefHeader.h>

namespace DX {
	/// <summary>
	/// Buffering mode of the swap chain
	/// </summary>
	enum class GISwapChain_BufferingMode : unsigned int{
		/// <summary>
		/// Use two buffers (Front and Back-Buffer)
		/// </summary>
		DOUBLE_BUFFERING = 2,

		/// <summary>
		/// Use three buffers (Front, Wait and Back-Buffer)
		/// </summary>
		TRIPLE_BUFFERING = 3,
	};

	/// <summary>
	/// DXGI Adapter Wrapper
	/// </summary>
	class GISwapChain : public ScopedComPointer<IDXGISwapChain1>{
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			GISwapChain() = default;

			/// <summary>
			/// Create swap chain
			/// </summary>
			/// <param name="ptrDevice">Pointer to device</param>
			/// <param name="ptrCmdQueue">Pointer to command queue</param>
			/// <param name="ptrFactory">Pointer to factory</param>
			/// <param name="refWindow">Reference to target window</param>
			/// <param name="bufferingMode">Buffering mode</param>
			/// <param name="bufferFormat">Buffer formate</param>
			GISwapChain(ID3D12Device* ptrDevice, ID3D12CommandQueue* ptrCmdQueue, IDXGIFactory2* ptrFactory, EasyHWND::Window& refWindow,
				GISwapChain_BufferingMode bufferingMode = GISwapChain_BufferingMode::DOUBLE_BUFFERING, DXGI_FORMAT bufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM);

			// Destructor
			~GISwapChain();

			/// <summary>
			/// Presents a frame
			/// </summary>
			/// <param name="vsync">Indicates weather the present should occure on the next vblank</param>
			void present(bool vsync = false);

			/// <summary>
			/// Resize the swap chain
			/// </summary>
			/// <param name="width">New width</param>
			/// <param name="height">New height</param>
			void resize(unsigned int width, unsigned int height);

			/// <summary>
			/// Retrive current back buffer
			/// </summary>
			/// <returns>Current back buffer as D3D12 resource</returns>
			ID3D12Resource* getCurrentBuffer();

			/// <summary>
			/// Retrive current rtv handle
			/// </summary>
			/// <returns>RTV Handle to current back buffer</returns>
			D3D12_CPU_DESCRIPTOR_HANDLE getCurrentRtvHandle();

			/// <summary>
			/// Returns the number of frames in flight
			/// </summary>
			/// <returns></returns>
			const unsigned int numberOfFramesInFlight() noexcept;

			/// <summary>
			/// Override the default release behaviour of the swapch chain
			/// </summary>
			virtual void release() override;

			/// <summary>
			/// Copy operator
			/// </summary>
			/// <param name="other">Other object</param>
			void operator=(GISwapChain& other);

			// Delete unused
			GISwapChain(const GISwapChain&) = delete;
		protected:
			/// <summary>
			/// Drops all buffers
			/// </summary>
			void dropBuffer();

			/// <summary>
			/// Retrive all buffers
			/// </summary>
			void retriveBuffer();

		private:
			/// <summary>
			/// Maximum of 3 used buffers
			/// </summary>
			ScopedComPointer<ID3D12Resource> m_arrPtrBuffers[3] = {};

			/// <summary>
			/// Heap for RTVs
			/// </summary>
			ScopedComPointer<ID3D12DescriptorHeap> m_ptrRtvHeap;

			/// <summary>
			/// Pointer to device
			/// </summary>
			ScopedComPointer<ID3D12Device> m_ptrDevice;

			/// <summary>
			/// Heap increment value
			/// </summary>
			unsigned int m_uiRTVHeapIncrement = 0;

			/// <summary>
			/// Number of used buffers
			/// </summary>
			unsigned int m_uiUsedBuffers = 0;

			/// <summary>
			/// Current render target buffer
			/// </summary>
			unsigned int m_uiCurrentBuffer = 0;
	};
}
