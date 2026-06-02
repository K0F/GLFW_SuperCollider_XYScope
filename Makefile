# Variables
CXX = g++
CXXFLAGS = -O3 -Wall

# SuperCollider Include Paths
SC_DIR = /usr/include/SuperCollider
SC_INCLUDES = -I $(SC_DIR)/plugin_interface \
              -I $(SC_DIR)/common \
              -I $(SC_DIR)

# Output Targets
PLUGIN = XYScope.so
VIEWER = scope_viewer

# Default target runs everything
all: $(PLUGIN) $(VIEWER)

# Compile the SuperCollider UGen Plugin
$(PLUGIN): XYScope.cpp
	$(CXX) -shared -fPIC $(CXXFLAGS) $(SC_INCLUDES) $< -o $@ -lrt

# Compile the GLFW/OpenGL Viewer
$(VIEWER): viewer.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -lglfw -lGL -lrt

# Clean up build binaries
clean:
	rm -f $(PLUGIN) $(VIEWER)

# Only non-file administrative commands should be PHONY
.PHONY: all clean