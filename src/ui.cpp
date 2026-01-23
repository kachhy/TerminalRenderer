#include "ui.h"

#ifdef WIN32
#include <conio.h>
#else
#include <termios.h>

char _getch() {
	char buf = 0;
	struct termios old = { 0 };
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON; // Disable canonical mode (buffering)
	old.c_lflag &= ~ECHO;	// Disable echoing key to screen
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
	return buf;
}
#endif

UIInstance* UIInstance::single = nullptr;

void Button::render(
	const size_t x,
	const size_t y,
	const size_t w,
	const size_t h,
	const Color base_color,
	const Color selected_color,
	const Color text_color,
	const std::string& text
) {
	if (w < 2 || h < 2)
		return;

	EngineInstance* inst = EngineInstance::getInstance();
	const size_t text_x = x + w / 2 - text.size() / 2;

	drawSquare(x, y, w, h, active() ? base_color : selected_color);
	drawSquare(x + 1, y + 1, w - 2, h - 2, active() ? selected_color : base_color);
	if (!text.empty())
		inst->drawText(text_x, y + h / 2, text, text_color, active() ? selected_color : base_color);
}

bool UIInstance::inputTick() {
	if (!elements.size())
		return false;

	char c = _getch();

	if (c == '\t') {
		elements[current_index].setActive(false);
		current_index++;
		current_index %= elements.size();
		elements[current_index].setActive(true);
		return true;
	}

	return false;
}