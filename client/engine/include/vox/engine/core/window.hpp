#pragma once

#include "GLFW/glfw3.h"
class Window {
public:
    Window();
    ~Window();
    
    void destroy();
    bool create(ivec2 size, std::string_view name);
    
    inline GLFWwindow *get_glfw_window() const { 
        return mp_window; 
    }
    
    inline void update_size(ivec2 size) { 
        m_window_size = size; 
    }

    inline f32 get_aspect_ratio() { 
        return static_cast<f32>(m_window_size.x) / static_cast<f32>(m_window_size.y); 
    }

    inline ivec2 get_size() const { 
        return m_window_size; 
    }
    
private:
    ivec2 m_window_size;
    GLFWwindow *mp_window = nullptr;
};