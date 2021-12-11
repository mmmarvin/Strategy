TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DEPRECATED_WARNINGS

include($${PWD}/../Game/Strategy.pri)
include($${PWD}/../GameObjects.pri)

HEADERS += \
    src/mapeditorcanvas.h \
    src/qsfmlcanvas.h \
    src/mapeditorwindow.h \
    src/map_editor_common.h \
    src/map_editor_config.h \
    src/objecttoolbox.h \
    src/tilesettoolbox.h

SOURCES += \
    main.cpp \
    src/mapeditorcanvas.cpp \
    src/qsfmlcanvas.cpp \
    src/mapeditorwindow.cpp \
    src/map_editor_common.cpp \
    src/objecttoolbox.cpp \
    src/tilesettoolbox.cpp

OBJECTS += \
    $${GAME_OBJECT_PATH}/basearchive.o \
    $${GAME_OBJECT_PATH}/classconsolewriter.o \
    $${GAME_OBJECT_PATH}/command_utility.o \
    $${GAME_OBJECT_PATH}/config_utility.o \
    $${GAME_OBJECT_PATH}/controlmap.o \
    $${GAME_OBJECT_PATH}/console_message_common.o \
    $${GAME_OBJECT_PATH}/console.o \
    $${GAME_OBJECT_PATH}/game.o \
    $${GAME_OBJECT_PATH}/gamestate.o \
    $${GAME_OBJECT_PATH}/high_resolution_clock.o \
    $${GAME_OBJECT_PATH}/imgui-SFML.o \
    $${GAME_OBJECT_PATH}/inputstate.o \
    $${GAME_OBJECT_PATH}/json_utility.o \
    $${GAME_OBJECT_PATH}/logger.o \
    $${GAME_OBJECT_PATH}/map_common.o \
    $${GAME_OBJECT_PATH}/mapproperty.o \
    $${GAME_OBJECT_PATH}/map.o \
    $${GAME_OBJECT_PATH}/sfmlrenderer.o \
    $${GAME_OBJECT_PATH}/sfmlinputhandler.o \
    $${GAME_OBJECT_PATH}/sfmlvertex2.o \
    $${GAME_OBJECT_PATH}/stream_utility.o \
    $${GAME_OBJECT_PATH}/string_utility.o \
    $${GAME_OBJECT_PATH}/stdinputstream.o \
    $${GAME_OBJECT_PATH}/texture_pak_utility.o \
    $${GAME_OBJECT_PATH}/text2dstyle.o \
    $${GAME_OBJECT_PATH}/tileinfo.o \
    $${GAME_OBJECT_PATH}/time.o \
    $${GAME_OBJECT_PATH}/type_common.o \
    $${GAME_OBJECT_PATH}/variable_common.o \
    $${GAME_OBJECT_PATH}/variablemanager.o \
    $${GAME_OBJECT_PATH}/vector_utility.o
