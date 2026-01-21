#include "draw.h"

void drawLine(const size_t x_0, const size_t y_0, const size_t x_1, const size_t y_1, const Color color, const RenderRule rule) {
    const double distance = std::sqrt(std::pow(static_cast<int64_t>(x_0) - static_cast<int64_t>(x_1), 2) + std::pow(static_cast<int64_t>(y_0) - static_cast<int64_t>(y_1), 2));
    const double dx = (static_cast<double>(x_1) - static_cast<double>(x_0)) / distance;
    const double dy = (static_cast<double>(y_1) - static_cast<double>(y_0)) / distance;

    double cx = x_0;
    double cy = y_0;

    for (size_t i = 0; i < static_cast<size_t>(distance); ++i) {
        EngineInstance::getInstance()->addToQueue(std::round(cx), std::round(cy), color, rule);
        cx += dx;
        cy += dy;
    }
}

void drawSquare(size_t x, size_t y, const size_t w, const size_t h, const Color color, const RenderRule rule) {
    for (int i = y; i < h; ++i)
        for (int j = x; j < w; ++j)
            EngineInstance::getInstance()->addToQueue(j, i, color, rule);
}

void drawCircle(const size_t x_0, const size_t y_0, const int r, const Color color, const RenderRule rule) {
    for(int64_t y = -r; y <= r; ++y)
        for(int64_t x = -r; x <= r; ++x)
            if(x * x + y * y <= r * r)
                EngineInstance::getInstance()->addToQueue(static_cast<int64_t>(x_0) + x, static_cast<int64_t>(y_0) + y, color, rule);
}
