#include "engine.h"
#include <sstream>

EngineInstance* EngineInstance::single = nullptr;

void EngineInstance::addToQueue(const Task& task) { screen[task.x + task.y * term_width] = task; }

void setCharAt(const Task& task) { std::cout << "\033[" << task.y << ";" << task.x * 2 << "H" << " \033[" << task.color << " \033[39"; }

void EngineInstance::tick() {
	std::stringstream ss;
	ss << "\033[2J\033[H"; // Move the cursor to the beginning
	size_t i = 0;
	std::unordered_set<Coord> text_skip_indices;
	for (const Task& task : screen) {
		if (task.rule == RenderRule::RENDER_TEXT) {
			ss << "\033[" << task.y << ";" << task.x * 2 << "H" << "\033[" << task.color << "\033[" << task.foreground_color << task.text << "\033[0m";
			for (size_t i = 0; i < task.text.size() / 2; i++)
				text_skip_indices.emplace(task.x + i, task.y);
		} else {
			if (text_skip_indices.find(Coord(task.x, task.y)) != text_skip_indices.end()) {
				if (!(++i % term_width))
					ss << "\n";

				continue;
			}
			ss << "\033[" << task.y << ";" << task.x * 2 << "H" << "\033[" << task.color << "  \033[0m"; // Write the pixel
		}

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

void EngineInstance::drawText(const size_t x, const size_t y, const std::string& text, const Color foreground_color, const Color background_color) {
	const Color bgc = background_color == Color::Default ? (backgroundTask.rule == RenderRule::RENDER_EMPTY ? Color::Default : backgroundTask.color) : background_color;

	addToQueue(x, y, background_color, foreground_color, text, RenderRule::RENDER_TEXT);
}