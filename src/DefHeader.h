#pragma once

#include <WinInclude.h>
#include <stdint.h>
#include <vendor/ExceptionsPlusPlus.h>
#include <vendor/ScopedPointer.h>
#include <vendor/ScopedHandle.h>
#include <vendor/ScopedComPointer.h>

#define EXPP_HRESULT(hr, what) EXPP_EXCEPTION(HRESULT, (HRESULT)hr, EXPP_TEXT(what))
#define EVALUATE_HRESULT(call, what) {HRESULT hr; if(FAILED(hr = call)) {throw EXPP_HRESULT(hr, what);}}
#define EVALUATE_HRESULT_HR(hr, call, what) if(FAILED(hr = call)) {throw EXPP_HRESULT(hr, what);}

#ifdef _DEBUG
#define DEBUG_ONLY_EXECUTE(args) args
#else
#define DEBUG_ONLY_EXECUTE(args) 
#endif

#define MEM_KiB(kib)		   (kib * 1024)
#define MEM_MiB(mib)	MEM_KiB(mib * 1024)
#define MEM_GiB(gib)	MEM_MiB(gib * 1024)

#define LOGICAL_INT_INVALID	0xFFFFFFFF
