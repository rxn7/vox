#include "vox/common/server/server_entity.hpp"

void ServerEntity::update(vec3 position, f32 pitch, f32 yaw) {
	m_position = position;
	m_pitch = pitch;
	m_yaw = yaw;

	m_block_position = BlockPosition(m_position);
}
