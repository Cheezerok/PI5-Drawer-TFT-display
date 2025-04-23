#ifndef CANVAS_H
#define CANVAS_H

#include "tools.h"
#include "display_pi.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Canvas {
public:
    Canvas(const sf::Vector2f& position, const sf::Vector2f& size, TFTDisplay& display);
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, DrawingProperties& props);
    void clear();

private:
    sf::RectangleShape canvas;
    TFTDisplay& tftDisplay;
    sf::Vector2f canvasPosition;
    sf::Vector2f canvasSize;

    void drawToDisplay(const DrawingProperties& props);
    void drawLine(const Point& start, const Point& end, uint16_t color, uint8_t width);
    void drawRectangle(const Point& start, const Point& end, uint16_t color, uint8_t width, bool filled);
    void drawCircle(const Point& center, uint16_t radius, uint16_t color, uint8_t width, bool filled);
    Point windowToCanvas(const sf::Vector2f& windowPos) const;
    bool isInsideCanvas(const sf::Vector2f& point) const;
};

#endif // CANVAS_H 