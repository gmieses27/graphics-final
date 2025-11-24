#include "LSystem.h"
#include "Turtle.h"
#include "Renderer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <cmath>
#include <algorithm>

int main() {
    // Initialize renderer
    Renderer renderer;
    if (!renderer.initialize(1280, 720, "Procedural Plant Modeling - L-Systems")) {
        return -1;
    }
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(renderer.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 120");
    
    // Create L-system and turtle
    LSystem lsystem;
    Turtle turtle;
    
    // UI state
    int iterations = 4;
    float angle = 25.0f;
    float stepLength = 0.5f;
    float stepWidth = 0.05f;
    float lengthScale = 0.9f;
    float widthScale = 0.7f;
    glm::vec3 tropism(0.0f, -0.1f, 0.0f);
    bool mode3D = true;
    bool autoRegenerate = true;
    bool needsRegenerate = true;
    
    // Preset management
    std::vector<std::string> presets = lsystem.getAvailablePresets();
    int currentPreset = 0;
    lsystem.loadPreset(presets[currentPreset]);
    
    // Custom rule editing
    char axiomBuffer[256] = "F";
    char ruleSymbol[2] = "F";
    char ruleReplacement[512] = "F[+F]F[-F]F";
    
    // Camera
    renderer.resetCamera();
    float lastFrameTime = glfwGetTime();
    
    // Main loop
    while (!renderer.shouldClose()) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        
        // Regenerate L-system if needed
        if (needsRegenerate) {
            std::string result = lsystem.generate(iterations);
            
            turtle.setAngle(angle);
            turtle.setStepLength(stepLength);
            turtle.setStepWidth(stepWidth);
            turtle.setLengthScale(lengthScale);
            turtle.setWidthScale(widthScale);
            turtle.setTropism(tropism);
            turtle.set3DMode(mode3D);
            
            turtle.interpret(result);
            
            // Auto-center camera around the plant root (bottom-most point)
            glm::vec3 minBounds = turtle.getMinBounds();
            glm::vec3 maxBounds = turtle.getMaxBounds();
            glm::vec3 center = (minBounds + maxBounds) * 0.5f;
            glm::vec3 size = maxBounds - minBounds;
            glm::vec3 rootPosition = turtle.getRootPosition();
            glm::vec3 rootTarget(rootPosition.x, rootPosition.y, rootPosition.z);
            float horizontalSpan = std::max(size.x, size.z);
            float dominantSpan = std::max(horizontalSpan, size.y);
            
            // Set camera to look at the root while keeping the entire plant in view
            const float targetOffsetX = -8.0f; // CUSTOMIZATION: shift to nudge root horizontally in viewport
            const float targetOffsetY = 0.0f; // CUSTOMIZATION: shift to raise/lower root framing
            renderer.cameraTarget_ = rootTarget + glm::vec3(targetOffsetX, targetOffsetY, 0.0f);
            const float zoomTightness = 1.85f; // CUSTOMIZATION: raise/lower to change auto zoom framing
            const float minZoom = 3.0f;        // CUSTOMIZATION: lowest camera distance allowed on regen
            renderer.cameraDistance = std::max(dominantSpan * zoomTightness, minZoom);
            renderer.cameraRotationX = 25.0f;
            renderer.cameraRotationY = 45.0f;
            
            needsRegenerate = false;
        }
        
        // Update camera
        renderer.updateCamera(deltaTime);
        
        // Render
        renderer.beginFrame();
        renderer.render(turtle);
        
        // ImGui UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Main control panel - fixed to left side
        // CUSTOMIZATION: Change these values to adjust UI panel size/position
        float uiPanelWidth = 400.0f;   // <- CHANGE THIS to adjust panel width (must match Renderer.cpp)
        float uiPanelHeight = 720.0f;  // <- CHANGE THIS to adjust panel height
        
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(uiPanelWidth, uiPanelHeight), ImGuiCond_FirstUseEver);
        ImGui::Begin("Plant Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        ImGui::Text("Procedural Plant Modeling System");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Controls temporarily hidden.");
        ImGui::TextWrapped("Use the buttons below to regenerate or reset the scenario.");
        ImGui::Spacing();
        
        // Action buttons
        ImGui::Separator();
        if (!autoRegenerate && ImGui::Button("Regenerate Plant")) {
            needsRegenerate = true;
        }
        
        if (ImGui::Button("Reset to Default")) {
            iterations = 4;
            angle = 25.0f;
            stepLength = 0.5f;
            stepWidth = 0.05f;
            lengthScale = 0.9f;
            widthScale = 0.7f;
            tropism = glm::vec3(0.0f, -0.1f, 0.0f);
            mode3D = true;
            currentPreset = 0;
            lsystem.loadPreset(presets[currentPreset]);
            autoRegenerate = true;
            renderer.resetCamera();
            needsRegenerate = true;
        }
        
        ImGui::Separator();
        ImGui::Text("Controls:");
        ImGui::BulletText("Left Mouse: Rotate camera");
        ImGui::BulletText("Scroll: Zoom in/out");
        
        ImGui::End();
        
        // Info panel - bottom left corner
        // CUSTOMIZATION: Adjust position/size to fit below main control panel
        float infoPanelY = 580.0f;     // <- CHANGE THIS for vertical position
        float infoPanelHeight = 140.0f; // <- CHANGE THIS for panel height
        
        ImGui::SetNextWindowPos(ImVec2(0, infoPanelY), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(uiPanelWidth, infoPanelHeight), ImGuiCond_FirstUseEver);
        ImGui::Begin("Plant Information", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::Text("Current Preset: %s", presets[currentPreset].c_str());
        ImGui::Text("Axiom: %s", lsystem.getAxiom().c_str());
        ImGui::Text("Generation: %d", iterations);
        ImGui::Text("String Length: %zu", lsystem.getCurrentString().length());
        
        if (mode3D) {
            ImGui::Text("Cylinders: %zu", turtle.getCylinders().size());
            ImGui::Text("Leaves: %zu", turtle.getLeaves().size());
        } else {
            ImGui::Text("Line Segments: %zu", turtle.getLines().size());
        }
        
        glm::vec3 bounds = turtle.getMaxBounds() - turtle.getMinBounds();
        ImGui::Text("Plant Size: %.2f x %.2f x %.2f", bounds.x, bounds.y, bounds.z);
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        renderer.endFrame();
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    renderer.shutdown();
    
    return 0;
}
