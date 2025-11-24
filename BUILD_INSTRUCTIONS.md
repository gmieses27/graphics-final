# Procedural Plant Modeling System
**Interactive Procedural Plant Modeling with L-Systems and Realistic Rendering**

## Team Members
- Jason Dixon (jkd2146)
- Jason Bbosa (jb4773)
- Gabriel Mieses (gm3132)

## Overview
This project implements a procedural plant modeling system using L-systems (Lindenmayer systems) with interactive parameter control and real-time 3D visualization. The system demonstrates the mathematical beauty of algorithmic plant generation through an intuitive GUI interface.

### Completed Milestones (Midpoint Evaluation)
✅ **Week 1**: L-system parser and 2D turtle visualization  
✅ **Week 2**: 3D geometry generation and rule expansion  
✅ **Week 3**: Interactive parameter control and rule editing  

## Features
- **L-System Engine**: Deterministic and stochastic production rules
- **2D & 3D Rendering**: Toggle between line-based and cylinder-based visualization
- **8 Plant Presets**: Simple Branch, Fractal Tree, Bush, Fern, 3D Tree, Stochastic Tree, Complex 3D Plant, Leaf Pattern
- **Interactive Parameters**:
  - Iterations (recursion depth)
  - Branching angle
  - Step length and width
  - Length/width scaling
  - Tropism (gravitational bending)
- **Custom Rule Editor**: Define your own axioms and production rules
- **Camera Controls**: Mouse-based rotation, zoom, auto-rotate mode
- **Real-time Updates**: See changes instantly as you adjust parameters

## Prerequisites

### macOS
Install dependencies using Homebrew:
```bash
brew install glfw glm
```

The setup script will automatically download ImGui.

## Build Instructions

### 1. Clone and Setup
```bash
cd graphics-final
chmod +x setup.sh
./setup.sh
```

This will:
- Download ImGui library
- Check for required dependencies (GLFW, GLM)
- Install missing dependencies via Homebrew

### 2. Build the Project
```bash
make
```

### 3. Run the Application
```bash
make run
```

Or directly:
```bash
./plant_modeler
```

### 4. Clean Build Files
```bash
make clean
```

## How to Use

### Basic Controls
- **Left Mouse Button + Drag**: Rotate camera around plant
- **Scroll Wheel**: Zoom in/out
- **UI Panels**: Adjust all parameters in real-time

### UI Panels

#### Plant Control Panel
- **Presets**: Choose from 8 predefined plant types
- **L-System Parameters**: 
  - Iterations: Controls growth complexity (1-7)
  - Shows current string length
- **Turtle Parameters**:
  - Branching Angle (5°-90°)
  - Step Length (0.1-3.0)
  - Step Width (0.01-0.5)
  - Length/Width Scale (0.5-1.0)
- **Tropism**: Gravitational bending effect (X, Y, Z components)
- **Rendering**: Toggle between 2D lines and 3D cylinders
- **Camera**: Control distance, rotation, auto-rotate
- **Custom Rules**: Define your own axioms and production rules

#### Plant Information Panel
- Shows current preset name
- Displays axiom and generation count
- Geometry statistics (lines/cylinders/leaves)
- Plant bounding box size

### Example Workflows

#### 1. Explore Presets
1. Select different presets from the "Plant Type" dropdown
2. Adjust iterations to see growth stages
3. Toggle 3D mode for cylinder rendering

#### 2. Fine-tune a Plant
1. Choose "Fractal Tree" preset
2. Increase iterations to 5
3. Adjust branching angle to 30°
4. Add tropism Y to -0.3 for drooping branches
5. Enable auto-rotate to see all angles

#### 3. Create Custom Plant
1. Go to "Custom Rules" section
2. Set Axiom: `F`
3. Set Symbol: `F`
4. Set Replacement: `F[+F][-F][&F][^F]`
5. Click "Apply Custom Rules"
6. Switch to 3D mode
7. Adjust iterations and angle as desired

## L-System Symbols

| Symbol | Action | Description |
|--------|--------|-------------|
| `F`, `G` | Move forward & draw | Creates a line/cylinder segment |
| `f` | Move forward (no draw) | Moves without rendering |
| `+` | Turn left | Rotate by angle |
| `-` | Turn right | Rotate by -angle |
| `&` | Pitch down | Rotate around left vector |
| `^` | Pitch up | Rotate around left vector (opposite) |
| `\` | Roll left | Rotate around heading vector |
| `/` | Roll right | Rotate around heading vector (opposite) |
| `\|` | Turn around | Rotate 180° |
| `[` | Push state | Save current position/orientation |
| `]` | Pop state | Restore saved position/orientation |
| `L` | Draw leaf | Creates a leaf at current position |
| `!` | Decrease width | Scale width down |
| `'` | Increase width | Scale width up |

## Project Structure
```
graphics-final/
├── Makefile                 # Build configuration
├── setup.sh                 # Dependency setup script
├── README.md               # This file
├── include/                # Header files
│   ├── LSystem.h          # L-system engine
│   ├── Turtle.h           # Turtle graphics interpreter
│   └── Renderer.h         # OpenGL renderer
├── src/                    # Implementation files
│   ├── main.cpp           # Application entry point
│   ├── LSystem.cpp        # L-system implementation
│   ├── Turtle.cpp         # Turtle interpretation
│   └── Renderer.cpp       # Rendering implementation
├── external/               # Third-party libraries
│   └── imgui/             # ImGui (auto-downloaded)
└── build/                  # Compiled object files
```

## Technical Details

### L-System Implementation
- **Grammar Engine**: Supports axiom, production rules, iterations
- **Rule Types**: Deterministic and stochastic (probability-based)
- **String Generation**: Iterative rule application with efficient string building

### Turtle Graphics
- **2D Mode**: Line segments with color and width
- **3D Mode**: Cylindrical segments with Phong shading
- **State Stack**: Push/pop mechanism for branching
- **Tropism**: Realistic gravitational bending using torque vectors
- **Coordinate System**: Right-handed 3D space with configurable orientations

### Rendering
- **OpenGL 2.1**: Legacy pipeline for compatibility
- **Lighting**: Single directional light with ambient, diffuse, specular components
- **Materials**: Different properties for stems (brown) and leaves (green)
- **Camera**: Spherical coordinate system for intuitive orbital control
- **Anti-aliasing**: 4x MSAA for smooth edges

## Performance Notes
- String length grows exponentially with iterations
- Recommended max iterations: 6 for simple rules, 4-5 for complex rules
- High iteration counts may cause slowdown (warning shown in UI)
- 3D mode is more performance-intensive than 2D mode

## Troubleshooting

### Build Errors
**Problem**: `glfw not found`  
**Solution**: Run `brew install glfw`

**Problem**: `glm/glm.hpp not found`  
**Solution**: Run `brew install glm`

**Problem**: ImGui files not found  
**Solution**: Run `./setup.sh` to download ImGui

### Runtime Issues
**Problem**: Plant not appearing  
**Solution**: 
- Increase iterations (try 3-4)
- Reset camera with "Reset Camera" button
- Try a different preset

**Problem**: Application crashes on high iterations  
**Solution**: Reduce iterations to 5 or below

**Problem**: Slow performance  
**Solution**: 
- Reduce iterations
- Use simpler presets
- Disable auto-regenerate
- Use 2D mode instead of 3D

## Future Enhancements (Remaining Milestones)
- **Week 4**: Advanced rendering with texture mapping and better shading
- **Week 5-6**: Performance optimization, final polish, documentation

## References
- Prusinkiewicz, P. & Lindenmayer, A. (1990): *The Algorithmic Beauty of Plants*
- ImGui: https://github.com/ocornut/imgui
- GLFW: https://www.glfw.org
- GLM: https://glm.g-truc.net

## License
Educational project for Computer Graphics course.
