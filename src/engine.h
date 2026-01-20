#ifndef ENGINE_H
#define ENGINE_H


#include <unordered_map>
#include <iostream>
#include <cstddef>
#include <string>
#include <queue>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif

enum class Color {
    Default = 49,

    Black   = 40,
    Red     = 41,
    Green   = 42,
    Yellow  = 43,
    Blue    = 44,
    Magenta = 45,
    Cyan    = 46,
    White   = 47,

    BrightBlack   = 100,
    BrightRed     = 101,
    BrightGreen   = 102,
    BrightYellow  = 103,
    BrightBlue    = 104,
    BrightMagenta = 105,
    BrightCyan    = 106,
    BrightWhite   = 107
};

enum class RenderRule {
    RENDER_DEFAULT = 0,
    RENDER_ALWAYS_ON_TOP,
    RENDER_BG,
    RENDER_TEXT,
    RENDER_EMPTY // Do nothing - used for background
};

struct Coord {
    size_t x, y;

    Coord() = default;
    Coord(const size_t x, const size_t y) : x(x), y(y) { }
    bool operator==(const Coord& other) const { return other.x == x && other.y == y; }
    Coord operator+(const Coord& other) const { return Coord(x + other.x, y + other.y); };
};

template<> struct std::hash<Coord> {
    size_t operator()(const Coord& c) const noexcept {
        return c.x ^ (c.y << 1);
    }
};

struct Task {
    std::string text;
    size_t x, y;
    Color color, foreground_color; // foreground_color is used for text only
    RenderRule rule;

    Task() = default;
    Task(const size_t x, const size_t y, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT) : x(x), y(y), color(color), rule(rule) { }
    Task(const size_t x, const size_t y, const Color color, const Color foreground_color, const std::string& text, const RenderRule rule = RenderRule::RENDER_DEFAULT) : x(x), y(y), color(color), foreground_color(foreground_color), text(text), rule(rule) { }

    Coord toCoord() const { return Coord(x, y); }
    bool operator==(const Task& other) { return x == other.x && y == other.y && color == other.color && rule == other.rule; }
};

class EngineInstance {
private:
    EngineInstance() {
        std::cout << "\033[2J"; // Clear screen

        // Reset background
        backgroundTask.rule = RenderRule::RENDER_EMPTY;
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int columns, rows;
    
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        term_width = (csbi.srWindow.Right - csbi.srWindow.Left + 1) / 2;
        term_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
        winsize w{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
            return;
        
        term_width = w.ws_col / 2;
        term_height = w.ws_row;
#endif
        screen.reserve(term_width * term_height);
        // toUpdate.reserve(term_width * term_height);
        for (size_t i = 0; i < term_height; ++i) {
            for (size_t j = 0; j < term_width; ++j) {
                screen.emplace_back(j, i, Color::Default, RenderRule::RENDER_EMPTY);
                // toUpdate.emplace_back(true);
            }
        }
    }

    // Singleton instance
    static EngineInstance* single;

    // Internal member variables
    std::unordered_map<Coord, Task> screenValues; // ONLY for non-background
    std::vector<Task> screen;
    
    Task backgroundTask;
    size_t term_width, term_height;
public:
    [[nodiscard]] static EngineInstance* getInstance() {
        if (single == nullptr)
            single = new EngineInstance();
        
        return single;
    }

    void addToQueue(const Task& task);
    template <typename... Args>
    inline void addToQueue(Args&&... args) {
        Task task = Task(std::forward<Args>(args)...);
        screen[task.x + task.y * term_width] = task;
    }
    
    void cpix(const Coord& coord);
    void fill(const Color color);
    void clear();
    void tick();

    // Info getters
    size_t termWidth() const { return term_width; }
    size_t termHeight() const { return term_height; }

    // Terminal functions
    void clearScreen() const { std::cout << "\033[2J" << std::flush; }

    // Render-rule setters
    void setBackground(const Task& task);
    void setBackground(const Color color);

    void resetBackground() { backgroundTask.rule = RenderRule::RENDER_EMPTY; }

    // Drawing tools
    void drawLine(const size_t x_0, const size_t y_0, const size_t x_1, const size_t y_1, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT);
    void drawSquare(const size_t x, const size_t y, const size_t w, const size_t h, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT);
    void drawCircle(const size_t x, const size_t y, const int r, const Color color, const RenderRule rule = RenderRule::RENDER_DEFAULT);
    void drawText(const size_t x, const size_t y, const std::string& text, const Color foreground_color, const Color background_color = Color::Default);
};

void setCharAt(const Task& tile);

inline std::ostream& operator<<(std::ostream& os, Color c) {
    return os << "\033[" << static_cast<int>(c) << "m";
}

#endif // ENGINE_H