#include "app.hpp"
#include "input.hpp"

App *App::sp_instance = nullptr;

App::App() { 
	sp_instance = this;
}

App::~App() { 
	mp_world.reset();

	if(mp_window) {
		glfwDestroyWindow(mp_window);
	}

	glfwTerminate();
}

void App::run() { 
	if(!init()) return;

	mp_world = std::make_unique<World>();

	f64 last_frame, current_frame;
	last_frame = current_frame = glfwGetTime();

	while(!glfwWindowShouldClose(mp_window)) {
		const f32 aspect_ratio = f32(m_window_size.x) / f32(m_window_size.y);

		current_frame = glfwGetTime();
		f32 delta_time = current_frame - last_frame;
		last_frame = current_frame;

		mp_world->update(delta_time);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mp_world->render(aspect_ratio);

		glfwSwapBuffers(mp_window);
		glfwPollEvents();
	}
}

bool App::init() {
	if(!init_glfw())	return false;
	if(!init_opengl())	return false;

	return true;
}

bool App::init_glfw() {
	glfwSetErrorCallback(error_callback_glfw);

	if(!glfwInit()) {
		std::println("failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	mp_window = glfwCreateWindow(1280, 720, "vox", nullptr, nullptr);
	if(!mp_window) {
		std::println("failed to create window");
		return false;
	}

	glfwSetWindowSizeCallback(mp_window, window_size_callback_glfw);
	glfwSetKeyCallback(mp_window, key_event_callback_glfw);

	glfwMakeContextCurrent(mp_window);

	return true;
}

bool App::init_opengl() {
	const i32 version = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
	if(version == 0) {
		std::println("failed to initialize GLAD");
		return false;
	}

	std::println("OpenGL version: {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	return true;
}

void App::error_callback_glfw(i32 error, const char *description) {
	std::println("GLFW error {}: {}", error, description);
}

void App::window_size_callback_glfw([[maybe_unused]] GLFWwindow *window, i32 w, i32 h) {
	sp_instance->m_window_size.x = w;
	sp_instance->m_window_size.y = h;

	glViewport(0, 0, w, h);
}

void App::key_event_callback_glfw([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] i32 key, [[maybe_unused]] i32 scancode, [[maybe_unused]] i32 action, [[maybe_unused]] i32 mods) {
	switch(action) {
		case GLFW_PRESS:
			InputManager::handle_key_event(key, true);
			break;

		case GLFW_RELEASE:
			InputManager::handle_key_event(key, false);
			break;
	}
}
