#pragma once

#include "colors.h"
#include "commands.h"
#include "display_types.h"
#include "spi_pi.h"
#include <cstdint>
#include <string>
#include <vector>

class TFTDisplay {
private:
    SPIDevice spi;
    int reset_pin;
    int dc_pin;
    int width;
    int height;
    DisplayRotation rotation;
    uint16_t current_color;
    Font current_font;
    
    void writeCommand(uint8_t cmd);
    void writeData(const uint8_t* data, size_t length);
    void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    
public:
    TFTDisplay(int channel = 0, int reset_pin = 25, int dc_pin = 24, 
               int width = 128, int height = 160);
    ~TFTDisplay();
    
    bool init();
    void setRotation(DisplayRotation rotation);
    void clearScreen(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void setFont(const Font& font);
    void drawText(int16_t x, int16_t y, const std::string& text, uint16_t color);
    void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const std::vector<uint16_t>& image_data);
}; 