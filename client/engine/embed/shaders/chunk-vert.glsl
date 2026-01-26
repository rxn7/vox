#version 430 core
#extension GL_ARB_shader_draw_parameters : require

layout(location = 0) in uint a_packed_vertex;

layout(std430, binding = 1) buffer ChunkData { 
    uint a_packed_chunk_pos[]; 
};

uniform mat4 u_camera_matrix;

flat out uint v_texture_id;
out vec2 v_uv;
out float v_brightness;

const vec3 NORMALS[6] = vec3[](
    vec3( 1,  0,  0), // 0: Right
    vec3(-1,  0,  0), // 1: Left
    vec3( 0,  1,  0), // 2: Top
    vec3( 0, -1,  0), // 3: Bottom
    vec3( 0,  0,  1), // 4: Front
    vec3( 0,  0, -1)  // 5: Back
);

void main() {
    uint p = a_packed_chunk_pos[gl_DrawIDARB];
    ivec3 chunk_position = ivec3(
        bitfieldExtract(int(p), 0, 12),
        bitfieldExtract(int(p), 12, 8),
        bitfieldExtract(int(p), 20, 12)
    );

    vec3 local_position = vec3(
        float(bitfieldExtract(a_packed_vertex, 0, 5)),
        float(bitfieldExtract(a_packed_vertex, 5, 5)),
        float(bitfieldExtract(a_packed_vertex, 10, 5))
    );

    vec3 world_pos = chunk_position * 16 + local_position;
    gl_Position = u_camera_matrix * vec4(world_pos, 1.0);

    v_texture_id = bitfieldExtract(a_packed_vertex, 15, 8);
    uint face_id = bitfieldExtract(a_packed_vertex, 23, 3);
    uint ao = bitfieldExtract(a_packed_vertex, 26, 2);

    vec3 normal = NORMALS[face_id];

    v_brightness = max(0.0, 1.0 - (float(ao) * 0.2));
    
    if (face_id <= 1) {
        v_uv = vec2(local_position.z, local_position.y);
    } else if (face_id <= 3) {
        v_uv = vec2(local_position.x, local_position.z);
    } else {
        v_uv = vec2(local_position.x, local_position.y);
    }

    if(face_id == 0 || face_id == 2 || face_id == 5) {
        v_uv.x = -v_uv.x;
    }
}
