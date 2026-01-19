#include "core/app.hpp"
#include "core/render/chunk_renderer.hpp"
#include "core/input.hpp"
#include "core/profiler/profiler.hpp"
#include "core/render/text_renderer.hpp"

#include <imgui.h>
#include <stb_image.h>

App *App::sp_instance = nullptr;

App::App() { 
	sp_instance = this;
}

App::~App() { 
	mp_game.reset();

	ChunkRenderer::destroy_instance();
	TextRenderer::destroy_instance();
	InputManager::destroy_instance();
	Profiler::destroy_instance();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	if(mp_window) {
		glfwDestroyWindow(mp_window);
	}

	glfwTerminate();
}

void App::run() { 
	if(!init()) return;

	f64 last_frame, current_frame; last_frame = current_frame = glfwGetTime();
	Profiler &profiler = Profiler::get_instance();

	while(!glfwWindowShouldClose(mp_window)) {
		profiler.new_frame();
		ChunkRenderer::get_instance().new_frame();

		current_frame = glfwGetTime();
		m_delta_time = current_frame - last_frame;
		last_frame = current_frame;

		update();
		render();

		glfwPollEvents();
		profiler.end_frame();
	}
}

void App::update() { 
	PROFILE_FUNC();

	{
		PROFILE_SCOPE("Get cursor pos");
		f64 mouse_x, mouse_y;
		glfwGetCursorPos(mp_window, &mouse_x, &mouse_y);
		InputManager::get_instance().update_mouse_position(vec2(mouse_x, mouse_y));
	}

	mp_game->update(m_delta_time);
}

void App::render() {
	PROFILE_FUNC();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render_3d();
	render_ui();
	render_imgui();

	PROFILE_SCOPE("Swap buffers");
	glfwSwapBuffers(mp_window);
}

void App::render_3d() { 
	PROFILE_FUNC();
	glEnable(GL_DEPTH_TEST);
	
	const f32 aspect_ratio = f32(m_window_size.x) / f32(m_window_size.y);
	mp_game->render_3d(aspect_ratio);
}

void App::render_ui() {
	PROFILE_FUNC();
	glDisable(GL_DEPTH_TEST);

	mp_game->render_ui();
}

void App::render_imgui() {
	PROFILE_FUNC();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_profiler_window.render();

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool App::init() {
	if(!init_glfw())	return false;
	if(!init_opengl())	return false;
	if(!init_imgui())	return false;

	stbi_set_flip_vertically_on_load(true);

	TextRenderer &text_renderer = TextRenderer::get_instance();
	text_renderer.init();
	text_renderer.update_screen_size(m_window_size);

	InputManager::get_instance().set_mouse_mode(mp_window, GLFW_CURSOR_DISABLED);
	ChunkRenderer::get_instance().init();

	mp_game = std::make_unique<Game>();

	constexpr float VRAM_USAGE_PER_CHUNK = (ChunkRenderer::VERTEX_SLOT_SIZE * sizeof(u32) + ChunkRenderer::INDEX_SLOT_SIZE * sizeof(u32)) / 1000.0f;

	std::println("ChunkRenderer VRAM usage: {} KB per chunk", VRAM_USAGE_PER_CHUNK);
	std::println("ChunkRenderer Total VRAM usage: {} KB", VRAM_USAGE_PER_CHUNK * ChunkRenderer::MAX_CHUNKS);

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

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	return true;
}

bool App::init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if(!ImGui_ImplGlfw_InitForOpenGL(mp_window, true)) {
		std::println("Failed to initialize ImGui");
		return false;
	}

	if(!ImGui_ImplOpenGL3_Init("#version 430 core")) {
		std::println("Failed to initialize ImGui");
		return false;
	}
	
	return true;
}

void App::error_callback_glfw(i32 error, const char *description) {
	std::println("GLFW error {}: {}", error, description);
}

void App::window_size_callback_glfw([[maybe_unused]] GLFWwindow *p_window, i32 w, i32 h) {
	sp_instance->m_window_size.x = w;
	sp_instance->m_window_size.y = h;

	glViewport(0, 0, w, h);

	TextRenderer::get_instance().update_screen_size(vec2(w, h));
}

void App::key_event_callback_glfw([[maybe_unused]] GLFWwindow *p_window, [[maybe_unused]] i32 key, [[maybe_unused]] i32 scancode, [[maybe_unused]] i32 action, [[maybe_unused]] i32 mods) {
	switch(action) {
		case GLFW_PRESS:
			InputManager::get_instance().handle_key_event(p_window, key, true);
			break;

		case GLFW_RELEASE:
			InputManager::get_instance().handle_key_event(p_window, key, false);
			break;
	}
}

