#include "Renderer.h"
#include <OpenGL/gl.h>
#include <cmath>
#include <iostream>

static Renderer* g_renderer = nullptr;

Renderer::Renderer() 
        : window_(nullptr), width_(1280), height_(720),
            cameraPos_(0.0f, 0.0f, 6.0f), cameraTarget_(0.0f, 0.0f, 0.0f),
      cameraUp_(0.0f, 1.0f, 0.0f), lastMouseX_(0.0), lastMouseY_(0.0),
            mousePressed_(false), cameraDistance(6.0f), 
      cameraRotationX(20.0f), cameraRotationY(45.0f), autoRotate(false) {
    g_renderer = this;
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(int width, int height, const char* title) {
    width_ = width;
    height_ = height;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    
    window_ = glfwCreateWindow(width_, height_, title, nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enable vsync
    
    // Set callbacks
    glfwSetMouseButtonCallback(window_, mouseButtonCallback);
    glfwSetCursorPosCallback(window_, cursorPosCallback);
    glfwSetScrollCallback(window_, scrollCallback);
    
    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void Renderer::shutdown() {
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void Renderer::setCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    cameraPos_ = position;
    cameraTarget_ = target;
    cameraUp_ = up;
}

void Renderer::updateCamera(float deltaTime) {
    if (autoRotate) {
        cameraRotationY += deltaTime * 20.0f; // 20 degrees per second
    }
    
    // Calculate camera position based on spherical coordinates
    float radX = glm::radians(cameraRotationX);
    float radY = glm::radians(cameraRotationY);
    
    cameraPos_.x = cameraTarget_.x + cameraDistance * sin(radY) * cos(radX);
    cameraPos_.y = cameraTarget_.y + cameraDistance * sin(radX);
    cameraPos_.z = cameraTarget_.z + cameraDistance * cos(radY) * cos(radX);
}

void Renderer::resetCamera() {
    cameraDistance = 6.0f;
    cameraRotationX = 20.0f;
    cameraRotationY = 45.0f;
    cameraTarget_ = glm::vec3(0.0f, 0.0f, 0.0f);
    autoRotate = false;
}

void Renderer::beginFrame() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // CUSTOMIZATION: Change the 400 value to adjust the UI panel width
    // Set viewport to right side only (leave 400px for UI on left)
    int uiPanelWidth = 400;  // <- CHANGE THIS VALUE to adjust split position
    int viewportWidth = width_ - uiPanelWidth;
    int viewportHeight = height_;
    glViewport(uiPanelWidth, 0, viewportWidth, viewportHeight);
    
    setupProjection();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set up camera
    glm::vec3 pos = cameraPos_;
    glm::vec3 target = cameraTarget_;
    glm::vec3 up = cameraUp_;
    
    // Simple lookAt implementation
    glm::vec3 f = glm::normalize(target - pos);
    glm::vec3 s = glm::normalize(glm::cross(f, up));
    glm::vec3 u = glm::cross(s, f);
    
    float m[16] = {
        s.x, u.x, -f.x, 0,
        s.y, u.y, -f.y, 0,
        s.z, u.z, -f.z, 0,
        -glm::dot(s, pos), -glm::dot(u, pos), glm::dot(f, pos), 1
    };
    glMultMatrixf(m);
    
    setupLighting();
}

void Renderer::endFrame() {
    glfwSwapBuffers(window_);
    glfwPollEvents();
}

void Renderer::render(const Turtle& turtle) {
    if (!turtle.is3DMode()) {
        renderLines(turtle.getLines());
    } else {
        renderCylinders(turtle.getCylinders());
        renderLeaves(turtle.getLeaves());
    }
}

void Renderer::renderLines(const std::vector<LineSegment>& lines) {
    glDisable(GL_LIGHTING);
    
    for (const auto& line : lines) {
        glLineWidth(line.width * 2.0f);
        glBegin(GL_LINES);
        glColor3f(line.color.r, line.color.g, line.color.b);
        glVertex3f(line.start.x, line.start.y, line.start.z);
        glVertex3f(line.end.x, line.end.y, line.end.z);
        glEnd();
    }
}

void Renderer::renderCylinders(const std::vector<Cylinder>& cylinders) {
    glEnable(GL_LIGHTING);
    
    for (const auto& cyl : cylinders) {
        drawCylinder(cyl.start, cyl.end, cyl.radius, cyl.color);
    }
}

void Renderer::renderLeaves(const std::vector<Leaf>& leaves) {
    glEnable(GL_LIGHTING);
    
    for (const auto& leaf : leaves) {
        drawLeaf(leaf.position, leaf.normal, leaf.size, leaf.color);
    }
}

void Renderer::drawCylinder(const glm::vec3& start, const glm::vec3& end, float radius, const glm::vec3& color) {
    glm::vec3 direction = end - start;
    float height = glm::length(direction);
    
    if (height < 0.001f) return;
    
    direction = glm::normalize(direction);
    
    glPushMatrix();
    glTranslatef(start.x, start.y, start.z);
    
    // Align cylinder with direction
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    if (fabs(glm::dot(direction, up)) < 0.999f) {
        glm::vec3 axis = glm::cross(up, direction);
        float angle = acos(glm::dot(up, direction));
        glRotatef(glm::degrees(angle), axis.x, axis.y, axis.z);
    } else if (direction.y < 0) {
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    }
    
    // Set material color
    float mat_ambient[] = {color.r * 0.3f, color.g * 0.3f, color.b * 0.3f, 1.0f};
    float mat_diffuse[] = {color.r, color.g, color.b, 1.0f};
    float mat_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);
    
    // Draw cylinder using triangle strips
    int segments = 8;
    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / segments * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / segments * 2.0f * M_PI;
        
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);
        
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(cos(angle1), 0.0f, sin(angle1));
        glVertex3f(x1, 0.0f, z1);
        glVertex3f(x1, height, z1);
        glNormal3f(cos(angle2), 0.0f, sin(angle2));
        glVertex3f(x2, 0.0f, z2);
        glVertex3f(x2, height, z2);
        glEnd();
    }
    
    glPopMatrix();
}

void Renderer::drawLeaf(const glm::vec3& position, const glm::vec3& normal, float size, const glm::vec3& color) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    // Set material color for leaves
    float mat_ambient[] = {color.r * 0.3f, color.g * 0.3f, color.b * 0.3f, 1.0f};
    float mat_diffuse[] = {color.r, color.g, color.b, 1.0f};
    float mat_specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);
    
    // Draw simple quad for leaf
    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x, normal.y, normal.z);
    glVertex3f(-size, 0.0f, 0.0f);
    glVertex3f(size, 0.0f, 0.0f);
    glVertex3f(0.0f, size * 1.5f, 0.0f);
    glEnd();
    
    glPopMatrix();
}

void Renderer::setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Key light (main directional light)
    float light_pos[] = {2.0f, 5.0f, 3.0f, 0.0f}; // Directional
    float light_ambient[] = {0.3f, 0.3f, 0.35f, 1.0f};
    float light_diffuse[] = {0.8f, 0.8f, 0.7f, 1.0f};
    float light_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void Renderer::setupProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // CUSTOMIZATION: Must match the uiPanelWidth in beginFrame()
    // Calculate aspect ratio based on the actual viewport size (right side only)
    int uiPanelWidth = 400;  // <- CHANGE THIS to match beginFrame() value
    int viewportWidth = width_ - uiPanelWidth;
    int viewportHeight = height_;
    float aspect = (float)viewportWidth / (float)viewportHeight;
    float fov = 45.0f;
    float near = 0.1f;
    float far = 100.0f;
    
    float top = near * tan(glm::radians(fov) * 0.5f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;
    
    glFrustum(left, right, bottom, top, near, far);
}

bool Renderer::shouldClose() const {
    return glfwWindowShouldClose(window_);
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (g_renderer && button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            // CUSTOMIZATION: Must match uiPanelWidth in beginFrame() and main.cpp
            int uiPanelWidth = 400;  // <- CHANGE THIS to match other values
            // Only allow interaction in the right panel (x >= uiPanelWidth)
            if (xpos >= uiPanelWidth) {
                g_renderer->mousePressed_ = true;
                g_renderer->lastMouseX_ = xpos;
                g_renderer->lastMouseY_ = ypos;
            }
        } else if (action == GLFW_RELEASE) {
            g_renderer->mousePressed_ = false;
        }
    }
}

void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // CUSTOMIZATION: Must match uiPanelWidth in beginFrame() and main.cpp
    int uiPanelWidth = 400;  // <- CHANGE THIS to match other values
    // Only allow interaction in the right panel (x >= uiPanelWidth)
    if (g_renderer && g_renderer->mousePressed_ && xpos >= uiPanelWidth) {
        double dx = xpos - g_renderer->lastMouseX_;
        double dy = ypos - g_renderer->lastMouseY_;
        
        g_renderer->cameraRotationY += dx * 0.5f;
        g_renderer->cameraRotationX += dy * 0.5f;
        
        // Clamp vertical rotation
        if (g_renderer->cameraRotationX > 89.0f) g_renderer->cameraRotationX = 89.0f;
        if (g_renderer->cameraRotationX < -89.0f) g_renderer->cameraRotationX = -89.0f;
        
        g_renderer->lastMouseX_ = xpos;
        g_renderer->lastMouseY_ = ypos;
    }
}

void Renderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (g_renderer) {
        g_renderer->cameraDistance -= yoffset * 0.5f;
        if (g_renderer->cameraDistance < 1.0f) g_renderer->cameraDistance = 1.0f;
        if (g_renderer->cameraDistance > 50.0f) g_renderer->cameraDistance = 50.0f;
    }
}
