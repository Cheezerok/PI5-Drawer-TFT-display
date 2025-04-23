# TFT Display for Raspberry Pi

Проект для управления TFT дисплеем 1.8" (128x160) с контроллером ST7735S через SPI интерфейс на Raspberry Pi.

## Требования

- Raspberry Pi (тестировалось на Raspberry Pi 5)
- TFT дисплей 1.8" с контроллером ST7735S
- Соединительные провода
- Установленный wiringPi

## Подключение дисплея к Raspberry Pi

Подключите дисплей к Raspberry Pi следующим образом:

| Дисплей | Raspberry Pi |
|---------|--------------|
| VCC     | 3.3V         |
| GND     | GND          |
| CS      | GPIO 8 (CE0) |
| RESET   | GPIO 25      |
| DC      | GPIO 24      |
| MOSI    | GPIO 10 (MOSI)|
| SCK     | GPIO 11 (SCLK)|
| LED     | 3.3V         |

## Установка зависимостей

```bash
# Установка wiringPi
sudo apt-get update
sudo apt-get install wiringpi
```

## Сборка проекта

```bash
# Создание директории для сборки
mkdir build
cd build

# Генерация файлов сборки
cmake ..

# Сборка проекта
make
```

## Запуск

```bash
# Запуск демонстрационной программы
sudo ./tft_display
```

Примечание: Программа требует прав суперпользователя для доступа к GPIO.

## Структура проекта

```
.
├── CMakeLists.txt
├── README.md
├── include/
│   ├── common/
│   │   ├── colors.h
│   │   ├── commands.h
│   │   └── display_types.h
│   └── pi/
│       ├── display_pi.h
│       └── spi_pi.h
└── src/
    ├── main.cpp
    ├── display_pi.cpp
    └── spi_pi.cpp
```

## Использование в своем проекте

1. Скопируйте файлы из директории `include/` в свой проект
2. Подключите необходимые заголовочные файлы:
```cpp
#include "pi/display_pi.h"
#include "common/colors.h"
```

3. Создайте экземпляр дисплея:
```cpp
tft_display_pi::TFTDisplay display(DC_PIN, RST_PIN, CS_PIN, WIDTH, HEIGHT);
```

4. Инициализируйте дисплей:
```cpp
if (!display.init()) {
    // Обработка ошибки
}
```

5. Используйте методы для рисования:
```cpp
display.clearScreen(colors::BLACK);
display.drawLine(0, 0, 100, 100, colors::RED);
display.drawText(10, 10, "Hello!", colors::WHITE, font);
```

## Примечания

- Для работы с GPIO требуются права суперпользователя
- Рекомендуется использовать стабилизированный источник питания 3.3V
- При первом запуске может потребоваться настройка прав доступа к GPIO 