#include <display_types.h>
#include <colors.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <linux/input.h>
#include <fcntl.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

class DrawingApp {
private:
    tft_display_pi::TFTDisplay& display;
    int16_t cursor_x;
    int16_t cursor_y;
    uint16_t current_color;
    bool is_drawing;
    char current_char;
    tft_display::Font font;
    std::vector<std::pair<int16_t, int16_t>> drawing_points;
    bool show_cursor;
    int brush_size;
    std::atomic<bool> mouse_thread_running;
    std::thread mouse_thread;
    Display* x_display;
    Window window;
    GC gc;
    int screen_width;
    int screen_height;
    XImage* x_image;
    std::vector<uint8_t> image_buffer;
    std::mutex display_mutex;

    struct termios old_settings, new_settings;

    void setup_x11() {
        x_display = XOpenDisplay(nullptr);
        if (!x_display) {
            std::cerr << "Не удалось подключиться к серверу\n";
            return;
        }

        int screen = DefaultScreen(x_display);
        screen_width = DisplayWidth(x_display, screen);
        screen_height = DisplayHeight(x_display, screen);

        // Создаем окно
        window = XCreateSimpleWindow(x_display, RootWindow(x_display, screen),
                                    0, 0, 128, 160, 1,
                                    BlackPixel(x_display, screen),
                                    WhitePixel(x_display, screen));
        
        // Устанавливаем заголовок окна
        XStoreName(x_display, window, "TFT Display Mirror");
        
        // Показываем окно
        XMapWindow(x_display, window);
        
        // Создаем графический контекст
        gc = XCreateGC(x_display, window, 0, nullptr);
        
        //  буфер для изображения
        image_buffer.resize(128 * 160 * 4); // 4 байта на пиксель RGBA
        x_image = XCreateImage(x_display, DefaultVisual(x_display, screen),
                              24, ZPixmap, 0,
                              reinterpret_cast<char*>(image_buffer.data()),
                              128, 160, 32, 0);
    }

    void update_mirror_display() {
        if (!x_display) return;

        std::lock_guard<std::mutex> lock(display_mutex);
        
        // Очищаем буфер
        std::fill(image_buffer.begin(), image_buffer.end(), 0);
        
        // Копируем текущее изображение с TFT дисплея
        for (const auto& point : drawing_points) {
            int index = (point.second * 128 + point.first) * 4;
            uint16_t color = current_color;
            image_buffer[index] = (color >> 8) & 0xF8;     // R
            image_buffer[index + 1] = (color >> 3) & 0xFC; // G
            image_buffer[index + 2] = (color << 3) & 0xF8; // B
            image_buffer[index + 3] = 0xFF;                // A
        }
        
        // Отображаем курсор
        if (show_cursor) {
            int index = (cursor_y * 128 + cursor_x) * 4;
            image_buffer[index] = 0xFF;     // R
            image_buffer[index + 1] = 0xFF; // G
            image_buffer[index + 2] = 0xFF; // B
            image_buffer[index + 3] = 0xFF; // A
        }
        
        // Обновляем окно
        XPutImage(x_display, window, gc, x_image, 0, 0, 0, 0, 128, 160);
        XFlush(x_display);
    }

    void setup_terminal() {
        tcgetattr(STDIN_FILENO, &old_settings);
        new_settings = old_settings;
        new_settings.c_lflag &= (~ICANON & ~ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    }

    void restore_terminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    }

    void move_cursor(int dx, int dy) {
        int16_t new_x = std::max(0, std::min(127, cursor_x + dx));
        int16_t new_y = std::max(0, std::min(159, cursor_y + dy));
        
        if (new_x != cursor_x || new_y != cursor_y) {
            if (show_cursor) {
                display.drawPixel(cursor_x, cursor_y, colors::BLACK);
            }
            cursor_x = new_x;
            cursor_y = new_y;
            if (show_cursor) {
                display.drawPixel(cursor_x, cursor_y, colors::WHITE);
            }
        }
    }

    void change_color() {
        static const uint16_t colors[] = {
            colors::RED, colors::GREEN, colors::BLUE,
            colors::YELLOW, colors::CYAN, colors::MAGENTA,
            colors::WHITE, colors::BLACK
        };
        static int color_index = 0;
        color_index = (color_index + 1) % 8;
        current_color = colors[color_index];
        std::cout << "Текущий цвет: " << color_index + 1 << "/8\n";
    }

    void change_brush_size() {
        brush_size = (brush_size % 3) + 1;
        std::cout << "Размер кисти: " << brush_size << "\n";
    }

    void draw_with_brush(int16_t x, int16_t y) {
        if (brush_size == 1) {
            display.drawPixel(x, y, current_color);
        } else {
            int radius = brush_size - 1;
            display.fillCircle(x, y, radius, current_color);
        }
    }

    void print_help() {
        std::cout << "\nУправление:\n"
                  << "Стрелки - перемещение курсора\n"
                  << "Пробел - рисование/стоп\n"
                  << "c - смена цвета\n"
                  << "b - изменение размера кисти\n"
                  << "e - очистка экрана\n"
                  << "t - режим ввода текста\n"
                  << "s - показать/скрыть курсор\n"
                  << "q - выход\n"
                  << "\nУправление мышью:\n"
                  << "Левая кнопка - рисование\n"
                  << "Двойной клик левой кнопкой - смена цвета\n"
                  << "Правая кнопка - отменить последнее действие\n"
                  << "Средняя кнопка - очистка экрана\n"
                  << "Скролл - изменение размера кисти\n";
    }

    void save_drawing_point() {
        drawing_points.emplace_back(cursor_x, cursor_y);
    }

    void undo_last_action() {
        if (!drawing_points.empty()) {
            display.clearScreen(colors::BLACK);
            drawing_points.pop_back();
            for (const auto& point : drawing_points) {
                draw_with_brush(point.first, point.second);
            }
        }
    }

    void mouse_event_handler() {
        int mouse_fd = open("/dev/input/mice", O_RDONLY);
        if (mouse_fd < 0) {
            std::cerr << "Мышь не найдена\n";
            return;
        }

        unsigned char data[3];
        int left_button = 0;
        int right_button = 0;
        int middle_button = 0;
        int x = 0, y = 0;
        int last_left_click_time = 0;
        const int DOUBLE_CLICK_TIME = 300; 

        while (mouse_thread_running) {
            if (read(mouse_fd, data, sizeof(data)) == sizeof(data)) {
                left_button = data[0] & 0x1;
                right_button = data[0] & 0x2;
                middle_button = data[0] & 0x4;
                x = data[1];
                y = data[2];

                // скролл
                if (data[0] & 0x8) { // вверх
                    change_brush_size();
                } else if (data[0] & 0x10) { // вниз
                    change_brush_size();
                }

                // Масштабирование
                int16_t new_x = cursor_x + x;
                int16_t new_y = cursor_y - y;

                // Ограничиваем координаты размерами дисплея -1
                new_x = std::max(0, std::min(127, new_x));
                new_y = std::max(0, std::min(159, new_y));

                if (new_x != cursor_x || new_y != cursor_y) {
                    if (show_cursor) {
                        display.drawPixel(cursor_x, cursor_y, colors::BLACK);
                    }
                    cursor_x = new_x;
                    cursor_y = new_y;
                    if (show_cursor) {
                        display.drawPixel(cursor_x, cursor_y, colors::WHITE);
                    }
                }

                // дабл клик
                if (left_button) {
                    int current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();
                    
                    if (current_time - last_left_click_time < DOUBLE_CLICK_TIME) {
                        change_color(); // Двойной клик - смена цвета
                    }
                    last_left_click_time = current_time;
                    
                    draw_with_brush(cursor_x, cursor_y);
                    save_drawing_point();
                }
                
                if (right_button) {
                    undo_last_action();
                }

                if (middle_button) {
                    display.clearScreen(colors::BLACK);
                    drawing_points.clear();
                }
            }
        }

        close(mouse_fd);
    }

public:
    DrawingApp(tft_display_pi::TFTDisplay& disp) 
        : display(disp), cursor_x(64), cursor_y(80), 
          current_color(colors::WHITE), is_drawing(false), 
          current_char('A'), show_cursor(true), brush_size(1),
          mouse_thread_running(true), x_display(nullptr) {
        font.width = 5;
        font.height = 7;
        font.first_char = ' ';
        font.last_char = '~';
        font.data = nullptr;

        setup_x11();
        mouse_thread = std::thread(&DrawingApp::mouse_event_handler, this);
    }

    ~DrawingApp() {
        mouse_thread_running = false;
        if (mouse_thread.joinable()) {
            mouse_thread.join();
        }
        if (x_display) {
            XDestroyImage(x_image);
            XFreeGC(x_display, gc);
            XDestroyWindow(x_display, window);
            XCloseDisplay(x_display);
        }
    }

    void run() {
        setup_terminal();
        display.clearScreen(colors::BLACK);
        print_help();

        char key;
        bool running = true;
        bool text_mode = false;

        while (running) {
          
            update_mirror_display();

            //клава
            if (kbhit()) {
                key = getchar();
                
                switch (key) {
                    case 'q': // Выход
                        running = false;
                        break;

                    case ' ': // Рисование/стоп
                        is_drawing = !is_drawing;
                        if (is_drawing) {
                            save_drawing_point();
                        }
                        break;

                    case 'c': // Смена цвета
                        change_color();
                        break;

                    case 'b': // Изменение размера кисти
                        change_brush_size();
                        break;

                    case 'e': // Очистка экрана
                        display.clearScreen(colors::BLACK);
                        drawing_points.clear();
                        break;

                    case 't': // Режим ввода текста
                        text_mode = !text_mode;
                        std::cout << (text_mode ? "Режим ввода текста включен\n" : "Режим ввода текста выключен\n");
                        break;

                    case 's': // Показать/скрыть курсор
                        show_cursor = !show_cursor;
                        if (show_cursor) {
                            display.drawPixel(cursor_x, cursor_y, colors::WHITE);
                        } else {
                            display.drawPixel(cursor_x, cursor_y, colors::BLACK);
                        }
                        break;

                    case 'u': // Отменить последнее действие
                        undo_last_action();
                        break;

                    case 27: // Escape sequence для стрелок
                        if (getchar() == '[') {
                            switch (getchar()) {
                                case 'A': // Вверх
                                    move_cursor(0, -1);
                                    break;
                                case 'B': // Вниз
                                    move_cursor(0, 1);
                                    break;
                                case 'C': // Вправо
                                    move_cursor(1, 0);
                                    break;
                                case 'D': // Влево
                                    move_cursor(-1, 0);
                                    break;
                            }
                        }
                        break;

                    default:
                        if (text_mode && key >= ' ' && key <= '~') {
                            display.drawText(cursor_x, cursor_y, std::string(1, key), current_color);
                            cursor_x += font.width;
                            if (cursor_x >= 128) {
                                cursor_x = 0;
                                cursor_y += font.height;
                                if (cursor_y >= 160) cursor_y = 0;
                            }
                        }
                        break;
                }
            }

            if (is_drawing && !text_mode) {
                draw_with_brush(cursor_x, cursor_y);
                save_drawing_point();
            }

            // задержка для снижения нагрузки на CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        restore_terminal();
    }

private:
    int kbhit() {
        struct timeval tv = { 0L, 0L };
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        return select(1, &fds, NULL, NULL, &tv);
    }
}; 
