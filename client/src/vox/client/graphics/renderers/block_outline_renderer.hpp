#pragma once

#include "vox/common/world/block_position.hpp"
#include "vox/client/graphics/backend/static_mesh.hpp"
#include "vox/client/graphics/backend/shader.hpp"

struct BlockOutlineVertex {
	static void setup_attributes() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockOutlineVertex), (void*)offsetof(BlockOutlineVertex, position));
	}

	vec3 position;
};

class BlockOutlineRenderer {
public:
	BlockOutlineRenderer();
	void render(BlockPosition block_position, const mat4 &camera_matrix);

private:
	StaticMesh<BlockOutlineVertex> m_mesh;
	Shader m_shader;

	vec4 m_color = vec4(1.0f, 1.0f, 1.0f, 0.5f);
	f32 m_thickness = 0.01f;
};
