#pragma once

#include <DefHeader.h>

#define SHADER_TYPE_INVALID		SHADER_TYPE::INVALID
#define SHADER_TYPE_VERTEX		SHADER_TYPE::VERTEX
#define SHADER_TYPE_PIXEL		SHADER_TYPE::PIXEL

/// <summary>
/// Type of shader
/// </summary>
enum class SHADER_TYPE {
	/// <summary>
	/// Invalid shader
	/// </summary>
	INVALID,

	/// <summary>
	/// Vertex shader
	/// </summary>
	VERTEX,

	/// <summary>
	/// Pixel shader
	/// </summary>
	PIXEL,
};
