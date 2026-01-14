#pragma once

enum ShaderType {
	SHADER_VERTEX = GL_VERTEX_SHADER,
	SHADER_FRAGMENT = GL_FRAGMENT_SHADER,
};

class Shader {
public:
	Shader();
	~Shader();

	bool load(std::string_view vert_source, std::string_view frag_source);

	void set_uniform_f32(const std::string &name, f32 value);
	void set_uniform_mat4(const std::string &name, const mat4 &matrix);
	i32 get_uniform_location(const std::string &name) const;

	inline void bind() const { 
		if(m_program_id == s_bound_program) 
			return;

		s_bound_program = m_program_id;
		glUseProgram(m_program_id); 
	}

private:
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
