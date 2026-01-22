#include "vox/core/engine.hpp"
#include "vox/core/i_game.hpp"
#include "vox/core/input.hpp"
#include "vox/tools/fps_counter.hpp"
#include "vox/tools/profiler/profiler_scope_timer.hpp"

#include <stb_image.h>

Engine *Engine::sp_instance = nullptr;

Engine::Engine() { 
    assert(sp_instance == nullptr);
    sp_instance = this;
}

Engine::~Engine() { 
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
    
    m_window.destroy();

	glfwTerminate();
}

void Engine::run_game(IGame *game) {
	Profiler &profiler = Profiler::get_instance();
    profiler.begin();

	if(!init())
		return;

    mp_game = game;
    if(!mp_game->init())
        return;

	f64 last_frame, current_frame; last_frame = current_frame = glfwGetTime();
	Input &input = Input::get_instance();

	while(!glfwWindowShouldClose(m_window.get_glfw_window())) {
		current_frame = glfwGetTime();
		m_delta_time = current_frame - last_frame;

		last_frame = current_frame;
		FpsCounter::get_instance().tick();
		input.new_frame();

		{
			PROFILE_SCOPE("Poll Events");
			glfwPollEvents();
		}

		update();
		render();

		{
			PROFILE_SCOPE("Swap Buffers");
			glfwSwapBuffers(m_window.get_glfw_window());
		}

        if(profiler.should_end(PROFILING_DURATION)) {
            profiler.end();
            profiler.begin();
        }
	}
    
    mp_game = nullptr;
}

void Engine::update() { 
	PROFILE_FUNC();
	mp_game->update(m_delta_time);
}

void Engine::render() {
	PROFILE_FUNC();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		PROFILE_SCOPE("Render 3D");
		glEnable(GL_DEPTH_TEST);
		
		const f32 aspect_ratio = f32(m_window.get_size().x) / f32(m_window.get_size().y);
		mp_game->render_3d(aspect_ratio);
	}

	{
		PROFILE_SCOPE("Render UI");
		glDisable(GL_DEPTH_TEST);

		mp_game->render_ui();
	}

	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
        
        m_profiler_imgui_tool.render();
		mp_game->render_imgui();

		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

bool Engine::init() {
	ProfilerTimePoint start = ProfilerClock::now();

	if(!init_glfw())	return false;
	if(!init_opengl())	return false;
	if(!init_imgui())	return false;

	stbi_set_flip_vertically_on_load(true);

	Input &input = Input::get_instance();
	input.init();
	input.set_mouse_mode(GLFW_CURSOR_DISABLED);

	// constexpr f32 VRAM_USAGE_PER_CHUNK = (WorldRenderer::VERTEX_SLOT_SIZE * sizeof(u32) + WorldRenderer::INDEX_SLOT_SIZE * sizeof(u32)) / 1000.0f;

	// std::println("WorldRenderer VRAM usage: {} KB per chunk", VRAM_USAGE_PER_CHUNK);
	// std::println("WorldRenderer Total VRAM usage: {} KB", VRAM_USAGE_PER_CHUNK * WorldRenderer::MAX_CHUNKS);

	ProfilerTimePoint end = ProfilerClock::now();
	std::println("App initialization took {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

	return true;
}

bool Engine::init_glfw() {
	glfwSetErrorCallback(error_callback_glfw);

	if(!glfwInit()) {
		std::println("failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if(!m_window.create({1280, 720}, "vox")) {
		std::println("failed to create window");
		return false;
	}
    
    GLFWwindow *p_window = m_window.get_glfw_window();
	glfwSetWindowSizeCallback(p_window, window_size_callback_glfw);
	glfwSetCursorPosCallback(p_window, mouse_move_callback_glfw);
	glfwSetKeyCallback(p_window, key_event_callback_glfw);
	glfwSetMouseButtonCallback(p_window, mouse_button_event_callback_glfw);

	glfwMakeContextCurrent(p_window);
	glfwSwapInterval(0);

	return true;
}

bool Engine::init_opengl() {
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

bool Engine::init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if(!ImGui_ImplGlfw_InitForOpenGL(m_window.get_glfw_window(), true)) {
		std::println("Failed to initialize ImGui");
		return false;
	}

	if(!ImGui_ImplOpenGL3_Init("#version 430 core")) {
		std::println("Failed to initialize ImGui");
		return false;
	}
	
	return true;
}

void Engine::error_callback_glfw(i32 error, const char *description) {
	std::println("GLFW error {}: {}", error, description);
}

void Engine::window_size_callback_glfw([[maybe_unused]] GLFWwindow *p_window, i32 w, i32 h) {
    Window &window = sp_instance->get_window();
	window.update_size({w, h});

	glViewport(0, 0, w, h);
	sp_instance->mp_game->handle_window_resize(vec2(w, h));
}

void Engine::key_event_callback_glfw([[maybe_unused]] GLFWwindow *p_window, [[maybe_unused]] i32 key, [[maybe_unused]] i32 scancode, [[maybe_unused]] i32 action, [[maybe_unused]] i32 mods) {
	Input &input = Input::get_instance();
	switch(action) {
		case GLFW_PRESS:
			input.update_key(key, true);
			break;

		case GLFW_RELEASE:
			input.update_key(key, false);
			break;
	}
}

void Engine::mouse_button_event_callback_glfw([[maybe_unused]] GLFWwindow *p_window, i32 button, i32 action, [[maybe_unused]] i32 mods) {
	Input &input = Input::get_instance();
	switch(action) {
		case GLFW_PRESS:
			input.update_mouse_button(button, true);
			break;

		case GLFW_RELEASE:
			input.update_mouse_button(button, false);
			break;
	}
}

void Engine::mouse_move_callback_glfw([[maybe_unused]] GLFWwindow *p_window, [[maybe_unused]] f64 x, [[maybe_unused]] f64 y) {
	Input &input = Input::get_instance();
	input.update_mouse_position(vec2(x, y));
}
