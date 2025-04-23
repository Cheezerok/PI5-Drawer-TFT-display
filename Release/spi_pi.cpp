#include "spi_pi.h"
#include <stdexcept>
#include <cstring>

SPIDevice::SPIDevice(int channel, int speed) 
    : spi_channel(channel), spi_speed(speed), spi_handle(-1),
      chip(nullptr), dc_line(nullptr), rst_line(nullptr) {
}

SPIDevice::~SPIDevice() {
    if (spi_handle >= 0) {
        spiClose(spi_handle);
    }
    if (dc_line) {
        gpiod_line_release(dc_line);
    }
    if (rst_line) {
        gpiod_line_release(rst_line);
    }
    if (chip) {
        gpiod_chip_close(chip);
    }
}

bool SPIDevice::init() {
    // Initialize pigpio
    if (gpioInitialise() < 0) {
        return false;
    }

    // Open SPI device
    spi_handle = spiOpen(spi_channel, spi_speed, 0);
    if (spi_handle < 0) {
        return false;
    }

    // Open GPIO chip
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        return false;
    }

    // Get DC and RST lines (using GPIO 24 and 25)
    dc_line = gpiod_chip_get_line(chip, 24);
    rst_line = gpiod_chip_get_line(chip, 25);
    
    if (!dc_line || !rst_line) {
        return false;
    }

    // Request lines for output
    if (gpiod_line_request_output(dc_line, "tft-dc", 0) < 0 ||
        gpiod_line_request_output(rst_line, "tft-rst", 0) < 0) {
        return false;
    }

    return true;
}

void SPIDevice::write(uint8_t* data, size_t length) {
    if (spi_handle < 0) {
        throw std::runtime_error("SPI device not initialized");
    }
    
    if (spiWrite(spi_handle, reinterpret_cast<char*>(data), length) < 0) {
        throw std::runtime_error("SPI write failed");
    }
}

void SPIDevice::setDC(bool state) {
    if (dc_line) {
        gpiod_line_set_value(dc_line, state ? 1 : 0);
    }
}

void SPIDevice::setRST(bool state) {
    if (rst_line) {
        gpiod_line_set_value(rst_line, state ? 1 : 0);
    }
}

void SPIDevice::delay(uint32_t ms) {
    gpioDelay(ms * 1000);
} 
