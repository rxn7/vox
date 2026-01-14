#include "app.h"

#include <iostream>

App::App() { 
}

App::~App() { 
	if(m_window) {
		glfwDestroyWindow(m_window);
	}

	glfwTerminate();
}

void App::run() { 
	if(!init()) {
		return;
	}

	while(!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(m_window);
	}
}

bool App::init() {
	if(!init_glfw())	return false;
	if(!init_opengl())	return false;

	return true;
}

bool App::init_glfw() {
	if(!glfwInit()) {
		std::cerr << "failed to initialize GLFW" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(1280, 720, "vox", nullptr, nullptr);
	if(!m_window) {
		std::cerr << "failed to create window" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(m_window);
	return true;
}

bool App::init_opengl() {
	int32_t version = gladLoadGL(glfwGetProcAddress);
	if(version == 0) {
		std::cerr << "failed to initialize GLAD" << std::endl;
		return false;
	}

	std::cout << "OpenGL version: " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	return true;
}
