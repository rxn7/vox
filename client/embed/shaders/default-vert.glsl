#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

out vec2 v_uv;
out vec3 v_normal;

uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;

void main() {
	gl_Position = u_camera_matrix * u_model_matrix * vec4(a_pos, 1.0);
	v_uv = a_uv;
	v_normal = a_normal;
}
