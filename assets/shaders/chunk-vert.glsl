#version 430 core
#extension GL_ARB_shader_draw_parameters : require

layout(location = 0) in uint packed_vertex;

layout(std430, binding = 1) buffer ChunkData { 
    uint packed_chunk_pos[]; 
};

uniform mat4 u_camera_matrix;

out vec3 v_color; 

void main() {
    float lx = float(packed_vertex & 31u);
    float ly = float((packed_vertex >> 5u) & 31u);
    float lz = float((packed_vertex >> 10u) & 31u);

    uint p = packed_chunk_pos[gl_DrawIDARB];
    int cx = int(p << 18) >> 18;
    int cz = int(p << 4) >> 18;
    int cy = int(p >> 28);

    vec3 world_pos = vec3(cx * 16.0 + lx, cy * 16.0 + ly, cz * 16.0 + lz);
    gl_Position = u_camera_matrix * vec4(world_pos, 1.0);
    
    v_color = vec3(lx/16.0, ly/16.0, lz/16.0);
}
