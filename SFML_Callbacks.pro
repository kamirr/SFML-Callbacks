TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

QMAKE_CXXFLAGS += -std=c++17
LIBS += -lsfml-graphics -lsfml-window -lsfml-system

HEADERS += \
    SFCB/CallbackWindow.hpp \
    SFCB/CallbackContext.hpp \
    SFCB/Callback.hpp
