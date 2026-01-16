#pragma once

#include <print>
#include <chrono>
#include <array>
#include <vector>
#include <unordered_map>
#include <span>
#include <memory>
#include <set>
#include <unordered_set>

#define DB_PRODUCTION_MODE
#include <battery/embed.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

#include "core/scope_timer.hpp"
