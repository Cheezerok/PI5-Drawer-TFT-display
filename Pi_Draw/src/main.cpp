#include "display_pi.h"
#include "tool_panel.h"
#include "canvas.h"
#include <SFML/Graphics.hpp>
#include <string>

int main() {
    // Initialize display
    TFTDisplay display;
    display.init();
    display.setRotation(1);
    display.fillScreen(COLOR_WHITE);

    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Drawing Application");
    window.setFramerateLimit(60);

    // Create tool panel and canvas
    ToolPanel toolPanel(sf::Vector2f(10, 10), sf::Vector2f(150, 580));
    Canvas canvas(sf::Vector2f(170, 10), sf::Vector2f(620, 580), display);
    
    // Initialize drawing properties
    DrawingProperties props;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            toolPanel.handleEvent(event, props);
            canvas.handleEvent(event, props);
        }

        window.clear(sf::Color(240, 240, 240));
        
        toolPanel.draw(window);
        canvas.draw(window);
        
        window.display();
    }

    return 0;
} 
