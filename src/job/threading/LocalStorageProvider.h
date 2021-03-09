#pragma once

#include <DefHeader.h>

namespace Threading {
	/// <summary>
	/// TLS and FLS provider
	/// </summary>
	class LocalStorageProvider {
		// Class
		public:
			/// <summary>
			/// Get current threads TLS Data
			/// </summary>
			/// <returns>Stored Pointer</returns>
			void* getTLSData();

			/// <summary>
			/// Set current threads TLS Data
			/// </summary>
			/// <param name="data">Input pointer</param>
			void setTLSData(void* data);

			/// <summary>
			/// Get current fibers FLS Data
			/// </summary>
			/// <returns>Stored Pointer</returns>
			void* getFLSData();

			/// <summary>
			/// Get current fibers FLS Data
			/// </summary>
			/// <param name="data">Input pointer</param>
			void setFLSData(void* data);

		private:
			/// <summary>
			/// Cookie for TLS
			/// </summary>
			DWORD m_dwTlsCookie;

			/// <summary>
			/// Cookie for FLS
			/// </summary>
			DWORD m_dwFlsCookie;

		// Singelton
		public:
			/// <summary>
			/// Function for retriving the singelton instance
			/// </summary>
			/// <returns>Ref to LocalStorageProvider</returns>
			static LocalStorageProvider& getInstance();

		private:
			/// <summary>
			/// Construct
			/// </summary>
			LocalStorageProvider();

			/// <summary>
			/// Destruct
			/// </summary>
			~LocalStorageProvider();

			/// <summary>
			/// Static instance
			/// </summary>
			static LocalStorageProvider s_instance;
			
	};
}