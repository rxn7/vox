#pragma once

namespace MathUtils {
	constexpr f32 angle_diff(f32 a, f32 b) {
		f32 diff = glm::fmod(b - a + glm::pi<f32>(), glm::two_pi<f32>()) - glm::pi<f32>();
		if(diff < -glm::pi<f32>()) {
			diff += glm::pi<f32>() * 2.0f;
		}

		return diff;
	}

	constexpr f32 lerp_angle(f32 a, f32 b, f32 t) {

		return a + angle_diff(a, b) * glm::clamp(t, 0.0f, 1.0f);
	}
}
