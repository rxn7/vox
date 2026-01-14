#version 430 core

in vec2 v_uv;
out vec4 f_color;

void main() {
	f_color = vec4(v_uv.x, v_uv.y, 1.0 - v_uv.y, 1.0);
}
