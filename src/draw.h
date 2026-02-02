#ifndef DRAW_H
#define DRAW_H

#include "engine.h"

// Drawing tools
void drawLine(const size_t x_0, const size_t y_0, const size_t x_1, const size_t y_1, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT, void (*addToQueueService)(const Task&) = nullptr);
void drawSquare(const size_t x, const size_t y, const size_t w, const size_t h, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT, void (*addToQueueService)(const Task&) = nullptr);
void drawBox(const size_t x, const size_t y, const size_t w, const size_t h, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT, void (*addToQueueService)(const Task&) = nullptr);
void drawCircle(const size_t x, const size_t y, const int r, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT, void (*addToQueueService)(const Task&) = nullptr);

#endif // DRAW_H