#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Windows
#define NOMINMAX
#include <Windows.h>

// Windows related
#include <wincodec.h>

// DirectX 12
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_6.h>
