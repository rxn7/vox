#pragma once

class App {
public:
	App();
	~App();
	void run();

private:
	bool init();
	bool init_glfw();
	bool init_opengl();

private:
	GLFWwindow *m_window = nullptr;
};
