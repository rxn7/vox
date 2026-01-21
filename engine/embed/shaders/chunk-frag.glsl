#version 430 core

flat in uint v_texture_id;
in vec3 v_normal;
in vec2 v_uv;

uniform sampler2DArray u_textures;

out vec4 f_color;

void main() {
	vec4 color = texture(u_textures, vec3(v_uv, float(v_texture_id)));

	float brightness = 0.8;

	// Top (Sunlight)
	if(v_normal.y > 0.5) 
		brightness = 1.0;
	// Bottom
	else if(v_normal.y < -0.5) 
		brightness = 0.5;

	f_color = vec4(color.rgb * brightness, color.a);
}
