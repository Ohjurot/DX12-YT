#pragma once

#include <DefHeader.h>

#define CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_DXGI 4

namespace common {
	namespace image {
		/// <summary>
		/// WIC Meta for files
		/// </summary>
		struct WIC_META {
			/// <summary>
			/// Inidcates load from memory
			/// </summary>
			bool fromMemory;

			union {
				/// <summary>
				/// File Handle
				/// </summary>
				HANDLE fileHandle;
				
				struct {
					/// <summary>
					/// Memory pointer
					/// </summary>
					void* ptrData;

					/// <summary>
					/// Memory size
					/// </summary>
					UINT64 size;
				} Memory;
			};

			/// <summary>
			/// Width of the image
			/// </summary>
			UINT width;
			
			/// <summary>
			/// Height of the image
			/// </summary>
			UINT height;

			/// <summary>
			/// Image bits per pixel
			/// </summary>
			UINT bpp;

			/// <summary>
			/// Row stride
			/// </summary>
			UINT stride;

			/// <summary>
			/// Image native pixel format
			/// </summary>
			GUID nativePixelFormat;

			/// <summary>
			/// Conversion target pixel format
			/// </summary>
			GUID targetPixelFormat;

			/// <summary>
			/// DXGI Target format
			/// </summary>
			DXGI_FORMAT targetGiFormat;
		};

		/// <summary>
		/// IO For WIC open / save
		/// </summary>
		class WicIO {
			public:
				/// <summary>
				/// Open from file
				/// </summary>
				/// <param name="path">File path</param>
				/// <param name="refMeta">Reference to target meta</param>
				/// <returns>true on succeed</returns>
				static bool open(LPCWSTR path, WIC_META& refMeta) noexcept;

				/// <summary>
				/// Open from memory
				/// </summary>
				/// <param name="data">Memory buffer</param>
				/// <param name="size">Size of memory buffer</param>
				/// <param name="refMeta">Reference to target meta</param>
				/// <returns>true on succeed</returns>
				static bool open(void* data, UINT64 size, WIC_META& refMeta) noexcept;

				/// <summary>
				/// Close meta handle
				/// </summary>
				/// <param name="refMeta">Reference to target meta</param>
				static void close(WIC_META& refMeta) noexcept;

			private:
				/// <summary>
				/// Internal open by factory decoder to meta
				/// </summary>
				/// <param name="ptrFactory">WIC Factory</param>
				/// <param name="ptrDecoder">WIC Decoder</param>
				/// <param name="refMeta">Meta to fill</param>
				/// <returns>true on suceed</returns>
				static bool open(IWICImagingFactory* ptrFactory, IWICBitmapDecoder* ptrDecoder, WIC_META& refMeta) noexcept;

				/// <summary>
				/// GUID to DXGI format conversion
				/// </summary>
				/// <param name="inputGuid">Input WIC GUID</param>
				/// <param name="ptrFormat">Pointer to output DXIG_FORMAT</param>
				/// <returns>true if conversion succeeded</returns>
				static bool convert_GUID_to_DXGI(GUID inputGuid, DXGI_FORMAT* ptrFormat) noexcept;

				/// <summary>
				/// Lookup table GUID -> DXIG / GUID Source
				/// </summary>
				static const GUID guid_to_dxgi_SOURCE[CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_DXGI];

				/// <summary>
				/// Lookup table GUID -> DXIG / DXGI Target
				/// </summary>
				static const DXGI_FORMAT guid_to_dxgi_TARGET[CLS_COMMONT_IMAGE_WICMETA__NUM_GUID_DXGI];
		};
	}
}