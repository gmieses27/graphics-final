#!/bin/bash

echo "Setting up Procedural Plant Modeling System..."

# Create external directory for ImGui
mkdir -p external

# Download ImGui if not present
if [ ! -d "external/imgui" ]; then
    echo "Downloading ImGui..."
    cd external
    git clone --depth 1 https://github.com/ocornut/imgui.git
    cd ..
    echo "ImGui downloaded successfully!"
else
    echo "ImGui already exists."
fi

# Check for required dependencies
echo ""
echo "Checking dependencies..."

# Check for GLFW
if ! brew list glfw &>/dev/null; then
    echo "GLFW not found. Installing via Homebrew..."
    brew install glfw
else
    echo "✓ GLFW is installed"
fi

# Check for GLM
if ! brew list glm &>/dev/null; then
    echo "GLM not found. Installing via Homebrew..."
    brew install glm
else
    echo "✓ GLM is installed"
fi

echo ""
echo "Setup complete! You can now build the project with 'make'"
echo "Run the project with 'make run' or './plant_modeler'"
