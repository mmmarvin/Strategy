TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

include(Strategy.pri)
include($${PWD}/../GameObjects.pri)

HEADERS += \
    src/core/archive/basearchive.h \
    src/core/archive/basearchive_def.h

SOURCES += \
    test/main_test.cpp \
    src/core/archive/basearchive.cpp

OBJECTS += \
    $${GAME_OBJECT_PATH}/animation2d.o \
    $${GAME_OBJECT_PATH}/animationset2d.o \
    $${GAME_OBJECT_PATH}/animationset2dlookup.o \
    $${GAME_OBJECT_PATH}/attack_data.o \
    $${GAME_OBJECT_PATH}/base_data.o \
    $${GAME_OBJECT_PATH}/console.o \
    $${GAME_OBJECT_PATH}/classconsolewriter.o \
    $${GAME_OBJECT_PATH}/command_utility.o \
    $${GAME_OBJECT_PATH}/controlmap.o \
    $${GAME_OBJECT_PATH}/config_utility.o \
    $${GAME_OBJECT_PATH}/configuration.o \
    $${GAME_OBJECT_PATH}/console_message_common.o \
    $${GAME_OBJECT_PATH}/entitydatalookup.o \
    $${GAME_OBJECT_PATH}/game.o \
    $${GAME_OBJECT_PATH}/gamestate.o \
    $${GAME_OBJECT_PATH}/high_resolution_clock.o \
    $${GAME_OBJECT_PATH}/imgui-SFML.o \
    $${GAME_OBJECT_PATH}/inputstate.o \
    $${GAME_OBJECT_PATH}/logger.o \
    $${GAME_OBJECT_PATH}/pak_utility.o \
    $${GAME_OBJECT_PATH}/sfmlinputhandler.o \
    $${GAME_OBJECT_PATH}/sfmlrenderer.o \
    $${GAME_OBJECT_PATH}/stream_utility.o \
    $${GAME_OBJECT_PATH}/string_utility.o \
    $${GAME_OBJECT_PATH}/text2dstyle.o \
    $${GAME_OBJECT_PATH}/texturelookup.o \
    $${GAME_OBJECT_PATH}/time.o \
    $${GAME_OBJECT_PATH}/tinyxml2.o \
    $${GAME_OBJECT_PATH}/type_common.o \
    $${GAME_OBJECT_PATH}/type_determine_utility.o \
    $${GAME_OBJECT_PATH}/variable_common.o \
    $${GAME_OBJECT_PATH}/variablemanager.o \
    $${GAME_OBJECT_PATH}/vertex2.o

