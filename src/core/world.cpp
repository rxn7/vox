#include "world.hpp"
#include "core/render/chunk_renderer.hpp"

World::World() {
	for(int x = 0; x < 4; ++x) {
		for(int z = 0; z < 4; ++z) {
			m_chunks.emplace(i16vec3(x, 0, z), std::make_unique<Chunk>());
		}
	}
}

World::~World() {}

void World::update() {
	PROFILE_FUNC();

	for(auto &[position, chunk] : m_chunks) {
		chunk->update();

		if(chunk->m_render_slot.has_value() && chunk->m_index_count > 0) {
			ChunkRenderer::get_instance().add_draw_command(chunk->m_render_slot.value(), chunk->m_index_count, position);
		}
	}
}
