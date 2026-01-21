#include "vox/core/window.hpp"
#include "GLFW/glfw3.h"

Window::Window() { }

Window::~Window() {}

bool Window::create(ivec2 size, std::string_view name) {
    mp_window = glfwCreateWindow(size.x, size.y, name.data(), nullptr, nullptr);
    
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