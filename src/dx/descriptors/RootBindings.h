#pragma once

#include <DefHeader.h>

#define _GET_NTH_ARG(_1, _2, _3, _4, N, ...) N
#define COUNT_VARARGS(...) _GET_NTH_ARG(__VA_ARGS__, 4, 3, 2, 1)


#define DX_ROOT_TYPES				dx::ROOT_TYPES
#define DX_ROOT_TYPE_SRV			dx::ROOT_TYPES::SRV
#define DX_ROOT_TYPE_CBV			dx::ROOT_TYPES::CBV
#define DX_ROOT_TYPE_UAV			dx::ROOT_TYPES::UAV
#define DX_ROOT_TYPE_CONSTANT		dx::ROOT_TYPES::CONSTANT
#define DX_ROOT_TYPE_TABLE			dx::ROOT_TYPES::TABLE

#define DX_ROOT_SIGNATURE_ENTRY		dx::ROOT_SIGNATURE_ENTRY

namespace dx {
	/// <summary>
	/// Type of root signature element
	/// </summary>
	enum class ROOT_TYPES : unsigned char {
		/// <summary>
		/// Shader resource view
		/// </summary>
		SRV = 0x1,

		/// <summary>
		/// Constant buffer view
		/// </summary>
		CBV = 0x2,

		/// <summary>
		/// Unordered access view
		/// </summary>
		UAV = 0x3,

		/// <summary>
		/// Root constant
		/// </summary>
		CONSTANT = 0x4,

		/// <summary>
		/// Descriptor tabel
		/// </summary>
		TABLE = 0x5,
	};

	/// <summary>
	/// Signature entry
	/// </summary>
	struct ROOT_SIGNATURE_ENTRY {
		// Type
		ROOT_TYPES type;

		union {
			/// <summary>
			/// Shader resource view
			/// </summary>
			struct {
				/// <summary>
				/// Object address
				/// </summary>
				D3D12_GPU_VIRTUAL_ADDRESS descriptorAddress;
			}SRV_CBV_UAV;

			/// <summary>
			/// Root const
			/// </summary>
			struct {
				/// <summary>
				/// Data pointer
				/// </summary>
				void* ptrData;

				/// <summary>
				/// Size of data
				/// </summary>
				UINT dataSize;
			}CONSTANT;

			/// <summary>
			/// Tabel
			/// </summary>
			struct {
				/// <summary>
				/// Address of first element in table
				/// </summary>
				D3D12_GPU_DESCRIPTOR_HANDLE tabelFirstAddress;
			}TABLE;
		};
	};
	
	/// <summary>
	/// Root binding class
	/// </summary>
	template<unsigned int N>
	class RootBindings {
		public:
			// Delete unsupported
			RootBindings() = delete;
			
			/// <summary>
			/// Copy construct
			/// </summary>
			/// <param name="other">other object</param>
			RootBindings(const RootBindings<N>& other) {
				// Copy data
				memcpy(m_entrys, other.m_entrys, sizeof(ROOT_SIGNATURE_ENTRY) * N);
			}

			/// <summary>
			/// Root Binding constructor
			/// </summary>
			/// <param name="...args">Input arguments</param>
			template<typename... T, typename = std::enable_if_t<(sizeof...(T) == N) && std::is_same_v<T, ROOT_SIGNATURE_ENTRY>>>
			RootBindings(T... args) {
				// Index and inline helper call to insert into element array
				unsigned int index = 0;
				((variadic_fillHelper(&index, args)), ...);
			}

			/// <summary>
			/// Bind root elements to command list
			/// </summary>
			/// <param name="ptrCommandList"></param>
			void bind(ID3D12GraphicsCommandList* ptrCommandList) {
				EXPP_ASSERT_DBG(ptrCommandList, "Invalid command list pointer");

				// Bind each element
				for (unsigned int i = 0; i < N; i++) {
					switch (m_entrys[i].type) {
						// Shader resource view
						case DX_ROOT_TYPE_SRV:
							ptrCommandList->SetGraphicsRootShaderResourceView(i, m_entrys[i].SRV_CBV_UAV.descriptorAddress);
							break;
						// Constant buffer view
						case DX_ROOT_TYPE_CBV:
							ptrCommandList->SetGraphicsRootConstantBufferView(i, m_entrys[i].SRV_CBV_UAV.descriptorAddress);
							break;
						// Univeral Access view
						case DX_ROOT_TYPE_UAV:
							ptrCommandList->SetGraphicsRootUnorderedAccessView(i, m_entrys[i].SRV_CBV_UAV.descriptorAddress);
							break;
						// Constant
						case DX_ROOT_TYPE_CONSTANT:
							ptrCommandList->SetGraphicsRoot32BitConstants(i, m_entrys[i].CONSTANT.dataSize / 4, m_entrys[i].CONSTANT.ptrData, 0);
							break;
						// Root tabel
						case DX_ROOT_TYPE_TABLE:
							ptrCommandList->SetGraphicsRootDescriptorTable(i, m_entrys[i].TABLE.tabelFirstAddress);
							break;
						// Error
						default:
							EXPP_ASSERT(false, "Invalid root signature entry type");
							break;
					}
				}
			}

		private:
			/// <summary>
			/// Variadic helpter
			/// </summary>
			/// <param name="index">Index pointer</param>
			/// <param name="entry">Current entry</param>
			inline void variadic_fillHelper(unsigned int* index, ROOT_SIGNATURE_ENTRY entry) {
				m_entrys[*index] = entry;
				(*index)++;
			}

		private:
			/// <summary>
			/// Entry array
			/// </summary>
			ROOT_SIGNATURE_ENTRY m_entrys[N];
	};

	// Predecl 1
	template<DX_ROOT_TYPES T>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT(D3D12_GPU_VIRTUAL_ADDRESS) {
		static_assert(false, "Type Not Supported");
	}

	// Predecl 2
	template<DX_ROOT_TYPES T>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT(D3D12_GPU_DESCRIPTOR_HANDLE) {
		static_assert(false, "Type Not Supported");
	}

	// Predecl 3
	template<DX_ROOT_TYPES T>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT(UINT, void*) {
		static_assert(false, "Type Not Supported");
	}

	// SRV
	template<>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT<DX_ROOT_TYPE_SRV>(D3D12_GPU_VIRTUAL_ADDRESS address) {
		ROOT_SIGNATURE_ENTRY e;
		e.type = DX_ROOT_TYPE_SRV;
		e.SRV_CBV_UAV.descriptorAddress = address;
		return e;
	}

	// CBV
	template<>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT<DX_ROOT_TYPE_CBV>(D3D12_GPU_VIRTUAL_ADDRESS address) {
		ROOT_SIGNATURE_ENTRY e;
		e.type = DX_ROOT_TYPE_CBV;
		e.SRV_CBV_UAV.descriptorAddress = address;
		return e;
	}

	// UAV
	template<>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT<DX_ROOT_TYPE_UAV>(D3D12_GPU_VIRTUAL_ADDRESS address) {
		ROOT_SIGNATURE_ENTRY e;
		e.type = DX_ROOT_TYPE_UAV;
		e.SRV_CBV_UAV.descriptorAddress = address;
		return e;
	}

	// Table
	template<>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT<DX_ROOT_TYPE_TABLE>(D3D12_GPU_DESCRIPTOR_HANDLE handle) {
		ROOT_SIGNATURE_ENTRY e;
		e.type = DX_ROOT_TYPE_TABLE;
		e.TABLE.tabelFirstAddress = handle;
		return e;
	}

	// Constant
	template<>
	static ROOT_SIGNATURE_ENTRY ROOT_ELEMENT<DX_ROOT_TYPE_CONSTANT>(UINT size, void* ptr) {
		EXPP_ASSERT(size % 4 == 0, "Type must be 32Bit alligned and sized");

		ROOT_SIGNATURE_ENTRY e;
		e.type = DX_ROOT_TYPE_CONSTANT;
		e.CONSTANT.ptrData = ptr;
		e.CONSTANT.dataSize = size;
		return e;
	}
}
