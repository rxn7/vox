#pragma once

#include "vox/core/singleton.hpp"

struct GraphicsSettingsState {
    bool ambient_occlusion_enabled = true;
};

// TODO: Common Settings base class with load & save
class GraphicsSettings {
SINGLETON_CLASS(GraphicsSettings);

public:
    GraphicsSettingsState m_state;
};