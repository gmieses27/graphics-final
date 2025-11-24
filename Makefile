# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I./external/imgui -I./external/imgui/backends -I/opt/homebrew/include
CXXFLAGS += -w
LDFLAGS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -L/opt/homebrew/lib
LIBS = -lglfw

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
IMGUI_DIR = external/imgui

# Source files
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/LSystem.cpp \
          $(SRC_DIR)/Turtle.cpp \
          $(SRC_DIR)/Renderer.cpp \
          $(IMGUI_DIR)/imgui.cpp \
          $(IMGUI_DIR)/imgui_draw.cpp \
          $(IMGUI_DIR)/imgui_tables.cpp \
          $(IMGUI_DIR)/imgui_widgets.cpp \
          $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
          $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Object files
OBJECTS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))

# Target executable
TARGET = plant_modeler

# Build rules
all: setup $(TARGET)

setup:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS)
	@echo "Build complete: ./$(TARGET)"

# Compile source files
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/LSystem.o: $(SRC_DIR)/LSystem.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/Turtle.o: $(SRC_DIR)/Turtle.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/Renderer.o: $(SRC_DIR)/Renderer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ImGui files
$(BUILD_DIR)/imgui.o: $(IMGUI_DIR)/imgui.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui_draw.o: $(IMGUI_DIR)/imgui_draw.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui_tables.o: $(IMGUI_DIR)/imgui_tables.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui_widgets.o: $(IMGUI_DIR)/imgui_widgets.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui_impl_glfw.o: $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui_impl_opengl3.o: $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all setup clean run
