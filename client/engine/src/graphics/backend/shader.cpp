#include "vox/engine/graphics/backend/shader.hpp"

u32 Shader::s_bound_program = 0;

Shader::Shader() {}

Shader::~Shader() {
	if(!m_created) return;

	glDeleteProgram(m_program_id);
}

bool Shader::load(std::string_view vert_source, std::string_view frag_source) {
	m_program_id = glCreateProgram();

	m_created = true;

	m_frag_id = create_shader(SHADER_FRAGMENT, frag_source);
	m_vert_id = create_shader(SHADER_VERTEX, vert_source);

	if(m_vert_id == 0 || m_frag_id == 0) [[unlikely]] {
		return false;
	}

	glAttachShader(m_program_id, m_vert_id);
	glAttachShader(m_program_id, m_frag_id);

	glBindAttribLocation(m_program_id, 0, "a_pos");
	glBindAttribLocation(m_program_id, 1, "a_uv");

	glLinkProgram(m_program_id);
	if(check_compile_errors(m_program_id, GL_LINK_STATUS, true)) [[unlikely]] {
		return false;
	}

	if(m_frag_id != 0) {
		glDetachShader(m_program_id, m_frag_id);
		glDeleteShader(m_frag_id);
	}

	if(m_vert_id != 0) {
		glDetachShader(m_program_id, m_vert_id);
		glDeleteShader(m_vert_id);
	}

	glValidateProgram(m_program_id);
	if(check_compile_errors(m_program_id, GL_VALIDATE_STATUS, true)) [[unlikely]] {
		return false;
	}

	return true;
}

u32 Shader::create_shader(ShaderType type, std::string_view source) {
	const u32 id = glCreateShader(type);
	
	if(id == 0) [[unlikely]] {
		std::println("Failed to create a shader\nShader source\n: {}", source);
		return 0;
	}

	const char *sources[1] = { source.data() };
	const i32 lengths[1] = { (i32)source.size() };

	glShaderSource(id, 1, sources, lengths);
	glCompileShader(id);
	
	if(check_compile_errors(id, GL_COMPILE_STATUS, false)) [[unlikely]] {
		return 0;
	}

	return id;
}

i32 Shader::get_uniform_location(std::string_view name) const {
	PROFILE_FUNC();

	const auto &uniform_location_it = m_uniform_locations.find(std::string(name));
	const bool uniform_exists = uniform_location_it != m_uniform_locations.end();

	if(!uniform_exists) [[unlikely]] {
		const i32 location = glGetUniformLocation(m_program_id, name.data());
		if(location == -1) [[unlikely]] {
			std::println("Failed to get uniform location: {}", name);
		}

		m_uniform_locations.emplace(name, location);
		return location;
	}

	return uniform_location_it->second;
}

bool Shader::check_compile_errors(u32 id, u32 flag, bool is_program) {
	i32 result;
	char msg_buffer[1024];

	is_program 
		? glGetProgramiv(id, flag, &result) 
		: glGetShaderiv(id, flag, &result);

	if(result != 0) [[likely]] {
		return false;
	}

	is_program
		? glGetProgramInfoLog(id, sizeof(msg_buffer), nullptr, msg_buffer)
		: glGetShaderInfoLog(id, sizeof(msg_buffer), nullptr, msg_buffer);

	std::println("Shader error: {}", msg_buffer);

	return true;
}
