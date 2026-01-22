#version 430 core

flat in uint v_texture_id;
in vec3 v_normal;
in vec2 v_uv;
in float v_brightness;

uniform sampler2DArray u_textures;

out vec4 f_color;

void main() {
	vec4 color = texture(u_textures, vec3(v_uv, float(v_texture_id)));
	f_color = vec4(color.rgb * v_brightness, color.a);
}
