QT     += core gui
CONFIG += console c++17

TARGET = Packer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
include($${PWD}/../Game/Strategy.pri)
include($${PWD}/../GameObjects.pri)

SOURCES += \
    src/main.cpp \
    src/commands.cpp

HEADERS += \
    src/commands.h \
    src/config.h

OBJECTS += \
    $${GAME_OBJECT_PATH}/basearchive.o \
    $${GAME_OBJECT_PATH}/config_utility.o \
    $${GAME_OBJECT_PATH}/string_utility.o \
    $${GAME_OBJECT_PATH}/type_common.o
