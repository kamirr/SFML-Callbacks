TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
# SOURCES += example1.cpp
# SOURCES += udp_example.cpp
# SOURCES += tcp_example.cpp
# SOURCES += tcp_listener_example.cpp

QMAKE_CXXFLAGS += -std=c++17
LIBS += -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system -lpthread

HEADERS += \
    SFCB/Callback.hpp \
    SFCB/UdpSocket.hpp \
    SFCB/Context.hpp \
    SFCB/Window.hpp \
    SFCB/TcpSocket.hpp \
    SFCB/NetworkBase.hpp \
    SFCB/TcpListener.hpp \
    SFCB/ConcurrentQueue.hpp \
    SFCB/ResourceLoader.hpp \
    SFCB/Base.hpp
