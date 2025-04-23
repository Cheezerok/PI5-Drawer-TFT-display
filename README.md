# TFT Display Drawing Application for Raspberry Pi 5

Проект для создания приложения рисования с использованием TFT дисплея 1.8" (128x160) с контроллером ST7735S через SPI интерфейс на Raspberry Pi 5.

## Требования

- Raspberry Pi 5
- TFT дисплей 1.8" с контроллером ST7735S
- Соединительные провода
- Клавиатура и мышь (для работы с интерфейсом)
- Монитор (для отображения интерфейса)

## Подключение дисплея к Raspberry Pi 5

Подключите дисплей к Raspberry Pi следующим образом:

| Дисплей | Raspberry Pi 5 |
|---------|----------------|
| VCC     | 3.3V (Pin 1)   |
| GND     | GND (Pin 6)    |
| CS      | CE0 (Pin 24)   |
| RESET   | GPIO 25 (Pin 22)|
| DC      | GPIO 24 (Pin 18)|
| MOSI    | MOSI (Pin 19)  |
| SCK     | SCLK (Pin 23)  |
| LED     | 3.3V (Pin 1)   |

## Установка зависимостей и настройка системы

```bash
# Обновление системы
sudo apt-get update
sudo apt-get upgrade -y

# Установка инструментов разработки
sudo apt-get install -y build-essential cmake pkg-config

# Установка зависимостей для SFML
sudo apt-get install -y libsfml-dev

# Установка зависимостей для GTK и giflib
sudo apt-get install -y libgtkmm-3.0-dev libgif-dev

# Установка pigpio для работы с GPIO
sudo apt-get install -y pigpio

# Установка библиотек для работы с графикой
sudo apt-get install -y libjpeg-dev libpng-dev libtiff-dev
sudo apt-get install -y libsdl2-dev libsdl2-image-dev

# Установка Python библиотек для тестирования
sudo apt-get install -y python3-spidev python3-pip
sudo pip3 install adafruit-circuitpython-st7735r

# Настройка SPI
sudo raspi-config
# В меню выбираем:
# 1. Interface Options
# 2. SPI -> Yes
# 3. I2C -> Yes (опционально)
```

## Настройка прав доступа

```bash
# Добавляем пользователя в необходимые группы
sudo usermod -a -G spi,gpio pi

# Перезагрузка для применения изменений
sudo reboot
```

## Сборка проекта

```bash
# Создание рабочей директории
mkdir -p ~/Pi_Draw
cd ~/Pi_Draw

# Клонирование проекта (если используется git)
# git clone <URL репозитория> .

# Создание директории для сборки
mkdir Release
cd Release

# Сборка проекта
cmake ..
make
```

## Запуск приложения

```bash
# Запуск с правами суперпользователя
sudo ./tft_display
```

## Автозапуск приложения

```bash
# Создание сервиса
sudo nano /etc/systemd/system/tft-display.service

# Добавление содержимого:
[Unit]
Description=TFT Display Drawing Application
After=network.target

[Service]
ExecStart=/home/pi/Pi_Draw/Release/tft_display
WorkingDirectory=/home/pi/Pi_Draw/Release
User=pi
Group=pi
Restart=always

[Install]
WantedBy=multi-user.target

# Включение и запуск сервиса
sudo systemctl enable tft-display.service
sudo systemctl start tft-display.service
```

## Функциональность приложения

- Рисование линий, прямоугольников, кругов
- Выбор цвета и толщины линии
- Загрузка фоновых изображений (BMP, GIF)
- Изменение цвета фона
- Панель инструментов с предпросмотром
- Рабочая область для рисования

## Структура проекта

```
.
├── CMakeLists.txt
├── README.md
├── include/
│   ├── colors.h
│   ├── commands.h
│   ├── display_types.h
│   ├── display_pi.h
│   ├── spi_pi.h
│   ├── tools.h
│   ├── tool_panel.h
│   ├── canvas.h
│   └── file_dialog.h
└── src/
    ├── main.cpp
    ├── display_pi.cpp
    ├── spi_pi.cpp
    ├── tool_panel.cpp
    └── canvas.cpp
```

## Устранение неполадок

1. **Дисплей не включается**:
   - Проверьте подключение проводов
   - Убедитесь, что напряжение питания 3.3V
   - Проверьте настройки SPI в raspi-config

2. **Нет доступа к GPIO**:
   - Проверьте членство в группах spi и gpio
   - Перезагрузите систему

3. **Проблемы с интерфейсом**:
   - Убедитесь, что установлены все зависимости SFML и GTK
   - Проверьте подключение клавиатуры и мыши

4. **Проблемы с загрузкой изображений**:
   - Проверьте права доступа к файлам
   - Убедитесь, что установлены все графические библиотеки

## Примечания

- Для работы с GPIO требуются права суперпользователя
- Рекомендуется использовать стабилизированный источник питания 3.3V
- При работе с SSH используйте флаг -X для X11 forwarding
- Для тестирования дисплея можно использовать Python скрипты из библиотеки Adafruit 
