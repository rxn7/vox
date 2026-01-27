#pragma once

class IGame {
public:
	virtual ~IGame() {}

	virtual bool init() = 0;

	virtual void update(f32 delta_time) = 0;
	virtual void render_3d(f32 aspect_ratio) = 0;
	virtual void render_ui() = 0;
	virtual void render_imgui() = 0;
};
