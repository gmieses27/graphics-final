#ifndef TURTLE_H
#define TURTLE_H

#include <vector>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// Structure to represent turtle state
struct TurtleState {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 left;
    float length;
    float width;
    glm::vec3 color;
    
    TurtleState() : position(0.0f), direction(0.0f, 1.0f, 0.0f), 
                    up(0.0f, 0.0f, 1.0f), left(1.0f, 0.0f, 0.0f),
                    length(1.0f), width(0.1f), color(0.4f, 0.8f, 0.3f) {}
};

// Line segment for 2D/3D rendering
struct LineSegment {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 color;
    float width;
};

// Cylinder for 3D rendering
struct Cylinder {
    glm::vec3 start;
    glm::vec3 end;
    float radius;
    glm::vec3 color;
};

// Leaf for 3D rendering
struct Leaf {
    glm::vec3 position;
    glm::vec3 normal;
    float size;
    glm::vec3 color;
};

// Turtle graphics interpreter
class Turtle {
public:
    Turtle();
    ~Turtle();
    
    // Interpretation parameters
    void setAngle(float angle) { angle_ = angle; }
    void setStepLength(float length) { stepLength_ = length; }
    void setStepWidth(float width) { stepWidth_ = width; }
    void setLengthScale(float scale) { lengthScale_ = scale; }
    void setWidthScale(float scale) { widthScale_ = scale; }
    void setTropism(const glm::vec3& tropism) { tropism_ = tropism; }
    void set3DMode(bool mode) { mode3D_ = mode; }
    
    float getAngle() const { return angle_; }
    float getStepLength() const { return stepLength_; }
    float getStepWidth() const { return stepWidth_; }
    bool is3DMode() const { return mode3D_; }
    
    // Interpret L-system string
    void interpret(const std::string& lsystemString);
    void reset();
    
    // Get geometry
    const std::vector<LineSegment>& getLines() const { return lines_; }
    const std::vector<Cylinder>& getCylinders() const { return cylinders_; }
    const std::vector<Leaf>& getLeaves() const { return leaves_; }
    
    // Get bounding information
    glm::vec3 getMinBounds() const { return minBounds_; }
    glm::vec3 getMaxBounds() const { return maxBounds_; }
    glm::vec3 getCenter() const { return (minBounds_ + maxBounds_) * 0.5f; }
    glm::vec3 getRootPosition() const { return lowestPoint_; }
    
private:
    // Turtle state
    TurtleState state_;
    std::stack<TurtleState> stateStack_;
    
    // Parameters
    float angle_;           // Branching angle in degrees
    float stepLength_;      // Base step length
    float stepWidth_;       // Base step width
    float lengthScale_;     // Length reduction per level
    float widthScale_;      // Width reduction per level
    glm::vec3 tropism_;     // Gravitational tropism vector
    bool mode3D_;           // 2D or 3D mode
    
    // Generated geometry
    std::vector<LineSegment> lines_;
    std::vector<Cylinder> cylinders_;
    std::vector<Leaf> leaves_;
    
    // Bounds
    glm::vec3 minBounds_;
    glm::vec3 maxBounds_;
    glm::vec3 lowestPoint_;
    float lowestY_;
    
    // Turtle commands
    void moveForward();
    void turnLeft();
    void turnRight();
    void pitchUp();
    void pitchDown();
    void rollLeft();
    void rollRight();
    void turnAround();
    void pushState();
    void popState();
    void drawLeaf();
    void scaleLength(float factor);
    void scaleWidth(float factor);
    
    // Helper methods
    void updateBounds(const glm::vec3& point);
    void applyTropism();
    glm::mat3 getRotationMatrix(const glm::vec3& axis, float angleDeg);
};

#endif // TURTLE_H
