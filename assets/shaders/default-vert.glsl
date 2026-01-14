#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 v_uv;
uniform mat4 u_cameraMatrix;

void main() {
	gl_Position = u_cameraMatrix * vec4(a_pos, 1.0);
}
