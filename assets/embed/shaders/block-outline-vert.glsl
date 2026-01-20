#version 430 core

layout(location = 0) in vec3 a_pos;

uniform mat4 u_camera_matrix;
uniform mat4 u_model_matrix;

out vec3 v_local_pos;

void main() {
	v_local_pos = a_pos;
	gl_Position = u_camera_matrix * u_model_matrix * vec4(a_pos, 1.0);
}
