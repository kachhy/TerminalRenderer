#ifndef UI_H
#define UI_H

#include "draw.h"

class UIElement {
protected:
	bool is_active;
public:
	UIElement() : is_active(false) { }
	bool active() const { return is_active; };
	void setActive(const bool val) { is_active = val; }
	
	virtual void render() const = 0;
	virtual bool alwaysRender() const { return false; }
	
	// Input functions
	virtual void onSelect() { };
	virtual void onLeftArrow() { };
	virtual void onRightArrow() { };
	virtual void onUpArrow() { };
	virtual void onDownArrow() { };
	virtual void onChar(const char c) { };
};

class Button : public UIElement {
protected:
	std::string text;
	void (*func)();
	size_t x, y, w, h;
	ForegroundColor text_color;
	Color default_color, selected_color, outline_color;
	bool is_active;
public:
	Button(size_t x, size_t y, size_t w, size_t h, const std::string& text, void (*func)(), ForegroundColor text_color, Color default_color, Color selected_color, Color outline_color) :
		x(x), y(y), w(w), h(h), text(text), func(func), text_color(text_color), default_color(default_color), selected_color(selected_color), outline_color(outline_color) { }

	void render() const override;
	void onSelect() override;
};

class Slider : public UIElement {
protected:
	size_t x, y, w, h;
	size_t min_val, cur_val, max_val, step; // Slider elements
	Color outline_color, background_color, slider_color, selected_slider_color;
	bool is_active;
public:
	Slider(size_t x, size_t y, size_t w, size_t h, size_t min_val, size_t max_val, size_t default_val, size_t step, Color outline_color, Color background_color, Color slider_color, Color selected_slider_color) : 
		x(x), y(y), w(w), h(h), min_val(min_val), max_val(max_val), cur_val(default_val), step(step), outline_color(outline_color), background_color(background_color), slider_color(slider_color), selected_slider_color(selected_slider_color) { }

	void render() const override;
	void onUpArrow() override;
	void onDownArrow() override;
};

class CheckBox : public UIElement {
protected:
	size_t x, y, size;
	std::string text;
	bool is_active, is_selected;
	ForegroundColor text_color;
	Color outline_color, unselected_color, selected_color; 
public:
	CheckBox(size_t x, size_t y, size_t size, const std::string& text, Color outline_color, Color unselected_color, Color selected_color, ForegroundColor text_color, bool starting_value = false) :
		x(x), y(y), size(size), text(text), outline_color(outline_color), unselected_color(unselected_color), selected_color(selected_color), text_color(text_color), is_selected(starting_value) { }

	void render() const override;
	void onSelect() override;
	bool getSelected() const { return is_selected; }
};

// TODO: implement scrolling for this
class TextBox : public UIElement {
protected:
	std::string text;
	size_t x, y, w, h;
	ForegroundColor text_color;
	Color background_color, outline_color, selected_outline_color;
	bool is_active;
public:
	TextBox(size_t x, size_t y, size_t w, size_t h, ForegroundColor text_color, Color background_color, Color selected_outline_color, Color outline_color, const std::string& default_text = "") :
		x(x), y(y), w(w), h(h), text(default_text), text_color(text_color), background_color(background_color), selected_outline_color(selected_outline_color), outline_color(outline_color) { }

	void render() const override;
	void onChar(const char c) override;
	std::string getText() const { return text; } 
};

class ScrollableCanvas : public UIElement {
protected:
	size_t x, y, w, h, screen_w, screen_h, view_x, view_y;
	Color outline_color, selected_color;
	std::vector<Task> screen;
	bool is_active;
public:
	ScrollableCanvas(size_t x, size_t y, size_t w, size_t h, size_t screen_w, size_t screen_h, Color outline_color, Color selected_color) :
		x(x), y(y), w(w), h(h), screen_w(screen_w), screen_h(screen_h), outline_color(outline_color), selected_color(selected_color), view_x(0), view_y(0)
	{
		screen.reserve(screen_w * screen_h);
		for (size_t i = 0; i < screen_h; ++i) {
			for (size_t j = 0; j < screen_w; ++j)
				screen.emplace_back(j, i, Color::Default, RenderRule::RENDER_EMPTY);
		}
	}

	void clear() {
		screen.clear();
		for (size_t i = 0; i < screen_h; ++i) {
			for (size_t j = 0; j < screen_w; ++j)
				screen.emplace_back(j, i, Color::Default, RenderRule::RENDER_EMPTY);
		}
	}

	void addToQueue(const Task& task) { screen[task.x + task.y * screen_w] = task; }
	void render() const override;
	virtual bool alwaysRender() const override { return true; }

	void onLeftArrow() override;
	void onRightArrow() override;
	void onUpArrow() override;
	void onDownArrow() override;
};

class UIInstance {
private:
	UIInstance() : current_index(0) { };
	static UIInstance* single;
	size_t current_index;
public:
	std::vector<UIElement*> elements;

	[[nodiscard]] static UIInstance* getInstance() {
		if (!single)
			single = new UIInstance();

		return single;
	}

	bool inputTick();
	void UITick();
};

#endif // UI_H