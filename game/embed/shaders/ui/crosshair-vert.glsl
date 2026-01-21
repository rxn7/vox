#version 430 core

layout(location = 0) in vec2 a_pos;

uniform vec2 u_scale;
out vec2 v_uv;

void main() {
    v_uv = a_pos + vec2(0.5);

    gl_Position = vec4(a_pos.x * u_scale.x, a_pos.y * u_scale.y, 0.0, 1.0);
}
