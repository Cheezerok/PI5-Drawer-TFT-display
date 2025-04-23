QT -= core gui

CONFIG += c++17

SOURCES += \
    main.cpp \
    display_pi.cpp \
    spi_pi.cpp \
    draw.cpp

HEADERS += \
    colors.h \
    commands.h \
    display_pi.h \
    display_types.h \
    spi_pi.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Additional libraries for Raspberry Pi
unix {
    LIBS += -lgpiod
    LIBS += -lpigpio
    LIBS += -lX11
    LIBS += -levdev
    INCLUDEPATH += /usr/include
    INCLUDEPATH += /usr/include/X11
    LIBS += -L/usr/lib
}

# Debug configuration
debug {
    DEFINES += DEBUG
    QMAKE_CXXFLAGS += -g -Wall -Wextra
}

# Release configuration
release {
    DEFINES += NDEBUG
    QMAKE_CXXFLAGS += -O2 -Wall -Wextra
} 