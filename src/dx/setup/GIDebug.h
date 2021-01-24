#pragma once
#include <DefHeader.h>

#ifdef _DEBUG
#include <dxgidebug.h>

namespace DX {
	/// <summary>
	/// DXGI Debug singelton
	/// </summary>
	class GIDebug : public ScopedComPointer<IDXGIDebug> {
		// Singelton	
		public:
			/// <summary>
			/// Retrive singelton instance
			/// </summary>
			/// <returns></returns>
			static GIDebug& getInstance();
		
		// Class 
		public:
			/// <summary>
			/// Validate existance of the debug interface
			/// </summary>
			void validate();

			/// <summary>
			/// Report DXGI live objects
			/// </summary>
			void reportLiveObjects();

		// Singelton
		private:
			/// <summary>
			/// Create empty adapter
			/// </summary>
			GIDebug() noexcept {};

			/// <summary>
			/// Static instance of debug interface
			/// </summary>
			static GIDebug s_instance;

			// Delete copy and assign
			GIDebug(const GIDebug&) = delete;
			void operator=(const GIDebug&) = delete;
	};
}

#endif

