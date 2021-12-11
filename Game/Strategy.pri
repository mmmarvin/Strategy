HOME = $$system(echo $HOME)
INCLUDE_PATH_EASTL = $${HOME}/userspace/include/EASTL
INCLUDE_PATH_ANAX = $${HOME}/userspace/include/
INCLUDE_PATH_SFML = $${HOME}/userspace/shared/include
INCLUDE_PATH_ZIPLIB = $${HOME}/userspace/shared/include
INCLUDE_PATH_JSONXX = $${HOME}/userspace/include
INCLUDE_PATH_INFOWARE = $${HOME}/userspace/include
INCLUDE_PATH_SKA = $${HOME}/userspace/include
INCLUDE_PATH_IMGUI = $${PWD}/src/core/3rdparty/imgui/
INCLUDE_PATH_IMGUISFML = $${PWD}/src/core/3rdparty/imgui-sfml/
INCLUDE_PATH_SOL = $${PWD}/src/core/3rdparty/sol3

# EASTL
LIB_PATH_EASTL = $${HOME}/userspace/lib64
LIB_NAME_EASTL = libEASTL.a

# Anax
LIB_PATH_ANAX = $${HOME}/userspace/lib64
LIB_NAME_ANAX = libanax.a

# SFML
LIB_PATH_SFML = $${HOME}/userspace/shared/lib64
LIB_NAME_SFML_GRAPHICS = libsfml-graphics.so
LIB_NAME_SFML_WINDOW = libsfml-window.so
LIB_NAME_SFML_SYSTEM = libsfml-system.so
LIB_NAME_SFML_AUDIO = libsfml-audio.so

# ZipLib
LIB_PATH_ZIPLIB = $${HOME}/userspace/shared/lib64
LIB_NAME_ZIPLIB = libzip.so

# Jsonxx
LIB_PATH_JSONXX = $${HOME}/userspace/lib64
LIB_NAME_JSONXX = libjsonxx.a

# InfoWare
LIB_PATH_INFOWARE = $${HOME}/userspace/lib64
LIB_NAME_INFOWARE = libinfoware.a

# ImGui
LIB_PATH_IMGUI = $${PWD}/lib/imgui
LIB_NAME_IMGUI = libimgui.a

QMAKE_CXXFLAGS += \
# Compiler Warnings
    -g -O0 -Wall -pedantic  \
    -pedantic-errors -Wno-long-long -Wextra -Wcast-align \
    -Wcast-qual -Wconversion \
    -Wdisabled-optimization \
    -Wfloat-equal -Wformat=2 \
    -Wformat-nonliteral \
   #-Wformat-security  \
    -Wformat-y2k \
    -Wimplicit  -Wimport  -Winit-self \
    -Winline \
    -Winvalid-pch   \
#    -Wlong-long \ # Disable this completely
    -Wmissing-field-initializers -Wmissing-format-attribute   \
    -Wmissing-include-dirs -Wmissing-noreturn \
    -Wpacked -Wpointer-arith \
    -Wredundant-decls \
    -Wshadow -Wstack-protector \
    -Wstrict-aliasing=2 -Wswitch-default \
    -Wunreachable-code -Wunused \
    -Wunused-parameter \
    -Wvariadic-macros \
    -Wwrite-strings \
# Includes
    -isystem $${INCLUDE_PATH_EASTL} \
    -isystem $${INCLUDE_PATH_ANAX} \
    -isystem $${INCLUDE_PATH_SFML} \
    -isystem $${INCLUDE_PATH_ZIPLIB} \
    -isystem $${INCLUDE_PATH_JSONXX} \
    -isystem $${INCLUDE_PATH_INFOWARE} \
    -isystem $${INCLUDE_PATH_SKA} \
    -isystem $${INCLUDE_PATH_IMGUI} \
    -isystem $${INCLUDE_PATH_IMGUISFML} \
    -isystem $${INCLUDE_PATH_SOL}

INCLUDEPATH += \
    $${INCLUDE_PATH_EASTL} \
    $${INCLUDE_PATH_ANAX} \
    $${INCLUDE_PATH_SFML} \
    $${INCLUDE_PATH_ZIPLIB} \
    $${INCLUDE_PATH_JSONXX} \
    $${INCLUDE_PATH_INFOWARE} \
    $${INCLUDE_PATH_SKA} \
    $${INCLUDE_PATH_IMGUI} \
    $${INCLUDE_PATH_IMGUISFML} \
    $${INCLUDE_PATH_SOL}
  
LIBS += \
    -lGL -lpthread \
    $${LIB_PATH_EASTL}/$${LIB_NAME_EASTL} \
    $${LIB_PATH_ANAX}/$${LIB_NAME_ANAX} \
    $${LIB_PATH_SFML}/$${LIB_NAME_SFML_SYSTEM} \
    $${LIB_PATH_SFML}/$${LIB_NAME_SFML_WINDOW} \
    $${LIB_PATH_SFML}/$${LIB_NAME_SFML_GRAPHICS} \
    $${LIB_PATH_SFML}/$${LIB_NAME_SFML_AUDIO} \
    $${LIB_PATH_ZIPLIB}/$${LIB_NAME_ZIPLIB} \
    $${LIB_PATH_JSONXX}/$${LIB_NAME_JSONXX} \
    $${LIB_PATH_INFOWARE}/$${LIB_NAME_INFOWARE} \
    $${LIB_PATH_IMGUI}/$${LIB_NAME_IMGUI}

