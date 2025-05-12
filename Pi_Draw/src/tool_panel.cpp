#include "tool_panel.h"
#include "file_dialog.h"
#include <gif_lib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

ToolPanel::ToolPanel(const sf::Vector2f& position, const sf::Vector2f& size) {
    panel.setPosition(position);
    panel.setSize(size);
    panel.setFillColor(sf::Color(240, 240, 240));
    panel.setOutlineColor(sf::Color(200, 200, 200));
    panel.setOutlineThickness(1.0f);

    initializeButtons();
    initializeColorPalette();

    // слайдер
    lineWidthSlider.setSize(sf::Vector2f(100.f, 10.f));
    lineWidthSlider.setPosition(position.x + 10.f, position.y + size.y - 40.f);
    lineWidthSlider.setFillColor(sf::Color::White);
    lineWidthSlider.setOutlineColor(sf::Color::Black);
    lineWidthSlider.setOutlineThickness(1.0f);
    sliderValue = 1.0f;
}

void ToolPanel::initializeButtons() {
    float buttonSize = 40.f;
    float spacing = 10.f;
    float startX = panel.getPosition().x + spacing;
    float startY = panel.getPosition().y + spacing;

    std::vector<std::pair<Tool, std::string>> tools = {
        {Tool::Pencil, "Pencil"},
        {Tool::Line, "Line"},
        {Tool::Rectangle, "Rect"},
        {Tool::Circle, "Circle"},
        {Tool::Eraser, "Eraser"},
        {Tool::Image, "Image"}
    };

    for (size_t i = 0; i < tools.size(); ++i) {
        ToolButton button;
        button.shape.setSize(sf::Vector2f(buttonSize, buttonSize));
        button.shape.setPosition(startX, startY + (buttonSize + spacing) * i);
        button.shape.setFillColor(sf::Color::White);
        button.shape.setOutlineColor(sf::Color::Black);
        button.shape.setOutlineThickness(1.0f);
        button.tool = tools[i].first;
        button.name = tools[i].second;
        toolButtons.push_back(button);
    }
}

void ToolPanel::initializeColorPalette() {
    float buttonSize = 30.f;
    float spacing = 5.f;
    float startX = panel.getPosition().x + 60.f;
    float startY = panel.getPosition().y + 200.f;

    std::vector<sf::Color> colors = {
        sf::Color::Black,
        sf::Color::White,
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Yellow
    };

    for (size_t i = 0; i < colors.size(); ++i) {
        ColorButton button;
        button.shape.setSize(sf::Vector2f(buttonSize, buttonSize));
        button.shape.setPosition(startX + (buttonSize + spacing) * (i % 3),
                               startY + (buttonSize + spacing) * (i / 3));
        button.shape.setFillColor(colors[i]);
        button.shape.setOutlineColor(sf::Color::Black);
        button.shape.setOutlineThickness(1.0f);
        colorButtons.push_back(button);
    }
}

void ToolPanel::draw(sf::RenderWindow& window) {
    window.draw(panel);
    
    for (const auto& button : toolButtons) {
        window.draw(button.shape);
    }
    
    for (const auto& button : colorButtons) {
        window.draw(button.shape);
    }
    
    window.draw(lineWidthSlider);
}

bool ToolPanel::loadGIF(const std::string& filename, ImageData& imageData) {
    int error = 0;
    GifFileType* gif = DGifOpenFileName(filename.c_str(), &error);
    
    if (!gif) {
        std::cerr << "Failed to open GIF file: " << filename << std::endl;
        return false;
    }

    if (DGifSlurp(gif) != GIF_OK) {
        std::cerr << "Failed to read GIF file" << std::endl;
        DGifCloseFile(gif, &error);
        return false;
    }

    // Get dimensions
    imageData.width = gif->SWidth;
    imageData.height = gif->SHeight;
    imageData.pixels.resize(imageData.width * imageData.height);

    // Get color map
    ColorMapObject* colorMap = gif->SColorMap;
    if (!colorMap) {
        std::cerr << "No color map in GIF" << std::endl;
        DGifCloseFile(gif, &error);
        return false;
    }

    // Process first frame
    SavedImage* frame = &gif->SavedImages[0];
    GifByteType* raster = frame->RasterBits;

    for (int y = 0; y < imageData.height; y++) {
        for (int x = 0; x < imageData.width; x++) {
            int index = raster[y * imageData.width + x];
            GifColorType color = colorMap->Colors[index];

            // Convert to RGB565
            uint16_t rgb565 = ((color.Red >> 3) << 11) |
                             ((color.Green >> 2) << 5) |
                             (color.Blue >> 3);

            imageData.pixels[y * imageData.width + x] = rgb565;
        }
    }

    DGifCloseFile(gif, &error);
    return true;
}

void ToolPanel::handleEvent(const sf::Event& event, DrawingProperties& props) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        // Check tool buttons
        for (const auto& button : toolButtons) {
            if (button.shape.getGlobalBounds().contains(mousePos)) {
                props.currentTool = button.tool;
                if (button.tool == Tool::Image) {
                    // Show file dialog
                    std::vector<std::string> filters = {"*.bmp", "*.gif"};
                    std::string filename = FileDialog::showOpenDialog(
                        "Select Image",
                        "/home/pi/Pictures",
                        filters
                    );

                    if (!filename.empty()) {
                        if (loadGIF(filename, props.imageData)) {
                            props.hasBackgroundImage = true;
                        } else {
                            std::cerr << "Failed to load image: " << filename << std::endl;
                        }
                    }
                }
                return;
            }
        }
        
        // кнопки цветов
        for (const auto& button : colorButtons) {
            if (button.shape.getGlobalBounds().contains(mousePos)) {
                props.color = button.color;
                return;
            }
        }
        
        // слайдер
        if (lineWidthSlider.getGlobalBounds().contains(mousePos)) {
            float relativeX = (mousePos.x - lineWidthSlider.getPosition().x) / lineWidthSlider.getSize().x;
            sliderValue = std::max(1.0f, std::min(10.0f, relativeX * 10.0f));
            props.lineWidth = static_cast<uint8_t>(sliderValue);
        }
    }
}

void ToolPanel::updateSlider(float value) {
    sliderValue = value;
} 