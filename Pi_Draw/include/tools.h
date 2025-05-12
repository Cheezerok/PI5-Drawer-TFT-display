#ifndef TOOLS_H
#define TOOLS_H

#include "colors.h"
#include <cstdint>
#include <string>
#include <vector>

enum class Tool {
    Rectangle,
    Line,
    Circle,
    Pencil,
    Eraser,
    Background,
    Image
};

struct Point {
    int16_t x;
    int16_t y;
};

struct ImageData {
    std::vector<uint16_t> pixels;
    uint16_t width;
    uint16_t height;
};

class DrawingProperties {
public:
    Tool currentTool = Tool::Pencil;
    uint16_t color = COLOR_BLACK;
    uint8_t lineWidth = 1;
    bool filled = false;
    Point startPoint = {0, 0};
    Point endPoint = {0, 0};
    bool isDrawing = false;
    uint16_t backgroundColor = COLOR_WHITE;
    ImageData backgroundImage;
    bool hasBackgroundImage = false;
};

#endif // TOOLS_H 