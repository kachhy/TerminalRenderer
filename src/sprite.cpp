
#include "sprite.h"
#include <fstream>

void Sprite::render(EngineInstance* inst, const size_t frame) const {
	for (const Task& task : frames[frame])
		inst->addToQueue(task);
}

void Sprite::clear(EngineInstance* inst, const size_t frame, const Coord start) const {
	for (const Task& task : frames[frame])
		inst->cpix(start + task.toCoord());
}

void Sprite::load(const std::string& filename) {
	std::ifstream in(filename);
	if (!in.is_open())
		throw std::runtime_error("Failed to load " + filename);

	int x, y, color, rule;
	SpriteFrame f;

	while (in >> x >> y >> color >> rule) {
		if (x == NEW_FRAME) { // New frame
			frames.emplace_back(f);
			f.clear();
			continue;
		}

		f.emplace_back(x, y, static_cast<Color>(color), static_cast<RenderRule>(rule));
	}
}

void Sprite::save(const std::string& filename) const {

	std::ofstream out(filename);
	if (!out.is_open())
		throw std::runtime_error("Failed to open " + filename);

	for (const SpriteFrame& frame : frames) {
		for (const Task& task : frame)
			out << task.x << ' ' << task.y << ' ' << static_cast<int>(task.color) << ' ' << static_cast<int>(task.rule) << "\n";
		out << "-1 0 0 0\n";
	}
}