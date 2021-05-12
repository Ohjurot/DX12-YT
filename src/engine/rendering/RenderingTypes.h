#pragma once
#include <DefHeader.h>

/// <summary>
/// Two component vector
/// </summary>
/// <typeparam name="T">Encapsulated type</typeparam>
template<typename T>
struct Vec2 {
	union {
		struct {
			T x, y;
		};
		struct {
			T r, g;
		};
		struct {
			T u, v;
		};
	};
};

/// <summary>
/// Three component vector
/// </summary>
/// <typeparam name="T">Encapsulated type</typeparam>
template<typename T>
struct Vec3 {
	union {
		struct {
			T x, y, z;
		};
		struct {
			T r, g, b;
		};
		struct {
			T u, v, w;
		};
	};
};

/// <summary>
/// Four component vector
/// </summary>
/// <typeparam name="T">Encapsulated type</typeparam>
template<typename T>
struct Vec4 {
	union {
		struct {
			T x, y, z, w;
		};
		struct {
			T r, g, b, a;
		};
	};
};
