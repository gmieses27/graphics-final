# Quick Start Guide - Procedural Plant Modeling System

## Installation & Setup (5 minutes)

1. **Navigate to project directory**
   ```bash
   cd graphics-final
   ```

2. **Run setup script**
   ```bash
   chmod +x setup.sh
   ./setup.sh
   ```
   This will:
   - Download ImGui library
   - Check for GLFW and GLM
   - Install missing dependencies

3. **Build the project**
   ```bash
   make
   ```

4. **Run the application**
   ```bash
   ./plant_modeler
   ```
   Or use:
   ```bash
   make run
   ```

## First-Time Usage

When the application opens, you'll see:
- **Main Window**: 3D view of the plant
- **Plant Control Panel**: Left side panel with all controls
- **Plant Information Panel**: Shows current plant statistics

## Try These Examples

### Example 1: Simple Fractal Tree
1. Select "Fractal Tree" from Plant Type dropdown
2. Set Iterations to 4
3. Set Branching Angle to 25°
4. Toggle "3D Mode" ON
5. Click "Auto-rotate" to see all angles

### Example 2: Fern
1. Select "Fern" preset
2. Set Iterations to 5
3. Set Branching Angle to 22°
4. Set Tropism Y to -0.2 (drooping effect)
5. Keep in 2D mode for classic look

### Example 3: Bushy Plant
1. Select "Bush" preset
2. Set Iterations to 4
3. Toggle 3D Mode ON
4. Adjust Step Width to 0.15
5. Set Branching Angle to 35°

### Example 4: Create Your Own
1. Go to "Custom Rules" section
2. Axiom: `F`
3. Symbol: `F`
4. Replacement: `F[+F][&F][-F][^F]`
5. Click "Apply Custom Rules"
6. Set Iterations to 4
7. Enable 3D Mode
8. Experiment with angles!

## Controls Reference

### Mouse Controls
- **Left Click + Drag**: Rotate camera
- **Scroll Wheel**: Zoom in/out

### Key Parameters
- **Iterations**: How many times rules are applied (1-7)
  - Higher = more complex plant
  - Warning: Exponential growth!
  
- **Branching Angle**: Angle between branches (5°-90°)
  - Lower = narrow, upright trees
  - Higher = wide, spreading bushes

- **Step Length**: Length of each segment (0.1-3.0)
  - Affects overall plant size

- **Tropism**: Gravity effect (-1.0 to 1.0 each axis)
  - Y negative = drooping/weeping
  - X/Z = sideways bending

## Troubleshooting

**Problem**: Plant too small/large
- Adjust Step Length parameter
- Use "Reset Camera" button

**Problem**: Application won't build
- Run `./setup.sh` again
- Check that GLFW and GLM are installed: `brew list glfw glm`

**Problem**: Performance slow
- Lower iterations (try 3-4)
- Use simpler presets
- Disable "Auto-regenerate"

**Problem**: Can't see plant
- Click "Reset Camera"
- Increase iterations
- Try different preset

## Tips for Best Results

1. **Start simple**: Begin with 3 iterations and increase gradually
2. **Use Auto-regenerate**: See changes in real-time
3. **Experiment with angles**: Small changes make big differences
4. **Try both modes**: 2D for speed, 3D for beauty
5. **Tropism is subtle**: Start with small values like -0.1

## Understanding L-System Symbols

- `F` = Draw forward (creates branch)
- `[` = Start new branch
- `]` = End branch, return to saved position
- `+` = Turn left
- `-` = Turn right
- `&` = Pitch down
- `^` = Pitch up
- `/` = Roll right
- `\` = Roll left
- `L` = Draw leaf

## Saving Your Work

Currently, you can:
- Take screenshots (macOS: Cmd+Shift+4)
- Note down parameter values for later
- Write down custom rules

Future versions will add save/load functionality.

## Performance Guidelines

| Iterations | String Length | Performance |
|-----------|---------------|-------------|
| 1-3       | < 100         | Instant     |
| 4-5       | 100-1000      | Fast        |
| 6         | 1000-10000    | Moderate    |
| 7+        | 10000+        | Slow        |

## Getting Help

- Check BUILD_INSTRUCTIONS.md for detailed documentation
- Review the L-System symbol table in README
- Try the preset plants to understand the system
- Experiment with one parameter at a time

Enjoy creating beautiful procedural plants! 🌱🌿🌳
