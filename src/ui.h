#ifndef UI_H
#define UI_H

#include "engine.h"

class UIElement {
public:
    UIElement();
    virtual bool active() {}
    virtual void render(const size_t x, const size_t y) {}
};

class Button : public UIElement {
public:
    Button() { UIElement(); }
    void render(const size_t x, const size_t y); // override
};

class UIInstance {
private:
    UIInstance();
    static UIInstance* single;
public:
    static UIInstance* GetInstance() {
        if (!single)
            single = new UIInstance();
        
        return single;
    }

    void inputTick() const;
};

#endif // UI_H