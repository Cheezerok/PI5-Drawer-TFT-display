#ifndef TOOL_PANEL_H
#define TOOL_PANEL_H

#include "tools.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <string>

class ToolPanel {
public:
    ToolPanel(const sf::Vector2f& position, const sf::Vector2f& size);
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, DrawingProperties& props);
    bool loadImage(const std::string& filename, DrawingProperties& props);

private:
    struct ToolButton {
        sf::RectangleShape shape;
        Tool tool;
        std::string name;
    };

    struct ColorButton {
        sf::RectangleShape shape;
        uint16_t color;
    };

    sf::RectangleShape panel;
    std::vector<ToolButton> toolButtons;
    std::vector<ColorButton> colorButtons;
    sf::RectangleShape lineWidthSlider;
    float sliderValue;
    sf::Text toolText;
    sf::Font font;

    void initializeButtons();
    void initializeColorPalette();
    void updateSlider(float value);
    bool loadBMP(const std::string& filename, ImageData& imageData);
    bool loadGIF(const std::string& filename, ImageData& imageData);
};

#endif // TOOL_PANEL_H 