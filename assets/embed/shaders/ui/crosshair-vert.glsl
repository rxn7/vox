#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;

uniform vec2 u_scale;

out vec2 v_uv;

void main() {
    v_uv = a_uv;

    gl_Position = vec4(a_pos.x * u_scale.x, a_pos.y * u_scale.y, 0.0, 1.0);
}
