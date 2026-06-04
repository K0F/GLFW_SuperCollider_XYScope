CXX ?= g++
CXXFLAGS += -O3 -shared -fPIC -std=c++17 -Wall -Wextra

GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
GLFW_LIBS := $(shell pkg-config --libs glfw3 2>/dev/null || echo "-lglfw")
GL_LIBS := -lGL -lOpenGL

SC_DIR_A := /usr/include/SuperCollider
SC_DIR_B := /usr/include/supercollider

INCLUDES := -I$(SC_DIR_A) -I$(SC_DIR_A)/plugin_interface -I$(SC_DIR_A)/common -I$(SC_DIR_B) -I$(SC_DIR_B)/plugin_interface -I$(SC_DIR_B)/common $(GLFW_CFLAGS)
LIBS := $(GLFW_LIBS) $(GL_LIBS)

TARGET = XYScope.so
SRCS = XYScope.cpp


.PHONY: all clean install

all: $(TARGET) viewer

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(LIBS) -o $(TARGET)

viewer: viewer.cpp
	g++ -O3 viewer.cpp -o scope_viewer -lglfw -lGL -lrt


clean:
	rm -f $(TARGET)

install: $(TARGET)
	mkdir -p ~/.local/share/SuperCollider/Extensions/
	cp $(TARGET) ~/.local/share/SuperCollider/Extensions/
