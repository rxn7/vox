#version 430 core

uniform vec4 u_color;

in vec2 v_uv;
in flat int v_char_index;

out vec4 f_color;

layout(std430, binding = 0) buffer FontBuffer {
    uint packed_rows[]; 
};

void main() {
    int col = int(v_uv.x * 8.0);
    int row = int((1.0 - v_uv.y) * 8.0);

    uint row_block = packed_rows[(v_char_index * 2) + (row / 4)];
    
    uint shift = (row % 4) * 8;
    uint row_bits = (row_block >> shift) & 0xFFu;
    
    if(((row_bits >> (7 - col)) & 1u) == 0u) {
        discard;
    }

    f_color = u_color;
}
