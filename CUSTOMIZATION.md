# Customization Guide

This guide explains how to customize the split-screen layout of the Plant Modeler application.

## Changing the UI Panel Width

The application uses a split-screen layout:
- **Left side**: Interactive UI panel (default: 400px wide)
- **Right side**: 3D plant rendering viewport (fills remaining space)

To change the width of the UI panel, you need to update **THREE** files with synchronized values:

### Files to Modify

#### 1. `src/Renderer.cpp` (3 locations)

**Location 1 - beginFrame() function (around line 100):**
```cpp
void Renderer::beginFrame() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // CUSTOMIZATION: Change this value to adjust UI panel width
    int uiPanelWidth = 400;  // <- CHANGE THIS VALUE (e.g., 300, 500, etc.)
```

**Location 2 - setupProjection() function (around line 115):**
```cpp
void Renderer::setupProjection() {
    // CUSTOMIZATION: Must match uiPanelWidth in beginFrame() and main.cpp
    int uiPanelWidth = 400;  // <- CHANGE THIS to match Location 1
```

**Location 3 - mouseButtonCallback() function (around line 298):**
```cpp
void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (g_renderer && button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            // CUSTOMIZATION: Must match uiPanelWidth in beginFrame() and main.cpp
            int uiPanelWidth = 400;  // <- CHANGE THIS to match Location 1
```

**Location 4 - cursorPosCallback() function (around line 315):**
```cpp
void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // CUSTOMIZATION: Must match uiPanelWidth in beginFrame() and main.cpp
    int uiPanelWidth = 400;  // <- CHANGE THIS to match Location 1
```

#### 2. `src/main.cpp` (2 locations)

**Location 1 - Control panel (around line 105):**
```cpp
        // CUSTOMIZATION: Change these values to adjust panel size/position
        float uiPanelWidth = 400.0f;   // <- CHANGE THIS to match Renderer.cpp
        float uiPanelHeight = 580.0f;  // Height of the control panel
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(uiPanelWidth, uiPanelHeight));
```

**Location 2 - Info panel (around line 245):**
```cpp
        // CUSTOMIZATION: Change these values to adjust panel size/position
        float infoPanelY = 580.0f;      // <- Y position (should match uiPanelHeight above)
        float infoPanelHeight = 140.0f; // <- Height of info panel
        
        ImGui::SetNextWindowPos(ImVec2(0, infoPanelY));
        ImGui::SetNextWindowSize(ImVec2(uiPanelWidth, infoPanelHeight));
```

### Example: Making the Panel 300px Wide

1. In `src/Renderer.cpp`, change all 4 instances:
   ```cpp
   int uiPanelWidth = 300;  // Changed from 400
   ```

2. In `src/main.cpp`, change both instances:
   ```cpp
   float uiPanelWidth = 300.0f;  // Changed from 400.0f
   ```

3. Rebuild the project:
   ```bash
   make clean
   make
   ```

4. Run the updated application:
   ```bash
   ./plant_modeler
   ```

### Example: Making the Panel 600px Wide

1. In `src/Renderer.cpp`, change all 4 instances:
   ```cpp
   int uiPanelWidth = 600;  // Changed from 400
   ```

2. In `src/main.cpp`, change both instances:
   ```cpp
   float uiPanelWidth = 600.0f;  // Changed from 400.0f
   ```

3. Rebuild and run as shown above.

## Adjusting Panel Heights

You can also adjust the vertical layout of the UI panels:

### Control Panel Height
In `src/main.cpp` (around line 106):
```cpp
float uiPanelHeight = 580.0f;  // <- Adjust this value
```

### Info Panel Position and Height
In `src/main.cpp` (around line 246-247):
```cpp
float infoPanelY = 580.0f;      // <- Should match control panel height
float infoPanelHeight = 140.0f; // <- Adjust info panel height
```

**Note:** The total height should not exceed your window height (720px by default).

## Why Synchronize These Values?

The UI panel width appears in multiple places because:

1. **Renderer.cpp::beginFrame()** - Sets the OpenGL viewport to render only on the right side
2. **Renderer.cpp::setupProjection()** - Calculates the correct aspect ratio for the viewport
3. **Renderer.cpp::mouseButtonCallback()** - Restricts mouse clicks to the rendering area
4. **Renderer.cpp::cursorPosCallback()** - Restricts mouse dragging to the rendering area
5. **main.cpp (control panel)** - Sets the width of the ImGui control panel
6. **main.cpp (info panel)** - Sets the width of the ImGui info panel

All values must match to ensure:
- UI panels don't overlap the rendering area
- The 3D viewport has the correct aspect ratio
- Mouse interactions only affect the plant when clicking in the right panel
- The layout looks clean and professional

## Troubleshooting

**Problem:** Plant appears stretched or squished after changing panel width
- **Solution:** Make sure you updated the value in `setupProjection()` as well

**Problem:** Can rotate plant by clicking on the UI panel
- **Solution:** Verify you updated both mouse callback functions

**Problem:** UI panels overlap the rendering area
- **Solution:** Ensure all values in `main.cpp` match those in `Renderer.cpp`

**Problem:** Changes don't take effect
- **Solution:** Run `make clean && make` to rebuild everything from scratch
