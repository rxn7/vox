#version 430 core

uniform vec4 u_color;
uniform float u_thickness;

in vec3 v_local_pos;
out vec4 f_color;

void main() {
	f_color = vec4(0.0, 0.0, 0.0, 1.0);

	bool near_x = v_local_pos.x < u_thickness || v_local_pos.x > (1.0 - u_thickness);
	bool near_y = v_local_pos.y < u_thickness || v_local_pos.y > (1.0 - u_thickness);
	bool near_z = v_local_pos.z < u_thickness || v_local_pos.z > (1.0 - u_thickness);
	bool is_edge = (near_x && near_y) || (near_x && near_z) || (near_y && near_z);

	if (is_edge) {
		f_color = u_color;
	} else {
		discard;
	}
}
