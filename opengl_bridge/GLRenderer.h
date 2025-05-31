// opengl_bridge/GLRenderer.h
#pragma once
#include "../core/EventQueue.h"
#include <vector>

class GLRenderer {
public:
    GLRenderer(EventQueue<std::vector<float>>& q);
    void render(int width, int height);

    const std::vector<float>& getLastCoords() const { return lastCoords_; }

private:
    EventQueue<std::vector<float>>& queue_;
    // например, храним последние координаты
    std::vector<float> lastCoords_;
};
