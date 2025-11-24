#ifndef RENDERER_H
#define RENDERER_H

#include "Turtle.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height, const char* title);
    void shutdown();
    
    // Camera controls
    void setCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
    void updateCamera(float deltaTime);
    void resetCamera();
    
    // Rendering
    void beginFrame();
    void endFrame();
    void render(const Turtle& turtle);
    
    // Window management
    bool shouldClose() const;
    GLFWwindow* getWindow() const { return window_; }
    void setSceneOffset(const glm::vec3& offset);
    glm::vec3 getSceneOffset() const { return sceneOffset_; }
    
    // Camera parameters
    float cameraDistance;
    float cameraRotationX;
    float cameraRotationY;
    bool autoRotate;
    glm::vec3 cameraTarget_;
    
private:
    GLFWwindow* window_;
    int width_;
    int height_;
    
    glm::vec3 cameraPos_;
    glm::vec3 cameraUp_;
    glm::vec3 sceneOffset_;
    
    // Mouse state
    double lastMouseX_;
    double lastMouseY_;
    bool mousePressed_;
    
    // Rendering methods
    void renderLines(const std::vector<LineSegment>& lines);
    void renderCylinders(const std::vector<Cylinder>& cylinders);
    void renderLeaves(const std::vector<Leaf>& leaves);
    void drawCylinder(const glm::vec3& start, const glm::vec3& end, float radius, const glm::vec3& color);
    void drawLeaf(const glm::vec3& position, const glm::vec3& normal, float size, const glm::vec3& color);
    void setupLighting();
    void setupProjection();
    
    // Input handling
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif // RENDERER_H
