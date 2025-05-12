#pragma once

#include <cstdint>
#include <string>
#include <gpiod.h>
#include <pigpio.h>

class SPIDevice {
private:
    int spi_channel;
    int spi_speed;
    int spi_handle;
    struct gpiod_chip *chip;
    struct gpiod_line *dc_line;
    struct gpiod_line *rst_line;
    
public:
    SPIDevice(int channel = 0, int speed = 8000000);
    ~SPIDevice();
    
    bool init();
    void write(uint8_t* data, size_t length);
    void setDC(bool state);
    void setRST(bool state);
    void delay(uint32_t ms);
}; 