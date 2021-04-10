#pragma once

#include <DefHeader.h>

#define SHADER_TYPE_INVALID			SHADER_TYPE::INVALID
#define SHADER_TYPE_VERTEX			SHADER_TYPE::VERTEX
#define SHADER_TYPE_PIXEL			SHADER_TYPE::PIXEL
#define SHADER_TYPE_HULL			SHADER_TYPE::HULL
#define SHADER_TYPE_DOMAIN			SHADER_TYPE::DOMAIN_SHADER
#define SHADER_TYPE_GEOMETRY		SHADER_TYPE::GEOMETRY
#define SHADER_TYPE_ROOTSIGNATURE	SHADER_TYPE::ROOTSIGNATURE

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

	/// <summary>
	/// Hull shaders
	/// </summary>
	HULL,

	/// <summary>
	/// Domain shader
	/// </summary>
	DOMAIN_SHADER,

	/// <summary>
	/// Geometry shader
	/// </summary>
	GEOMETRY,

	/// <summary>
	/// Root signature
	/// </summary>
	ROOTSIGNATURE,
};
