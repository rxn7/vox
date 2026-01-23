#pragma once

#include <print>
#include <cmath>
#include <chrono>
#include <fstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <span>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;
typedef glm::mat4 mat4;
typedef glm::mat3 mat3;

typedef glm::i8vec3 i8vec3;
typedef glm::u8vec3 u8vec3;
typedef glm::i16vec3 i16vec3;

typedef float f32;
typedef double f64;

typedef int8_t i8;
typedef uint8_t u8;

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

typedef size_t sz;

#include "vox/common/profiler/profiler_scope_timer.hpp"