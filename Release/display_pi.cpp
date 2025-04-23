#include "display_pi.h"
#include <stdexcept>
#include <chrono>
#include <thread>

TFTDisplay::TFTDisplay(int channel, int reset_pin, int dc_pin, int width, int height)
    : spi(channel), reset_pin(reset_pin), dc_pin(dc_pin),
      width(width), height(height), rotation(DisplayRotation::PORTRAIT),
      current_color(0xFFFF), current_font(Font::DEFAULT) {
}

TFTDisplay::~TFTDisplay() {
}

bool TFTDisplay::init() {
    if (!spi.init()) {
        return false;
    }

    // Hardware reset
    spi.setRST(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    spi.setRST(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(120));

    // Initialize display
    writeCommand(0x01); // Software reset
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    writeCommand(0x11); // Sleep out
    std::this_thread::sleep_for(std::chrono::milliseconds(255));

    writeCommand(0x3A); // Interface Pixel Format
    uint8_t data = 0x05; // 16-bit color
    writeData(&data, 1);

    writeCommand(0x36); // Memory Data Access Control
    data = 0x00; // Normal mode
    writeData(&data, 1);

    writeCommand(0x29); // Display on
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return true;
}

void TFTDisplay::setRotation(DisplayRotation rotation) {
    this->rotation = rotation;
    writeCommand(0x36); // Memory Data Access Control
    
    uint8_t data = 0;
    switch (rotation) {
        case DisplayRotation::PORTRAIT:
            data = 0x00;
            break;
        case DisplayRotation::LANDSCAPE:
            data = 0x60;
            break;
        case DisplayRotation::PORTRAIT_INVERTED:
            data = 0xC0;
            break;
        case DisplayRotation::LANDSCAPE_INVERTED:
            data = 0xA0;
            break;
    }
    writeData(&data, 1);
}

void TFTDisplay::clearScreen(uint16_t color) {
    fillRect(0, 0, width, height, color);
}

void TFTDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    
    setAddressWindow(x, y, x, y);
    writeData(reinterpret_cast<uint8_t*>(&color), 2);
}

void TFTDisplay::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep = (y0 < y1) ? 1 : -1;

    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void TFTDisplay::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    drawLine(x, y, x, y + h - 1, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void TFTDisplay::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (x < 0 || y < 0 || w <= 0 || h <= 0) return;
    if (x + w > width) w = width - x;
    if (y + h > height) h = height - y;

    setAddressWindow(x, y, x + w - 1, y + h - 1);
    
    uint32_t num_pixels = w * h;
    std::vector<uint16_t> buffer(num_pixels, color);
    writeData(reinterpret_cast<uint8_t*>(buffer.data()), num_pixels * 2);
}

void TFTDisplay::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void TFTDisplay::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawLine(x0, y0 - r, x0, y0 - r + 2 * r + 1, color);
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawLine(x0 + x, y0 - y, x0 + x, y0 + y, color);
        drawLine(x0 - x, y0 - y, x0 - x, y0 + y, color);
        drawLine(x0 + y, y0 - x, x0 + y, y0 + x, color);
        drawLine(x0 - y, y0 - x, x0 - y, y0 + x, color);
    }
}

void TFTDisplay::setFont(const Font& font) {
    current_font = font;
}

void TFTDisplay::drawText(int16_t x, int16_t y, const std::string& text, uint16_t color) {
    // Basic font rendering implementation
    const uint8_t* font_data = current_font.data;
    int font_width = current_font.width;
    int font_height = current_font.height;

    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (c < 32 || c > 126) continue;

        const uint8_t* char_data = &font_data[(c - 32) * font_width * font_height / 8];
        
        for (int fy = 0; fy < font_height; fy++) {
            for (int fx = 0; fx < font_width; fx++) {
                if (char_data[fy * font_width / 8 + fx / 8] & (1 << (7 - fx % 8))) {
                    drawPixel(x + i * font_width + fx, y + fy, color);
                }
            }
        }
    }
}

void TFTDisplay::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const std::vector<uint16_t>& image_data) {
    if (x < 0 || y < 0 || w <= 0 || h <= 0) return;
    if (x + w > width) w = width - x;
    if (y + h > height) h = height - y;

    setAddressWindow(x, y, x + w - 1, y + h - 1);
    writeData(reinterpret_cast<const uint8_t*>(image_data.data()), w * h * 2);
}

void TFTDisplay::writeCommand(uint8_t cmd) {
    spi.setDC(false);
    spi.write(&cmd, 1);
}

void TFTDisplay::writeData(const uint8_t* data, size_t length) {
    spi.setDC(true);
    spi.write(const_cast<uint8_t*>(data), length);
}

void TFTDisplay::setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(0x2A); // Column Address Set
    uint8_t data[4] = {
        static_cast<uint8_t>(x0 >> 8),
        static_cast<uint8_t>(x0 & 0xFF),
        static_cast<uint8_t>(x1 >> 8),
        static_cast<uint8_t>(x1 & 0xFF)
    };
    writeData(data, 4);

    writeCommand(0x2B); // Row Address Set
    data[0] = static_cast<uint8_t>(y0 >> 8);
    data[1] = static_cast<uint8_t>(y0 & 0xFF);
    data[2] = static_cast<uint8_t>(y1 >> 8);
    data[3] = static_cast<uint8_t>(y1 & 0xFF);
    writeData(data, 4);

    writeCommand(0x2C); // Memory Write
} 