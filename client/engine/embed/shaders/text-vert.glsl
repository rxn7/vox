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

uniform vec2 u_position;
uniform float u_font_size;
uniform vec2 u_screen_size;

out vec2 v_uv;
out flat int v_char_index;

void main() {
    v_uv = vertices[gl_VertexID];
    
    uint block = packed_indices[gl_InstanceID / 4];
    uint shift = (gl_InstanceID % 4) * 8;
    
    v_char_index = int((block >> shift) & 0xFFu);

    vec2 pos = vertices[gl_VertexID];
    pos.x += float(gl_InstanceID) * 1.1; 
    pos *= u_font_size;
    pos += u_position;
    
    vec2 ndc = (2.0 * pos / u_screen_size) - vec2(1.0);
    gl_Position = vec4(ndc, 0.0, 1.0);
}
