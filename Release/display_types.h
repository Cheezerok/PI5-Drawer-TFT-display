#pragma once

#include <cstdint>

// Display rotation
enum class DisplayRotation {
    ROTATION_0 = 0,
    ROTATION_90 = 1,
    ROTATION_180 = 2,
    ROTATION_270 = 3
};

// Point structure
struct Point {
    int16_t x;
    int16_t y;
    
    Point(int16_t x = 0, int16_t y = 0) : x(x), y(y) {}
};

// Rectangle structure
struct Rectangle {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    
    Rectangle(int16_t x = 0, int16_t y = 0, int16_t width = 0, int16_t height = 0)
        : x(x), y(y), width(width), height(height) {}
};

// Font structure
struct Font {
    uint8_t width;
    uint8_t height;
    const uint8_t* data;
    
    Font(uint8_t width = 0, uint8_t height = 0, const uint8_t* data = nullptr)
        : width(width), height(height), data(data) {}
}; 