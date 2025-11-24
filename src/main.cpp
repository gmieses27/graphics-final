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
            glm::vec3 rootTarget(center.x, minBounds.y, center.z);
            float horizontalSpan = std::max(size.x, size.z);
            float dominantSpan = std::max(horizontalSpan, size.y);
            
            // Set camera to look at the root while keeping the entire plant in view
            renderer.cameraTarget_ = rootTarget;
            renderer.cameraDistance = std::max(dominantSpan * 1.4f, 6.0f);
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
        
        // Preset selection
        if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::BeginCombo("Plant Type", presets[currentPreset].c_str())) {
                for (int i = 0; i < presets.size(); i++) {
                    bool is_selected = (currentPreset == i);
                    if (ImGui::Selectable(presets[i].c_str(), is_selected)) {
                        currentPreset = i;
                        lsystem.loadPreset(presets[i]);
                        needsRegenerate = true;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
        
        // L-system parameters
        if (ImGui::CollapsingHeader("L-System Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::SliderInt("Iterations", &iterations, 1, 7)) {
                if (autoRegenerate) needsRegenerate = true;
            }
            
            ImGui::Text("Current String Length: %zu", lsystem.getCurrentString().length());
            if (lsystem.getCurrentString().length() > 10000) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Warning: High complexity!");
            }
        }
        
        // Turtle parameters
        if (ImGui::CollapsingHeader("Turtle Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::SliderFloat("Branching Angle", &angle, 5.0f, 90.0f, "%.1f°")) {
                if (autoRegenerate) needsRegenerate = true;
            }
            
            if (ImGui::SliderFloat("Step Length", &stepLength, 0.1f, 3.0f)) {
                if (autoRegenerate) needsRegenerate = true;
            }
            
            if (ImGui::SliderFloat("Step Width", &stepWidth, 0.01f, 0.5f)) {
                if (autoRegenerate) needsRegenerate = true;
            }
            
            if (ImGui::SliderFloat("Length Scale", &lengthScale, 0.5f, 1.0f)) {
                if (autoRegenerate) needsRegenerate = true;
            }
            
            if (ImGui::SliderFloat("Width Scale", &widthScale, 0.5f, 1.0f)) {
                if (autoRegenerate) needsRegenerate = true;
            }
        }
        
        // Tropism (gravity effect)
        if (ImGui::CollapsingHeader("Tropism (Gravity)", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::SliderFloat("Tropism X", &tropism.x, -1.0f, 1.0f)) {
                if (autoRegenerate) needsRegenerate = true;
            }
            if (ImGui::SliderFloat("Tropism Y", &tropism.y, -1.0f, 1.0f)) {
                if (autoRegenerate) needsRegenerate = true;
            }
            if (ImGui::SliderFloat("Tropism Z", &tropism.z, -1.0f, 1.0f)) {
                if (autoRegenerate) needsRegenerate = true;
            }
        }
        
        // Rendering mode
        if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("3D Mode", &mode3D)) {
                needsRegenerate = true;
            }
            ImGui::SameLine();
            ImGui::Text("(2D uses lines, 3D uses cylinders)");
            
            ImGui::Checkbox("Auto-regenerate", &autoRegenerate);
        }
        
        // Camera controls
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Orbital Controls:");
            ImGui::SliderFloat("Distance", &renderer.cameraDistance, 1.0f, 50.0f);
            ImGui::SliderFloat("Rotation X", &renderer.cameraRotationX, -89.0f, 89.0f);
            ImGui::SliderFloat("Rotation Y", &renderer.cameraRotationY, -180.0f, 180.0f);
            ImGui::Checkbox("Auto-rotate", &renderer.autoRotate);
            
            ImGui::Separator();
            ImGui::Text("Camera Target Position:");
            ImGui::SliderFloat("Target X", &renderer.cameraTarget_.x, -20.0f, 20.0f);
            ImGui::SliderFloat("Target Y", &renderer.cameraTarget_.y, -20.0f, 20.0f);
            ImGui::SliderFloat("Target Z", &renderer.cameraTarget_.z, -20.0f, 20.0f);
            
            if (ImGui::Button("Reset Camera")) {
                renderer.resetCamera();
                needsRegenerate = true;
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Center on Plant Root")) {
                glm::vec3 minBounds = turtle.getMinBounds();
                glm::vec3 maxBounds = turtle.getMaxBounds();
                glm::vec3 center = (minBounds + maxBounds) * 0.5f;
                glm::vec3 size = maxBounds - minBounds;
                float horizontalSpan = std::max(size.x, size.z);
                float dominantSpan = std::max(horizontalSpan, size.y);
                renderer.cameraTarget_ = glm::vec3(center.x, minBounds.y, center.z);
                renderer.cameraDistance = std::max(dominantSpan * 2.2f, 8.0f);
            }
        }
        
        // Custom rules
        if (ImGui::CollapsingHeader("Custom Rules")) {
            ImGui::InputText("Axiom", axiomBuffer, 256);
            
            ImGui::InputText("Symbol", ruleSymbol, 2);
            ImGui::InputText("Replacement", ruleReplacement, 512);
            
            if (ImGui::Button("Apply Custom Rules")) {
                lsystem.setAxiom(axiomBuffer);
                lsystem.clearRules();
                if (strlen(ruleSymbol) > 0 && strlen(ruleReplacement) > 0) {
                    lsystem.addRule(ruleSymbol[0], ruleReplacement);
                }
                needsRegenerate = true;
            }
            
            ImGui::TextWrapped("Symbols: F/G=forward, +/- =turn, &/^=pitch, \\/=roll, [/]=push/pop, L=leaf");
        }
        
        // Action buttons
        ImGui::Separator();
        if (!autoRegenerate && ImGui::Button("Regenerate Plant")) {
            needsRegenerate = true;
        }
        
        if (ImGui::Button("Reset to Default")) {
            iterations = 3;
            angle = 25.0f;
            stepLength = 1.0f;
            stepWidth = 0.1f;
            lengthScale = 0.9f;
            widthScale = 0.7f;
            tropism = glm::vec3(0.0f, -0.1f, 0.0f);
            mode3D = false;
            currentPreset = 0;
            lsystem.loadPreset(presets[currentPreset]);
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
