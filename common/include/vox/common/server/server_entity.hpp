#pragma once

#include "vox/common/world/block_position.hpp"

class ServerEntity {
public:
	virtual ~ServerEntity() = default;

	virtual void update(vec3 position, f32 pitch, f32 yaw);

	inline BlockPosition get_block_position() const {
		return m_block_position;
	}

	inline vec3 get_position() const {
		return m_position;
	}

	inline f32 get_pitch() const {
		return m_pitch;
	}

	inline f32 get_yaw() const {
		return m_yaw;
	}

protected:
	BlockPosition m_block_position;
	vec3 m_position;
	f32 m_pitch;
	f32 m_yaw;
};
