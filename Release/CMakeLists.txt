cmake_minimum_required(VERSION 3.10)
project(TFT_Display)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(PIGPIO REQUIRED pigpio)
pkg_check_modules(GIF REQUIRED giflib)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Add executable
add_executable(tft_display 
    src/main.cpp
    src/display_pi.cpp
    src/spi_pi.cpp
    src/tool_panel.cpp
    src/canvas.cpp
)

# Link libraries
target_link_libraries(tft_display 
    ${PIGPIO_LIBRARIES}
    ${GIF_LIBRARIES}
    ${GTKMM_LIBRARIES}
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(tft_display PRIVATE 
    ${PIGPIO_INCLUDE_DIRS}
    ${GIF_INCLUDE_DIRS}
    ${GTKMM_INCLUDE_DIRS}
    ${SFML_INCLUDE_DIRS}
)

# Add compiler flags
target_compile_options(tft_display PRIVATE -Wall -Wextra) 