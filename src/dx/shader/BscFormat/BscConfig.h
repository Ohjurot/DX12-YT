#pragma once

#include <DefHeader.h>

#define BSC_SHADER_TYPE_VERTEX				BSC_SHADER_TYPE::VERTEX
#define BSC_SHADER_TYPE_HULL				BSC_SHADER_TYPE::HULL
#define BSC_SHADER_TYPE_DOMAIN				BSC_SHADER_TYPE::DOMAINSHADER
#define BSC_SHADER_TYPE_GEOMETRY			BSC_SHADER_TYPE::GEOMETRY
#define BSC_SHADER_TYPE_PIXEL				BSC_SHADER_TYPE::PIXEL
#define BSC_SHADER_TYPE_COMPUTE				BSC_SHADER_TYPE::COMPUTE
#define BSC_SHADER_TYPE_MESH				BSC_SHADER_TYPE::MESH
#define BSC_SHADER_TYPE_AMPLIFICATION		BSC_SHADER_TYPE::AMPLIFICATION

#define BSC_ENCODING_BINARY					BSC_ENCODING::BINARY
#define BSC_ENCODING_BINARY_LZ4_DEFAULT		BSC_ENCODING::BINARY_LZ4_DEFAULT

/// <summary>
/// Shader type
/// </summary>
enum class BSC_SHADER_TYPE : UINT8 {
	/// <summary>
	/// Vertex shader
	/// </summary>
	VERTEX = 0x01,
	
	/// <summary>
	/// Hull shader
	/// </summary>
	HULL = 0x02,

	/// <summary>
	/// Domain shader
	/// </summary>
	DOMAINSHADER = 0x03,

	/// <summary>
	/// Geometry shader
	/// </summary>
	GEOMETRY = 0x04,

	/// <summary>
	/// Pixel Shader
	/// </summary>
	PIXEL = 0x05,

	/// <summary>
	/// Compute shader
	/// </summary>
	COMPUTE = 0x06,

	/// <summary>
	/// Mesh shader
	/// </summary>
	MESH = 0x07,

	/// <summary>
	/// Ampification shader
	/// </summary>
	AMPLIFICATION = 0x08,
};

/// <summary>
/// Shader data encoding
/// </summary>
enum class BSC_ENCODING : UINT8 {
	/// <summary>
	/// Binary encoded
	/// </summary>
	BINARY = 0x10,

	/// <summary>
	/// Binary compressed with LZ4 default
	/// </summary>
	BINARY_LZ4_DEFAULT = 0x20,
};

/// <summary>
/// BSC Target descriptor
/// </summary>
typedef struct _BSC_TARGET_DESC {
	/// <summary>
	/// Type of shader
	/// </summary>
	BSC_SHADER_TYPE type;

	/// <summary>
	/// HLSL major version
	/// </summary>
	UINT8 majorVersion;

	/// <summary>
	/// HLSL minor version
	/// </summary>
	UINT8 minorVersion;
} BSC_TARGET_DESC;

/// <summary>
/// Shader Read / Write info
/// </summary>
typedef struct _BSC_SHADER_INFO {	
	/// <summary>
	/// Shader name
	/// </summary>
	LPCSTR shaderName;

	/// <summary>
	/// Shader encoding
	/// </summary>
	BSC_ENCODING encoding;

	/// <summary>
	/// Shader target
	/// </summary>
	BSC_TARGET_DESC target;
} BSC_SHADER_INFO;
