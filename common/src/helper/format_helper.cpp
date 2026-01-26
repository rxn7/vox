#include "vox/common/helper/format_helper.hpp"

std::string FormatHelper::memory(u64 bytes) {
	if(bytes < 1024) {
		return std::format("{}B", bytes);
	} 

	if(bytes < 1024 * 1024) {
		return std::format("{:.2f}KB", bytes / 1024.0);
	} 

	if(bytes < 1024 * 1024 * 1024) {
		return std::format("{:.2f}MB", bytes / (1024.0 * 1024.0));
	} 

	return std::format("{:.2f}GB", bytes / (1024.0 * 1024.0 * 1024.0));
}
