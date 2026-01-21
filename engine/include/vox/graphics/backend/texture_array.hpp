#pragma once

class TextureArray {
public:
	TextureArray();
	~TextureArray();

	bool load(std::span<const std::string_view> paths);
	inline u32 get_id() const { return m_texture_id; }

private:
	u32 m_texture_id;

	bool m_initialized = false;
};
