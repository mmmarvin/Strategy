TARGET = libimgui.a

CXX_COMPILER = g++
CXX_FLAGS = -std=c++17
CXX_LIB_COMPILER = ar
CXX_LIB_COMPILER_FLAGS = rcs

IMGUI_LOCATION = ../../src/core/3rdparty/imgui/
IMGUI_OBJECTS = imgui.o imgui_draw.o imgui_widgets.o imgui_stdlib.o

IMGUI_HEADER_LOCATION = ../../src/core/3rdparty/imgui/
IMGUI_SFML_HEADER_LOCATION = ../../src/core/3rdparty/imgui-sfml/
SFML_HEADER_LOCATION = /home/M/userspace/shared/include/

all: ${IMGUI_OBJECTS}
	${CXX_LIB_COMPILER} ${CXX_LIB_COMPILER_FLAGS} ${TARGET} ${IMGUI_OBJECTS}
	rm *.o

imgui.o:
	${CXX_COMPILER} -I${SFML_HEADER_LOCATION} -I${IMGUI_SFML_HEADER_LOCATION} ${CXX_FLAGS} -c ${IMGUI_LOCATION}/imgui.cpp

imgui_draw.o:
	${CXX_COMPILER} -I${SFML_HEADER_LOCATION} -I${IMGUI_SFML_HEADER_LOCATION} ${CXX_FLAGS} -c ${IMGUI_LOCATION}/imgui_draw.cpp

imgui_widgets.o:
	${CXX_COMPILER} -I${SFML_HEADER_LOCATION} -I${IMGUI_SFML_HEADER_LOCATION} ${CXX_FLAGS} -c ${IMGUI_LOCATION}/imgui_widgets.cpp

imgui_stdlib.o:
	${CXX_COMPILER} -I${SFML_HEADER_LOCATION} -I${IMGUI_HEADER_LOCATION} -I${IMGUI_SFML_HEADER_LOCATION} ${CXX_FLAGS} -c ${IMGUI_LOCATION}/misc/cpp/imgui_stdlib.cpp
