#pragma once

#include <cstdint>

// RGB565 цвета стандартные
constexpr uint16_t COLOR_BLACK   = 0x0000;
constexpr uint16_t COLOR_BLUE    = 0x001F;
constexpr uint16_t COLOR_RED     = 0xF800;
constexpr uint16_t COLOR_GREEN   = 0x07E0;
constexpr uint16_t COLOR_CYAN    = 0x07FF;
constexpr uint16_t COLOR_MAGENTA = 0xF81F;
constexpr uint16_t COLOR_YELLOW  = 0xFFE0;
constexpr uint16_t COLOR_WHITE   = 0xFFFF;

// вспомогательная функция для создания RGB565 цвета
constexpr uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
} 