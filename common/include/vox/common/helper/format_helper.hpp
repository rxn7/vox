#pragma once

namespace FormatHelper {
	template <typename Rep, typename Period>
	std::string duration(std::chrono::duration<Rep, Period> d) {
		const f64 ns_count = std::chrono::duration_cast<std::chrono::duration<f64, std::nano>>(d).count();
		
		if(ns_count == 0) {
			return "0ns";
		} 
		
		if(glm::abs(ns_count) < 1000.0) {
			return std::format("{}ns", ns_count);
		} 

		if(glm::abs(ns_count) < 1'000'000.0) {
			return std::format("{}us", ns_count / 1000);
		} 

		if(glm::abs(ns_count) < 1'000'000'000.0) {
			return std::format("{:.2f}ms", ns_count / 1'000'000);
		} 
		
		return std::format("{:.2f}s", ns_count / 1'000'000'000);
	}

	std::string memory(u64 bytes);
}
