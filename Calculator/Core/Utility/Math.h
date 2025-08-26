//
// Created by scion on 6/7/2025.
//

#pragma once

#include "Macro.h"

#include <cstdint>
#include <filesystem>
#include <type_traits>
#include <GLM/mat3x3.hpp>
#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include <GLM/gtx/matrix_decompose.hpp>
#include <GLM/gtx/quaternion.hpp>

#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GPU_ALIGN alignas(16)

#define TO_RAD 0.01745329251f
#define TO_DEG 57.2957795131f

#define US_TO_MS (1.0 / 1000000.0)
#define MS_TO_S (1.0 / 1000.0)

typedef std::filesystem::path Path;
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

enum class Dimension : u8
{
	D1D = 1,
	D2D = 2,
	D3D = 3
};

using namespace glm;

template<class COMPONENT_T>
concept is_integer_v = std::is_scalar_v<COMPONENT_T> && !std::is_floating_point_v<COMPONENT_T>;

template<Dimension DIMENSION, typename COMPONENT_T = float>
using Position = std::conditional_t<DIMENSION == Dimension::D1D, COMPONENT_T, vec<(u8) DIMENSION, COMPONENT_T>>;

template<Dimension DIMENSION, typename COMPONENT_T = u32> requires is_integer_v<COMPONENT_T>
using Size = Position<DIMENSION, COMPONENT_T>;

template<Dimension DIMENSION, typename COMPONENT_T = float>
using RotationMatrix = mat<(size_t) DIMENSION, (size_t) DIMENSION, COMPONENT_T>;

template<Dimension DIMENSION, typename COMPONENT_T = float>
using TransformMatrix = mat<(size_t) DIMENSION + 1, (size_t) DIMENSION + 1, COMPONENT_T>;

constexpr size_t GLSizeOf(u32 t)
{
	if(t == GL_FLOAT) return 4;
	return 1ull << ((t - 0x1400) >> 1ull);
}

namespace glm
{
	typedef quat quaternion;
}

typedef Size<Dimension::D1D> Size1D;
typedef Size<Dimension::D2D> Size2D;
typedef Size<Dimension::D3D> Size3D;

typedef Position<Dimension::D1D> Position1D;
typedef Position<Dimension::D2D> Position2D;
typedef Position<Dimension::D3D> Position3D;

enum class FOVType : u8
{
	Horizontal,
	Vertical
};

enum class AngleType : u8
{
	Degree,
	Radian
};

template<FOVType TO, AngleType UNIT = AngleType::Radian>
float constexpr fov_cast(float in, const Size2D& size)
{
	float aspect;
	if constexpr(TO == FOVType::Horizontal) aspect = (float) size.x / (float) size.y;
	else aspect = (float) size.y / (float) size.x;

	if constexpr(UNIT == AngleType::Degree) in *= TO_RAD;
	const float val = 2 * atanf(tanf(in / 2) * aspect);

	if constexpr(UNIT == AngleType::Degree) return val * TO_DEG;
	else return val;
}

struct Viewport
{
	Size2D Size = DEFAULT;
	Size2D Offset = DEFAULT;
};

template<class T> requires requires(const T& t)
{
	{ t + t } -> std::same_as<T>;
	{ t - t } -> std::same_as<T>;
	{ t / t } -> std::same_as<T>;
}
T DivCeil(const T& numerator, const T& denominator)
{
	return (numerator + denominator - T(1)) / denominator;
}

template<class T> requires std::is_scalar_v<T>
void FixMinMax(vec<2, T>& min, vec<2, T>& max)
{
	if(max.x < min.x) std::swap(min.x, max.x);
	if(max.y < min.y) std::swap(min.y, max.y);
}