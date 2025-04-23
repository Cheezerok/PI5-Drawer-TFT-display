#include "canvas.h"
#include <cmath>

Canvas::Canvas(const sf::Vector2f& position, const sf::Vector2f& size, TFTDisplay& display)
    : tftDisplay(display), canvasPosition(position), canvasSize(size) {
    canvas.setPosition(position);
    canvas.setSize(size);
    canvas.setFillColor(sf::Color::White);
    canvas.setOutlineColor(sf::Color::Black);
    canvas.setOutlineThickness(1.0f);
}

void Canvas::draw(sf::RenderWindow& window) {
    window.draw(canvas);
}

void Canvas::handleEvent(const sf::Event& event, DrawingProperties& props) {
    if (!isInsideCanvas(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
        return;
    }

    switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left) {
                props.isDrawing = true;
                props.startPoint = windowToCanvas(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                props.endPoint = props.startPoint;
                
                if (props.currentTool == Tool::Pencil || props.currentTool == Tool::Eraser) {
                    drawToDisplay(props);
                }
            }
            break;

        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left && props.isDrawing) {
                props.isDrawing = false;
                props.endPoint = windowToCanvas(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                drawToDisplay(props);
            }
            break;

        case sf::Event::MouseMoved:
            if (props.isDrawing) {
                props.endPoint = windowToCanvas(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
                
                if (props.currentTool == Tool::Pencil || props.currentTool == Tool::Eraser) {
                    drawToDisplay(props);
                    props.startPoint = props.endPoint;
                }
            }
            break;

        default:
            break;
    }
}

void Canvas::drawToDisplay(const DrawingProperties& props) {
    uint16_t color = props.currentTool == Tool::Eraser ? COLOR_WHITE : props.color;

    switch (props.currentTool) {
        case Tool::Line:
            drawLine(props.startPoint, props.endPoint, color, props.lineWidth);
            break;

        case Tool::Rectangle:
            drawRectangle(props.startPoint, props.endPoint, color, props.lineWidth, props.filled);
            break;

        case Tool::Circle: {
            Point center = props.startPoint;
            uint16_t radius = static_cast<uint16_t>(
                std::sqrt(std::pow(props.endPoint.x - props.startPoint.x, 2) +
                         std::pow(props.endPoint.y - props.startPoint.y, 2)));
            drawCircle(center, radius, color, props.lineWidth, props.filled);
            break;
        }

        case Tool::Pencil:
        case Tool::Eraser:
            drawLine(props.startPoint, props.endPoint, color, props.lineWidth);
            break;
    }
}

void Canvas::drawLine(const Point& start, const Point& end, uint16_t color, uint8_t width) {
    int16_t dx = end.x - start.x;
    int16_t dy = end.y - start.y;
    int16_t steps = std::max(std::abs(dx), std::abs(dy));

    float xIncrement = static_cast<float>(dx) / steps;
    float yIncrement = static_cast<float>(dy) / steps;

    float x = start.x;
    float y = start.y;

    for (int16_t i = 0; i <= steps; i++) {
        for (int8_t w = 0; w < width; w++) {
            for (int8_t h = 0; h < width; h++) {
                tftDisplay.drawPixel(static_cast<int16_t>(x) + w, 
                                   static_cast<int16_t>(y) + h, 
                                   color);
            }
        }
        x += xIncrement;
        y += yIncrement;
    }
}

void Canvas::drawRectangle(const Point& start, const Point& end, uint16_t color, uint8_t width, bool filled) {
    int16_t x1 = std::min(start.x, end.x);
    int16_t y1 = std::min(start.y, end.y);
    int16_t x2 = std::max(start.x, end.x);
    int16_t y2 = std::max(start.y, end.y);

    if (filled) {
        for (int16_t y = y1; y <= y2; y++) {
            for (int16_t x = x1; x <= x2; x++) {
                tftDisplay.drawPixel(x, y, color);
            }
        }
    } else {
        // Draw horizontal lines
        for (int16_t x = x1; x <= x2; x++) {
            for (int8_t w = 0; w < width; w++) {
                tftDisplay.drawPixel(x, y1 + w, color);
                tftDisplay.drawPixel(x, y2 - w, color);
            }
        }
        // Draw vertical lines
        for (int16_t y = y1; y <= y2; y++) {
            for (int8_t w = 0; w < width; w++) {
                tftDisplay.drawPixel(x1 + w, y, color);
                tftDisplay.drawPixel(x2 - w, y, color);
            }
        }
    }
}

void Canvas::drawCircle(const Point& center, uint16_t radius, uint16_t color, uint8_t width, bool filled) {
    if (filled) {
        for (int16_t y = -radius; y <= radius; y++) {
            for (int16_t x = -radius; x <= radius; x++) {
                if (x*x + y*y <= radius*radius) {
                    tftDisplay.drawPixel(center.x + x, center.y + y, color);
                }
            }
        }
    } else {
        int16_t x = radius - 1;
        int16_t y = 0;
        int16_t dx = 1;
        int16_t dy = 1;
        int16_t err = dx - (radius << 1);

        while (x >= y) {
            for (int8_t w = 0; w < width; w++) {
                tftDisplay.drawPixel(center.x + x + w, center.y + y, color);
                tftDisplay.drawPixel(center.x + y + w, center.y + x, color);
                tftDisplay.drawPixel(center.x - y + w, center.y + x, color);
                tftDisplay.drawPixel(center.x - x + w, center.y + y, color);
                tftDisplay.drawPixel(center.x - x + w, center.y - y, color);
                tftDisplay.drawPixel(center.x - y + w, center.y - x, color);
                tftDisplay.drawPixel(center.x + y + w, center.y - x, color);
                tftDisplay.drawPixel(center.x + x + w, center.y - y, color);
            }

            if (err <= 0) {
                y++;
                err += dy;
                dy += 2;
            }
            if (err > 0) {
                x--;
                dx += 2;
                err += dx - (radius << 1);
            }
        }
    }
}

Point Canvas::windowToCanvas(const sf::Vector2f& windowPos) const {
    sf::Vector2f relativePos = windowPos - canvasPosition;
    return Point{
        static_cast<int16_t>(relativePos.x),
        static_cast<int16_t>(relativePos.y)
    };
}

bool Canvas::isInsideCanvas(const sf::Vector2f& point) const {
    return canvas.getGlobalBounds().contains(point);
}

void Canvas::clear() {
    for (int16_t y = 0; y < canvasSize.y; y++) {
        for (int16_t x = 0; x < canvasSize.x; x++) {
            tftDisplay.drawPixel(x, y, COLOR_WHITE);
        }
    }
} 