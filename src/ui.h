#ifndef UI_H
#define UI_H

#include "draw.h"

class UIElement {
private:
	bool is_active;
public:
	UIElement() : is_active(false) { }
	bool active() const { return is_active; };
	void setActive(const bool val) { is_active = val; }
};

class Button : public UIElement {
public:
	Button() = default;
	void render(
		const size_t x,
		const size_t y,
		const size_t w,
		const size_t h,
		const Color base_color,
		const Color face_color,
		const Color text_color,
		const std::string& text = ""
	);
};

class UIInstance {
private:
	UIInstance() : current_index(0) { };
	static UIInstance* single;
	size_t current_index;
public:
	std::vector<UIElement> elements; // TODO: temp

	[[nodiscard]] static UIInstance* getInstance() {
		if (!single)
			single = new UIInstance();

		return single;
	}

	bool inputTick();
};

#endif // UI_H