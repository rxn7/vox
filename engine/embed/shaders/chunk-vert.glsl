#version 430 core
#extension GL_ARB_shader_draw_parameters : require

layout(location = 0) in uint a_packed_vertex;

layout(std430, binding = 1) buffer ChunkData { 
    uint a_packed_chunk_pos[]; 
};

uniform mat4 u_camera_matrix;

flat out uint v_texture_id;
out vec3 v_normal;
out vec2 v_uv;
out float v_ao;

const vec3 NORMALS[6] = vec3[](
    vec3( 1,  0,  0), // 0: Right
    vec3(-1,  0,  0), // 1: Left
    vec3( 0,  1,  0), // 2: Top
    vec3( 0, -1,  0), // 3: Bottom
    vec3( 0,  0,  1), // 4: Front
    vec3( 0,  0, -1)  // 5: Back
);

void main() {
    float local_x = float(bitfieldExtract(a_packed_vertex, 0, 5));
    float local_y = float(bitfieldExtract(a_packed_vertex, 5, 5));
    float local_z = float(bitfieldExtract(a_packed_vertex, 10, 5));

    v_texture_id = bitfieldExtract(a_packed_vertex, 15, 9);
    uint face_id = bitfieldExtract(a_packed_vertex, 24, 3);
    
    uint ao = bitfieldExtract(a_packed_vertex, 27, 2);
    v_ao = 1.0 - (ao * 0.2);

    v_normal = NORMALS[face_id];
    
    uint p = a_packed_chunk_pos[gl_DrawIDARB];
    int chunk_x = int(p << 18) >> 18;
    int chunk_z = int(p << 4) >> 18;
    int chunk_y = int(p >> 28);

    vec3 world_pos = vec3(
        chunk_x * 16.0 + local_x, 
        chunk_y * 16.0 + local_y, 
        chunk_z * 16.0 + local_z
    );

    gl_Position = u_camera_matrix * vec4(world_pos, 1.0);

    if (face_id <= 1) {
        v_uv = vec2(local_z, local_y);
    } else if (face_id <= 3) {
        v_uv = vec2(local_x, local_z);
    } else {
        v_uv = vec2(local_x, local_y);
    }

    if(face_id == 0 || face_id == 2 || face_id == 5) {
        v_uv.x = -v_uv.x;
    }
}
