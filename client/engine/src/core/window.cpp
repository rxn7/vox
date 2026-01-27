#include "vox/engine/core/window.hpp"

Window::Window() { }

Window::~Window() {}

bool Window::create(ivec2 size, std::string_view name, bool fullscreen) {
	if(fullscreen) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
		mp_window = glfwCreateWindow(vidmode->width, vidmode->height, name.data(), monitor, nullptr);
	} else {
		mp_window = glfwCreateWindow(size.x, size.y, name.data(), nullptr, nullptr);
	}
	
	if(!mp_window) {
		return false;
	}
	
	return true;
}

void Window::destroy() {
	if(mp_window) {
		glfwDestroyWindow(mp_window);
	}
}