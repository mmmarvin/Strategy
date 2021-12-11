TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

include($${PWD}/../Game/Strategy.pri)
include($${PWD}/../GameObjects.pri)

HEADERS += \
    basearchive.h \
    basearchive_def.h

SOURCES += \
    main.cpp \
    basearchive.cpp

OBJECTS += \
    $${GAME_OBJECT_PATH}/debug_log.o \
    $${GAME_OBJECT_PATH}/string_utility.o \
    $${GAME_OBJECT_PATH}/type_common.o
