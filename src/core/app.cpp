#include "app.hpp"
#include "text_renderer.hpp"
#include "input.hpp"
#include <format>

App *App::sp_instance = nullptr;

App::App() { 
	sp_instance = this;
}

App::~App() { 
	mp_world.reset();

	TextRenderer::destroy_instance();
	InputManager::destroy_instance();

	if(mp_window) {
		glfwDestroyWindow(mp_window);
	}

	glfwTerminate();
}

void App::run() { 
	if(!init()) return;

	TextRenderer &text_renderer = TextRenderer::get_instance();
	text_renderer.init();
	text_renderer.update_screen_size(m_window_size);

	InputManager &input_manager = InputManager::get_instance();
	input_manager.set_mouse_mode(mp_window, GLFW_CURSOR_DISABLED);

	mp_world = std::make_unique<World>();

	f64 last_frame, current_frame;
	last_frame = current_frame = glfwGetTime();

	while(!glfwWindowShouldClose(mp_window)) {
		current_frame = glfwGetTime();
		m_delta_time = current_frame - last_frame;
		last_frame = current_frame;

		f64 mouse_x, mouse_y;
		glfwGetCursorPos(mp_window, &mouse_x, &mouse_y);
		input_manager.update_mouse_position(vec2(mouse_x, mouse_y));

		mp_world->update(m_delta_time);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_3d();
		render_ui();

		glfwSwapBuffers(mp_window);
		glfwPollEvents();
	}
}

void App::render_3d() { 
	glEnable(GL_DEPTH_TEST);

	const f32 aspect_ratio = f32(m_window_size.x) / f32(m_window_size.y);
	mp_world->render(aspect_ratio);
}

void App::render_ui() {
	glDisable(GL_DEPTH_TEST);

	TextRenderer &text_renderer = TextRenderer::get_instance();

	const Camera &camera = mp_world->get_camera();
	const vec3 position = camera.get_position();

	text_renderer.render_text(std::format("FPS: {:.0f}", 1.0f / m_delta_time), vec2(0, m_window_size.y - 16.0f), 16.0f);
	text_renderer.render_text(std::format("Camera Position: ({:.2f}, {:.2f}, {:.2f})", position.x, position.y, position.z), vec2(0, 0), 16.0f);
	text_renderer.render_text(std::format("Camera Rotation: ({:.2f}, {:.2f})", glm::degrees(camera.get_pitch()), glm::degrees(camera.get_yaw())), vec2(0, 16), 16.0f);
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

	mp_window = glfwCreateWindow(1280, 720, "vox", nullptr, nullptr);
	if(!mp_window) {
		std::println("failed to create window");
		return false;
	}

	glfwSetWindowSizeCallback(mp_window, window_size_callback_glfw);
	glfwSetKeyCallback(mp_window, key_event_callback_glfw);

	glfwMakeContextCurrent(mp_window);
	glfwSwapInterval(0);

	return true;
}

bool App::init_opengl() {
	const i32 version = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
	if(version == 0) {
		std::println("failed to initialize GLAD");
		return false;
	}

	std::println("OpenGL version: {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	TextRenderer::get_instance().update_screen_size(vec2(w, h));
}

void App::key_event_callback_glfw([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] i32 key, [[maybe_unused]] i32 scancode, [[maybe_unused]] i32 action, [[maybe_unused]] i32 mods) {
	switch(action) {
		case GLFW_PRESS:
			InputManager::get_instance().handle_key_event(key, true);
			break;

		case GLFW_RELEASE:
			InputManager::get_instance().handle_key_event(key, false);
			break;
	}
}
