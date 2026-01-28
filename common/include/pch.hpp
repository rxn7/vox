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
#include <atomic>
#include <queue>
#include <mutex>
#include <thread>
#include <bitset>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/hash.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;
using ivec4 = glm::ivec4;
using mat4 = glm::mat4;
using mat3 = glm::mat3;

using i8vec3 = glm::i8vec3;
using u8vec3 = glm::u8vec3;
using i16vec3 = glm::i16vec3;
using i16vec2 = glm::i16vec2;

using f32 = float;
using f64 = double;

using i8 = int8_t;
using u8 = uint8_t;

using i16 = int16_t;
using u16 = uint16_t;

using i32 = int32_t;
using u32 = uint32_t;

using i64 = int64_t;
using u64 = uint64_t;

using sz = size_t;

template<class... Ts> 
struct Overloaded : Ts... { using Ts::operator()...; };

template<class... Ts> 
Overloaded(Ts...) -> Overloaded<Ts...>;

#include "vox/common/profiler/profiler_scope_timer.hpp"
