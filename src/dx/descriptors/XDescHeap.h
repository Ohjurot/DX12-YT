#pragma once

#include <DefHeader.h>

namespace DX {
	/// <summary>
	/// Handle compund
	/// </summary>
	struct DescHeapHandles {
		/// <summary>
		/// GPU Desc Heap handle
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

		/// <summary>
		/// CPU Desc Heap heandle
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	};

	/// <summary>
	/// Descritor heap
	/// </summary>
	class XDescHeap : public ScopedComPointer<ID3D12DescriptorHeap>{
		public:
			// Fwd
			class XDescHeap_Itterator;

			/// <summary>
			/// Allow default constructor
			/// </summary>
			XDescHeap() = default;

			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="ptrDevice">Device pointer</param>
			/// <param name="type">type of heap</param>
			/// <param name="size">descritor count</param>
			/// <param name="shaderVisble">shader visability</param>
			XDescHeap(ID3D12Device* ptrDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT size, bool shaderVisble = true);
			
			/// <summary>
			/// Retrive begin itterator
			/// </summary>
			/// <returns>itterator</returns>
			DX::XDescHeap::XDescHeap_Itterator begin();
			
			/// <summary>
			/// Retrive end itterator
			/// </summary>
			/// <returns></returns>
			DX::XDescHeap::XDescHeap_Itterator end();

			/// <summary>
			/// Get handle at index
			/// </summary>
			/// <param name="index">Index to retrive handle</param>
			/// <returns>Handles</returns>
			DX::DescHeapHandles get(unsigned int index);

			/// <summary>
			/// Array operator
			/// </summary>
			/// <param name="index">Index to retrive</param>
			/// <returns>Handles</returns>
			DX::DescHeapHandles operator[](unsigned int index);

			/// <summary>
			/// Retrive heap size
			/// </summary>
			/// <returns>Size of heap</returns>
			UINT size() noexcept;

		private:
			/// <summary>
			/// Heap size
			/// </summary>
			UINT m_size = 0;

			/// <summary>
			/// Increment size
			/// </summary>
			UINT m_increment = 0;

		public:
			/// <summary>
			/// Heap Itterator
			/// </summary>
			typedef class XDescHeap_Itterator {
				public:
					/// <summary>
					/// Constructor
					/// </summary>
					/// <param name="refHeap">Heap to create itterator for</param>
					/// <param name="index">Start index</param>
					XDescHeap_Itterator(XDescHeap& refHeap, UINT index = 0);

					// Default copy
					XDescHeap_Itterator(const XDescHeap_Itterator& other) = default;

					// Assign
					DX::XDescHeap::XDescHeap_Itterator& operator=(const XDescHeap_Itterator& other);

					// Increment
					DX::XDescHeap::XDescHeap_Itterator operator++(int);

					// Decrement
					DX::XDescHeap::XDescHeap_Itterator operator--(int);

					// Add
					DX::XDescHeap::XDescHeap_Itterator& operator+(unsigned int offset);

					// Add Type
					DX::XDescHeap::XDescHeap_Itterator& operator+(const DX::XDescHeap::XDescHeap_Itterator& other);

					// Add assign
					DX::XDescHeap::XDescHeap_Itterator& operator+=(unsigned int offset);

					// Sub 
					DX::XDescHeap::XDescHeap_Itterator& operator-(unsigned int offset);

					// Sub Type
					DX::XDescHeap::XDescHeap_Itterator& operator-(const DX::XDescHeap::XDescHeap_Itterator& other);

					// Sub assign
					DX::XDescHeap::XDescHeap_Itterator& operator-=(unsigned int offset);

					// Other itterator
					bool operator==(const DX::XDescHeap::XDescHeap_Itterator& other) const;

					// UInt converter
					operator unsigned int() noexcept;

					// Deref operator
					DescHeapHandles operator*();

					/// <summary>
					/// Get heap handles
					/// </summary>
					/// <returns>Heap handles</returns>
					DX::DescHeapHandles second();

					/// <summary>
					/// Itterator first (parent) object
					/// </summary>
					/// <returns>Heap ref</returns>
					DX::XDescHeap& first() noexcept;

				private:
					/// <summary>
					/// Heap reference
					/// </summary>
					XDescHeap& m_refHeap;

					/// <summary>
					/// Intterator index
					/// </summary>
					UINT m_itIndex = 0;
			} Itterator;
	};
}
