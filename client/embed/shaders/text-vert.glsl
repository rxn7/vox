#version 430 core

const vec2 vertices[4] = vec2[4](
	vec2(0.0, 0.0), // BL
	vec2(1.0, 0.0), // BR
	vec2(0.0, 1.0), // TL
	vec2(1.0, 1.0)  // TR
);

layout(std430, binding = 1) buffer TextBuffer {
	uint packed_indices[]; 
};

uniform int u_horizontal_align;
uniform int u_vertical_align;
uniform int u_text_length;
uniform float u_font_size;
uniform mat4 u_mvp;

out vec2 v_uv;
out flat int v_char_index;

void main() {
	v_uv = vertices[gl_VertexID];
	
	uint block = packed_indices[gl_InstanceID / 4];
	uint shift = (gl_InstanceID % 4) * 8;
	
	v_char_index = int((block >> shift) & 0xFFu);

	vec2 pos = vertices[gl_VertexID];
	pos.x += float(gl_InstanceID) * 1.1; 

	switch(u_horizontal_align) {
		case 0: break; // Left
		case 1: pos.x -= 0.55 * u_text_length; break; // Center
		case 2: pos.x -= 1.1 * u_text_length; break; // Right
	}

	switch(u_vertical_align) {
		case 0: pos.y -= 1.1; break; // Top
		case 1: pos.y -= 0.55; break; // Middle
		case 2: break; // Bottom
	}

	pos *= u_font_size;
	
	gl_Position = u_mvp * vec4(pos, 0.0, 1.0);
}
