#pragma once

#include <cstdint>

// команды дисплея
constexpr uint8_t CMD_NOP         = 0x00;
constexpr uint8_t CMD_SWRESET     = 0x01;
constexpr uint8_t CMD_RDDID       = 0x04;
constexpr uint8_t CMD_RDDST       = 0x09;
constexpr uint8_t CMD_SLPIN       = 0x10;
constexpr uint8_t CMD_SLPOUT      = 0x11;
constexpr uint8_t CMD_PTLON       = 0x12;
constexpr uint8_t CMD_NORON       = 0x13;
constexpr uint8_t CMD_INVOFF      = 0x20;
constexpr uint8_t CMD_INVON       = 0x21;
constexpr uint8_t CMD_DISPOFF     = 0x28;
constexpr uint8_t CMD_DISPON      = 0x29;
constexpr uint8_t CMD_CASET       = 0x2A;
constexpr uint8_t CMD_RASET       = 0x2B;
constexpr uint8_t CMD_RAMWR       = 0x2C;
constexpr uint8_t CMD_RAMRD       = 0x2E;
constexpr uint8_t CMD_PTLAR       = 0x30;
constexpr uint8_t CMD_COLMOD      = 0x3A;
constexpr uint8_t CMD_MADCTL      = 0x36;
constexpr uint8_t CMD_FRMCTR1     = 0xB1;
constexpr uint8_t CMD_FRMCTR2     = 0xB2;
constexpr uint8_t CMD_FRMCTR3     = 0xB3;
constexpr uint8_t CMD_INVCTR      = 0xB4;
constexpr uint8_t CMD_DISSET5     = 0xB6;
constexpr uint8_t CMD_PWCTR1      = 0xC0;
constexpr uint8_t CMD_PWCTR2      = 0xC1;
constexpr uint8_t CMD_PWCTR3      = 0xC2;
constexpr uint8_t CMD_PWCTR4      = 0xC3;
constexpr uint8_t CMD_PWCTR5      = 0xC4;
constexpr uint8_t CMD_VMCTR1      = 0xC5;
constexpr uint8_t CMD_VMOFCTR     = 0xC7;
constexpr uint8_t CMD_WRID2       = 0xD1;
constexpr uint8_t CMD_WRID3       = 0xD2;
constexpr uint8_t CMD_NVCTR1      = 0xD9;
constexpr uint8_t CMD_NVCTR2      = 0xDE;
constexpr uint8_t CMD_NVCTR3      = 0xDF;
constexpr uint8_t CMD_GAMCTRP1    = 0xE0;
constexpr uint8_t CMD_GAMCTRN1    = 0xE1;

// команды для рисования
constexpr uint8_t CMD_CLEAR_SCREEN = 0x40;
constexpr uint8_t CMD_DRAW_PIXEL   = 0x41;
constexpr uint8_t CMD_DRAW_LINE    = 0x42;
constexpr uint8_t CMD_DRAW_RECT    = 0x43;
constexpr uint8_t CMD_FILL_RECT    = 0x44;
constexpr uint8_t CMD_DRAW_CIRCLE  = 0x45;
constexpr uint8_t CMD_FILL_CIRCLE  = 0x46;
constexpr uint8_t CMD_DRAW_TEXT    = 0x47;
constexpr uint8_t CMD_DRAW_IMAGE   = 0x48;
constexpr uint8_t CMD_SET_ROTATION = 0x49;
constexpr uint8_t CMD_SET_FONT     = 0x4A; 