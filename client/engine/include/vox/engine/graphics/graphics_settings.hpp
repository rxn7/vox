#pragma once

#include "vox/common/helper/singleton.hpp"

struct GraphicsSettingsState {
    bool m_ambient_occlusion_enabled = true;
};

// TODO: Common Settings base class with load & save
class GraphicsSettings {
SINGLETON_CLASS(GraphicsSettings);

public:
    GraphicsSettingsState m_state;
};