#include "ui.h"

#ifdef WIN32
#include <conio.h>
#else
#include <termios.h>

#include <termios.h>
#include <unistd.h>
#include <cstdio>

int _getch() {
    int ch;
    struct termios oldt, newt;
    
    if (tcgetattr(STDIN_FILENO, &oldt) != 0)
		return -1;
    
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
#endif

UIInstance* UIInstance::single = nullptr;

void Button::render() const {
	if (w < 2 || h < 2)
		return;

	drawSquare(x, y, w, h, outline_color);
	drawSquare(x + 1, y + 1, w - 2, h - 2, active() ? selected_color : default_color);
	if (!text.empty()) {
		const size_t text_x = x * 2 + w - text.size() / 2;
		EngineInstance::getInstance()->drawText(text_x, y + h / 2, text, text_color, active() ? selected_color : default_color);
	}
}

void Button::onSelect() {
	if (func != nullptr)
		func();
}

void Slider::render() const {
	if (w < 2 || h < 2)
		return;

	EngineInstance* inst = EngineInstance::getInstance();

	// Bbox
	drawSquare(x, y, w, h, outline_color);
	drawSquare(x + 1, y + 1, w - 2, h - 2, background_color);

	// Slider
	const size_t slider_x = 1 + x + static_cast<double>(w - 3) * (static_cast<double>(cur_val - min_val) / static_cast<double>(max_val - min_val));
	drawSquare(slider_x, y + 1, 1, h - 2, active() ? selected_slider_color : slider_color);
}

void Slider::onUpArrow() {
	cur_val += step;
	if (cur_val > max_val)
		cur_val = max_val;

	render();
	EngineInstance::getInstance()->tick();
}

void Slider::onDownArrow() {
	if (step >= cur_val - min_val)
		cur_val = min_val;
	else
		cur_val -= step;

	render();
	EngineInstance::getInstance()->tick();
}

void CheckBox::render() const {
	if (size < 3)
		return;

	drawSquare(x, y, size, size, outline_color);
	drawSquare(x + 1, y + 1, size - 2, size - 2, is_selected ? selected_color : unselected_color);
	
	if (!text.empty()) {
		const size_t text_x = 2 * (x + size + 1);
		EngineInstance::getInstance()->drawText(text_x, y + size / 2, text, text_color, active() ? selected_color : unselected_color);
	}
}

void CheckBox::onSelect() {
	is_selected = !is_selected;
	render();
	EngineInstance::getInstance()->tick();
}

void TextBox::render() const {
	if (w < 2 || h < 2)
		return;

	drawSquare(x, y, w, h, active() ? selected_outline_color : outline_color);
	drawSquare(x + 1, y + 1, w - 2, h - 2, background_color);

	if (!text.empty()) {
		std::string rendered_text(text);
		if (text.size() > 2 * (w - 2)) // Check text bbox
			rendered_text = rendered_text.substr(text.size() - 2 * w + 4);

		EngineInstance::getInstance()->drawText(2 * (x + 1), y + h / 2, rendered_text, text_color, background_color);
	}
}

void TextBox::onChar(const char c) {
	// Backspace
	if ((c == '\b' || c == 8 || c == 127) && !text.empty())
		text.pop_back();
	else
		text += c;

	render();
	EngineInstance::getInstance()->tick();
}

void ScrollableCanvas::render() const {
	if (w < 3 || h < 3 || screen_w < 3 || screen_h < 3)
		return;

	// Render outline
	drawBox(x, y, w + 1, h + 1, active() ? selected_color : outline_color);
	drawSquare(x + 1, y + 1, w, h, Color::Default); // Clear background (TODO: maybe make this cooler and whatnot)

	// Render elements on screen
	for (const Task& task : screen) {
		if (task.rule == RenderRule::RENDER_EMPTY)
			continue;
		if (task.x < view_x || task.x - view_x >= w
			|| task.y < view_y || task.y - view_y >= h)
			continue;
		
		Task task_repositioned(task);
		task_repositioned.x = task.x + x + 1 - view_x;
		task_repositioned.y = task.y + y + 1 - view_y;
		EngineInstance::getInstance()->addToQueue(task_repositioned);
	}
}

void ScrollableCanvas::onLeftArrow() {
	if (static_cast<ssize_t>(view_x) - 1 < 0)
		return;

	--view_x;
	render();
	EngineInstance::getInstance()->tick();
}

void ScrollableCanvas::onRightArrow() {
	if (view_x + w + 1 >= screen_w)
		return;

	++view_x;
	render();
	EngineInstance::getInstance()->tick();
}

void ScrollableCanvas::onUpArrow() {
	if (static_cast<ssize_t>(view_y) - 1 < 0)
		return;

	--view_y;
	render();
	EngineInstance::getInstance()->tick();
}

void ScrollableCanvas::onDownArrow() {
	if (view_y + w + 1 >= screen_h)
		return;

	++view_y;
	render();
	EngineInstance::getInstance()->tick();
}

bool UIInstance::inputTick() {
	if (!elements.size())
		return false;

	char c = _getch();

	if (c == '\t') {
		elements[current_index]->setActive(false);
		current_index++;
		current_index %= elements.size();
		elements[current_index]->setActive(true);
		return true;
	}
	else if (c == '\n')
		elements[current_index]->onSelect();
#ifdef WIN32
	else if (c == 72)
		elements[current_index]->onUpArrow();
	else if (c == 80)
		elements[current_index]->onDownArrow();
	else if (c == 75)
		elements[current_index]->onLeftArrow();
	else if (c == 77)
		elements[current_index]->onRightArrow();
#else
	else if (c == 27) {
		_getch(); // Skip [
		c = _getch();
		if (c == 'A')
			elements[current_index]->onUpArrow();
		else if (c == 'B')
			elements[current_index]->onDownArrow();
		else if (c == 'D')
			elements[current_index]->onLeftArrow();
		else if (c == 'C')
			elements[current_index]->onRightArrow();
	}
#endif
	else
		elements[current_index]->onChar(c);

	return false;
}

void UIInstance::UITick() {
	if (inputTick()) {
		for (auto element : elements)
			element->render();
	}
}