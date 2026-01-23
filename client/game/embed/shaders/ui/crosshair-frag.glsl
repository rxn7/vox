#version 430 core

in vec2 v_uv;
out vec4 f_color;

uniform float u_thickness = 0.1; 

void main() {
	vec2 pos = abs(v_uv - 0.5);

    bool vertical   = pos.x < u_thickness && pos.y < 0.5;
    bool horizontal = pos.y < u_thickness && pos.x < 0.5;

    if(!vertical && !horizontal) {
        discard;
    }

    f_color = vec4(1.0, 1.0, 1.0, 1.0);
}
