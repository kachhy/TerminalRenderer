#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
#include <string>
#include "engine.h"

#define NEW_FRAME -1
using SpriteFrame = std::vector<Task>;

class Sprite {
private:
    std::vector<SpriteFrame> frames;
public:
    Sprite() = default;
    Sprite(const std::string& filename) { load(filename); }

    void render(EngineInstance* inst, const size_t frame) const;
    void clear(EngineInstance* inst, const size_t frame, const Coord start) const;
    void load(const std::string& filename);
    void save(const std::string& filename) const;
};

#endif // SPRITE_H