#pragma once

#include "tools/profiler/scope_timer.hpp"

enum ShaderType {
	SHADER_VERTEX = GL_VERTEX_SHADER,
	SHADER_FRAGMENT = GL_FRAGMENT_SHADER,
};

class Shader {
public:
	Shader();
	~Shader();

	bool load(std::string_view vert_source, std::string_view frag_source);
	i32 get_uniform_location(std::string_view name) const;

	inline void set_uniform_i32(std::string_view name, i32 value) {
		set_uniform(name, [value](i32 location) { glUniform1i(location, value); });
	}

	inline void set_uniform_f32(std::string_view name, f32 value) {
		set_uniform(name, [value](i32 location) { glUniform1f(location, value); });
	}

	inline void set_uniform_vec2(std::string_view name, vec2 value) {
		set_uniform(name, [value](i32 location) { glUniform2f(location, value.x, value.y); });
	}

	inline void set_uniform_ivec2(std::string_view name, vec2 value) {
		set_uniform(name, [value](i32 location) { glUniform2i(location, value.x, value.y); });
	}

	inline void set_uniform_mat4(std::string_view name, const mat4 &value) {
		set_uniform(name, [value](i32 location) { glUniformMatrix4fv(location, 1, false, glm::value_ptr(value)); });
	}

	inline void bind() const { 
		PROFILE_FUNC();
		if(m_program_id == s_bound_program) 
			return;

		s_bound_program = m_program_id;
		glUseProgram(m_program_id); 
	}

private:
	template <typename Func>
	void set_uniform(std::string_view name, Func &&uploader) {
		PROFILE_FUNC();
		const i32 location = get_uniform_location(name);
		if(location == -1) [[unlikely]] {
			return;
		}

		bind();
		uploader(location);
	}

	u32 create_shader(ShaderType type, std::string_view source);
	static bool check_compile_errors(u32 id, u32 flag, bool is_program); 

private:
	static u32 s_bound_program;

	bool m_created = false;
	mutable std::unordered_map<std::string, i32> m_uniform_locations;

	u32 m_program_id;
	u32 m_frag_id;
	u32 m_vert_id;
};
