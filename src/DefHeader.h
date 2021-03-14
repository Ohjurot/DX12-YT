#pragma once

#include <WinInclude.h>
#include <stdint.h>
#include <vendor/ExceptionsPlusPlus.h>
#include <vendor/EasyHWND.h>
#include <vendor/ScopedPointer.h>
#include <vendor/ScopedHandle.h>
#include <vendor/ScopedComPointer.h>
#include <vendor/LockTools.h>

#include <vendor/DirectX_TypeName.h>

#include <job/system/JobPause.h>

#include <atomic>
#include <type_traits>
#include <concepts>

#define EXPP_HRESULT(hr, what) EXPP_EXCEPTION(HRESULT, (HRESULT)hr, EXPP_TEXT(what))
#define EVALUATE_HRESULT(call, what) {HRESULT hr; if(FAILED(hr = call)) {throw EXPP_HRESULT(hr, what);}}
#define EVALUATE_HRESULT_HR(hr, call, what) if(FAILED(hr = call)) {throw EXPP_HRESULT(hr, what);}
#define EXPP_ASSERT(expr, message) if(!(expr)) {throw EXPP_EXCEPTION(std::exception, std::exception("Assertion failed!"), EXPP_TEXT(message));}

typedef void(*PAUSE_FUNCTION)(void);

#define THREAD_PAUSE_FUNCTION() JobPause::__job_pause()
#define THREAD_PAUSE_FUNCTION_F __inline_pause

static inline void __inline_pause() {
	THREAD_PAUSE_FUNCTION();
}

#ifdef _DEBUG
#define DEBUG_ONLY_EXECUTE(args) args
#define EXPP_ASSERT_DBG(expr, message) if(!(expr)) {throw EXPP_EXCEPTION(std::exception, std::exception("Debug assertion failed!"), EXPP_TEXT(message));}
#else
#define DEBUG_ONLY_EXECUTE(args) 
#define EXPP_ASSERT_DBG(expr, message) 
#endif

#define MEM_KiB(kib)		   (kib * 1024)
#define MEM_MiB(mib)	MEM_KiB(mib * 1024)
#define MEM_GiB(gib)	MEM_MiB(gib * 1024)
