#include "trender.h"

#include <chrono>
#include <thread>
#include <fstream>

void perfTest(EngineInstance* inst) {
    inst->setBackground(Color::Black);
    
    for (size_t i = 0; i < 10000; ++i) {
        inst->drawLine(20, 20, (i % 3) * (rand() % 10),(i % 3) * (rand() % 10), Color::Red);
        inst->tick();
        inst->clear();
        if (i % 100 == 0)
            inst->clear();
    }
}

void fpsTest(EngineInstance* inst) {
    std::chrono::time_point<std::chrono::high_resolution_clock> m_beg { std::chrono::high_resolution_clock::now() };

    for (size_t i = 0; i < 100; ++i) {
        inst->setBackground(static_cast<Color>(static_cast<int>(Color::Black) + i % 7));
        inst->tick();
    }

    auto time = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1> >>(std::chrono::high_resolution_clock::now() - m_beg).count();

    std::ofstream out("fps.txt");
    out << "FPS: " << 100 / time << std::endl;
}

int main() {
    // for (size_t i = 0; i < 100000; ++i) {
    //     std::cout << "\033[10;10H" << "\033[" << 41 + (i % 2) << "m  \033[0m" << std::flush;
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }
    // Hide cursor
    std::cout << "\033[?25l";
    EngineInstance* inst = EngineInstance::getInstance();
    // Spinning cube demo
    using std::sin; using std::cos;
    const float PI = 3.14159265f;
    // Cube vertices
    float cube[8][3] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}
    };
    // Cube edges (pairs of vertex indices)
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, // back face
        {4,5},{5,6},{6,7},{7,4}, // front face
        {0,4},{1,5},{2,6},{3,7}  // sides
    };
    float angle = 0.0f;
    size_t w = inst->termWidth();
    size_t h = inst->termHeight();
    float scale = (std::min)(w, h) * 0.4f; // Increased scale for a bigger cube
    size_t cx = w / 2, cy = h / 2;
    inst->setBackground(Color::Black);
    // 3x5 font for digits 0-9
    const char* digits[10] = {
        "111101101101111", // 0
        "001001001001001", // 1
        "111001111100111", // 2
        "111001111001111", // 3
        "101101111001001", // 4
        "111100111001111", // 5
        "111100111101111", // 6
        "111001001001001", // 7
        "111101111101111", // 8
        "111101111001111"  // 9
    };
    auto drawDigit = [&](int digit, int ox, int oy, Color color) {
        if (digit < 0 || digit > 9) return;
        const char* pat = digits[digit];
        for (int y = 0; y < 5; ++y) {
            for (int x = 0; x < 3; ++x) {
                if (pat[y * 3 + x] == '1')
                    inst->addToQueue(ox + x, oy + y, color);
            }
        }
    };
    using clock = std::chrono::high_resolution_clock;
    constexpr int FPS_SAMPLES = 32;
    double frameTimes[FPS_SAMPLES] = {0};
    int frameIdx = 0;
    auto lastFrame = clock::now();
    for (int frame = 0; frame < 1000000; ++frame) {
        inst->clear();
        // Rolling buffer FPS calculation
        auto now = clock::now();
        double dt = std::chrono::duration<double>(now - lastFrame).count();
        lastFrame = now;
        frameTimes[frameIdx % FPS_SAMPLES] = dt;
        frameIdx++;
        int samples = (frameIdx < FPS_SAMPLES) ? frameIdx : FPS_SAMPLES;
        double total = 0.0;
        for (int i = 0; i < samples; ++i) total += frameTimes[i];
        double fps = (total > 0.0) ? (samples / total) : 0.0;
        // Print FPS in top left using cout
        int ifps = static_cast<int>(fps + 0.5);
        std::cout << "\033[1;12H" << "FPS: " << ifps << "    \033[2J\033[H";
        // Rotated and projected points
        float proj[8][2];
        for (int i = 0; i < 8; ++i) {
            float x = cube[i][0], y = cube[i][1], z = cube[i][2];
            // Rotate around Y axis
            float x1 = x * cos(angle) - z * sin(angle);
            float z1 = x * sin(angle) + z * cos(angle);
            // Rotate around X axis
            float y1 = y * cos(angle * 0.7f) - z1 * sin(angle * 0.7f);
            float z2 = y * sin(angle * 0.7f) + z1 * cos(angle * 0.7f);
            // Perspective projection
            float d = 3.5f;
            float px = x1 / (z2 + d);
            float py = y1 / (z2 + d);
            proj[i][0] = cx + px * scale;
            proj[i][1] = cy + py * scale;
        }
        // Draw edges
        for (int e = 0; e < 12; ++e) {
            int a = edges[e][0], b = edges[e][1];
            inst->drawLine((size_t)proj[a][0], (size_t)proj[a][1], (size_t)proj[b][0], (size_t)proj[b][1], Color::BrightCyan);
        }
        inst->tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        angle += 0.05f;
    }
    // Show cursor before exit
    std::cout << "\033[?25h" << std::flush;
    return 0;
}