#include "engine.h"
#include <sstream>

EngineInstance* EngineInstance::single = nullptr;

void EngineInstance::addToQueue(const Task& task) {
    screen[task.x + task.y * term_width] = task;
}

void setCharAt(const Task& task) {
    std::cout << "\033[" << task.y << ";" << task.x * 2 << "H" << " \033[" << task.color << " \033[39";
}

void EngineInstance::tick() {
    std::stringstream ss;
    ss << "\033[2J\033[H"; // Move the cursor to the beginning
    size_t i = 0;
    for (const Task& task : screen) {
        ss << "\033[" << task.y << ";" << task.x * 2 << "H" << "\033[" << task.color << "  \033[0m"; // Write the pixel
        if (!(++i % term_width))
            ss << "\n";
    }
    std::cout << ss.str() << std::flush;
}

void EngineInstance::setBackground(const Task& task) {
    backgroundTask = task;
    
    // Render background
    if (backgroundTask.rule != RenderRule::RENDER_EMPTY) {
        for (size_t y = 0; y < term_width; ++y) {
            for (size_t x = 0; x < term_width; ++x) {
                backgroundTask.x = x;
                backgroundTask.y = y;

                if (screenValues.find(Coord(x, y)) != screenValues.end())
                    continue;

                setCharAt(backgroundTask);
            }
        }
    }
}

void EngineInstance::setBackground(const Color color) {
    backgroundTask = Task(0, 0, color, RenderRule::RENDER_BG);

    // Render background
    if (backgroundTask.rule != RenderRule::RENDER_EMPTY) {
        for (size_t y = 0; y < term_width; ++y) {
            for (size_t x = 0; x < term_width; ++x) {
                backgroundTask.x = x;
                backgroundTask.y = y;

                if (screenValues.find(Coord(x, y)) != screenValues.end())
                    continue;

                setCharAt(backgroundTask);
            }
        }
    }
}

void EngineInstance::cpix(const Coord& coord) {
    backgroundTask.x = coord.x;
    backgroundTask.y = coord.y;
    setCharAt(backgroundTask);
    std::cout << std::flush;
    screenValues.erase(coord);
}

void EngineInstance::fill(const Color color) {
    for (Task& task : screen)
        task.color = color;
}

void EngineInstance::clear() {
    fill(Color::Default);
    tick();
}

void EngineInstance::drawLine(const size_t x_0, const size_t y_0, const size_t x_1, const size_t y_1, const Color color, const RenderRule rule) {
    const double distance = std::sqrt(std::pow(static_cast<int64_t>(x_0) - static_cast<int64_t>(x_1), 2) + std::pow(static_cast<int64_t>(y_0) - static_cast<int64_t>(y_1), 2));
    const double dx = (static_cast<double>(x_1) - static_cast<double>(x_0)) / distance;
    const double dy = (static_cast<double>(y_1) - static_cast<double>(y_0)) / distance;

    double cx = x_0;
    double cy = y_0;

    for (size_t i = 0; i < static_cast<size_t>(distance); ++i) {
        addToQueue(std::round(cx), std::round(cy), color, rule);
        cx += dx;
        cy += dy;
    }
}

void EngineInstance::drawSquare(size_t x, size_t y, const size_t w, const size_t h, const Color color, const RenderRule rule) {
    for (int i = y; i < h; ++i) {
        for (int j = x; j < w; ++j) {
            addToQueue(j, i, color, rule);
        }
    }
}

void EngineInstance::drawCircle(const size_t x_0, const size_t y_0, const int r, const Color color, const RenderRule rule) {
    for(int64_t y = -r; y <= r; ++y)
        for(int64_t x = -r; x <= r; ++x)
            if(x * x + y * y <= r * r)
                addToQueue(static_cast<int64_t>(x_0) + x, static_cast<int64_t>(y_0) + y, color, rule);
}

void EngineInstance::drawText(const size_t x, const size_t y, const std::string& text, const Color foreground_color, const Color background_color) {
    const Color bgc = background_color == Color::Default
                        ? (backgroundTask.rule == RenderRule::RENDER_EMPTY ? Color::Default : backgroundTask.color)
                        : background_color;
    
    addToQueue(x, y, background_color, foreground_color, text, RenderRule::RENDER_TEXT);
}