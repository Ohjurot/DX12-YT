#pragma once

#include <DefHeader.h>

#include <common/MT/Locks/SpinLock.h>
#include <engine/rxManager/Resource.h>

#define CLS_ENGINE_RXMANAGER_RXALLOCATOR__ARRAY_ENTRYS 256
#define CLS_ENGINE_RXMANAGER_RXALLOCATOR__ALLOCATION_COUNT 1024

namespace engine {
	// Fwd decl
	class RxAllocator;
	
	/// <summary>
	/// Resource allocator (source class)
	/// </summary>
	class _RxAllocator {
		public:
			/// <summary>
			/// Allocation function
			/// </summary>
			/// <returns>RESOURCE handle</returns>
			RESOURCE alloc() noexcept;

		protected:
			// Allow allocator singelton
			friend class RxAllocator;

			/// <summary>
			/// Constructor
			/// </summary>
			_RxAllocator();

			/// <summary>
			/// Destructor
			/// </summary>
			~_RxAllocator();

		private:
			/// <summary>
			/// Allow CLS_ENGINE_RXMANAGER_RXALLOCATOR__ARRAY_ENTRYS * mc_allocationSize entrys
			/// </summary>
			__RX_DATA* m_arrAllocator[CLS_ENGINE_RXMANAGER_RXALLOCATOR__ARRAY_ENTRYS];

			/// <summary>
			/// Current array index
			/// </summary>
			UINT64 m_arrIndex = 0;

			/// <summary>
			/// Current element index
			/// </summary>
			UINT64 m_elementIndex = 0;

			/// <summary>
			/// Allocation spin lock
			/// </summary>
			MT::SpinLock<> m_allocationLock;
	};

	/// <summary>
	/// Resource allocator (Singelton)
	/// </summary>
	class RxAllocator {
		public:
			/// <summary>
			/// Init singelton
			/// </summary>
			static void init();

			/// <summary>
			/// Shutdown singelton
			/// </summary>
			static void shutdown();

			/// <summary>
			/// Retrive singelton instance
			/// </summary>
			/// <returns>Allocator instance</returns>
			static _RxAllocator* getInstance() noexcept;

		private:
			/// <summary>
			/// Signelton instance pointer
			/// </summary>
			static _RxAllocator* s_ptrInstance;
	};
}