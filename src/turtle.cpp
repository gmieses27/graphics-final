#include "Turtle.h"
#include <cmath>
#include <glm/gtc/constants.hpp>

Turtle::Turtle() 
    : angle_(25.0f), stepLength_(1.0f), stepWidth_(0.1f),
      lengthScale_(0.9f), widthScale_(0.7f), tropism_(0.0f, -0.1f, 0.0f),
      mode3D_(false), minBounds_(FLT_MAX), maxBounds_(-FLT_MAX) {
    reset();
}

Turtle::~Turtle() {}

void Turtle::reset() {
    state_ = TurtleState();
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    lines_.clear();
    cylinders_.clear();
    leaves_.clear();
    minBounds_ = glm::vec3(FLT_MAX);
    maxBounds_ = glm::vec3(-FLT_MAX);
}

void Turtle::interpret(const std::string& lsystemString) {
    reset();
    
    for (char symbol : lsystemString) {
        switch (symbol) {
            case 'F':  // Move forward and draw
            case 'G':  // Move forward and draw (alternative)
                moveForward();
                break;
            case 'f':  // Move forward without drawing
                state_.position += state_.direction * stepLength_;
                updateBounds(state_.position);
                break;
            case '+':  // Turn left
                turnLeft();
                break;
            case '-':  // Turn right
                turnRight();
                break;
            case '&':  // Pitch down
                pitchDown();
                break;
            case '^':  // Pitch up
                pitchUp();
                break;
            case '\\': // Roll left
                rollLeft();
                break;
            case '/':  // Roll right
                rollRight();
                break;
            case '|':  // Turn around
                turnAround();
                break;
            case '[':  // Push state
                pushState();
                break;
            case ']':  // Pop state
                popState();
                break;
            case 'L':  // Draw leaf
                drawLeaf();
                break;
            case '!':  // Decrease width
                scaleWidth(widthScale_);
                break;
            case '\'': // Increase width
                scaleWidth(1.0f / widthScale_);
                break;
            case 'A':  // Apex (just a symbol, no action)
            case 'X':  // Variable (no action)
            case 'Y':  // Variable (no action)
                break;
            default:
                // Ignore unknown symbols
                break;
        }
    }
}

void Turtle::moveForward() {
    glm::vec3 startPos = state_.position;
    
    // Apply tropism (gravitational bending)
    if (glm::length(tropism_) > 0.0001f) {
        applyTropism();
    }
    
    // Move forward
    state_.position += state_.direction * state_.length * stepLength_;
    updateBounds(state_.position);
    
    if (mode3D_) {
        // Create cylinder
        Cylinder cyl;
        cyl.start = startPos;
        cyl.end = state_.position;
        cyl.radius = state_.width * stepWidth_;
        cyl.color = glm::vec3(0.4f, 0.3f, 0.2f); // Brown for stems
        cylinders_.push_back(cyl);
    } else {
        // Create line segment
        LineSegment line;
        line.start = startPos;
        line.end = state_.position;
        line.color = state_.color;
        line.width = state_.width * stepWidth_;
        lines_.push_back(line);
    }
}

void Turtle::turnLeft() {
    if (mode3D_) {
        glm::mat3 rot = getRotationMatrix(state_.up, angle_);
        state_.direction = rot * state_.direction;
        state_.left = rot * state_.left;
    } else {
        float rad = glm::radians(angle_);
        float x = state_.direction.x * cos(rad) - state_.direction.y * sin(rad);
        float y = state_.direction.x * sin(rad) + state_.direction.y * cos(rad);
        state_.direction = glm::vec3(x, y, 0.0f);
    }
}

void Turtle::turnRight() {
    if (mode3D_) {
        glm::mat3 rot = getRotationMatrix(state_.up, -angle_);
        state_.direction = rot * state_.direction;
        state_.left = rot * state_.left;
    } else {
        float rad = glm::radians(-angle_);
        float x = state_.direction.x * cos(rad) - state_.direction.y * sin(rad);
        float y = state_.direction.x * sin(rad) + state_.direction.y * cos(rad);
        state_.direction = glm::vec3(x, y, 0.0f);
    }
}

void Turtle::pitchDown() {
    glm::mat3 rot = getRotationMatrix(state_.left, -angle_);
    state_.direction = rot * state_.direction;
    state_.up = rot * state_.up;
}

void Turtle::pitchUp() {
    glm::mat3 rot = getRotationMatrix(state_.left, angle_);
    state_.direction = rot * state_.direction;
    state_.up = rot * state_.up;
}

void Turtle::rollLeft() {
    glm::mat3 rot = getRotationMatrix(state_.direction, angle_);
    state_.left = rot * state_.left;
    state_.up = rot * state_.up;
}

void Turtle::rollRight() {
    glm::mat3 rot = getRotationMatrix(state_.direction, -angle_);
    state_.left = rot * state_.left;
    state_.up = rot * state_.up;
}

void Turtle::turnAround() {
    if (mode3D_) {
        glm::mat3 rot = getRotationMatrix(state_.up, 180.0f);
        state_.direction = rot * state_.direction;
        state_.left = rot * state_.left;
    } else {
        state_.direction = -state_.direction;
    }
}

void Turtle::pushState() {
    stateStack_.push(state_);
}

void Turtle::popState() {
    if (!stateStack_.empty()) {
        state_ = stateStack_.top();
        stateStack_.pop();
    }
}

void Turtle::drawLeaf() {
    Leaf leaf;
    leaf.position = state_.position;
    leaf.normal = state_.direction;
    leaf.size = state_.width * stepWidth_ * 2.0f;
    leaf.color = glm::vec3(0.2f, 0.8f, 0.3f); // Green
    leaves_.push_back(leaf);
}

void Turtle::scaleLength(float factor) {
    state_.length *= factor;
}

void Turtle::scaleWidth(float factor) {
    state_.width *= factor;
}

void Turtle::updateBounds(const glm::vec3& point) {
    minBounds_ = glm::min(minBounds_, point);
    maxBounds_ = glm::max(maxBounds_, point);
}

void Turtle::applyTropism() {
    // Bend the direction vector slightly toward tropism vector
    glm::vec3 t = tropism_;
    glm::vec3 h = state_.direction;
    
    // Calculate torque vector
    glm::vec3 torque = glm::cross(h, t);
    
    if (glm::length(torque) > 0.0001f) {
        float alpha = 0.3f; // Tropism strength
        glm::vec3 axis = glm::normalize(torque);
        float angle = alpha * glm::length(t);
        
        glm::mat3 rot = getRotationMatrix(axis, glm::degrees(angle));
        state_.direction = glm::normalize(rot * state_.direction);
    }
}

glm::mat3 Turtle::getRotationMatrix(const glm::vec3& axis, float angleDeg) {
    float rad = glm::radians(angleDeg);
    glm::vec3 ax = glm::normalize(axis);
    float c = cos(rad);
    float s = sin(rad);
    float t = 1.0f - c;
    
    glm::mat3 rot;
    rot[0][0] = t * ax.x * ax.x + c;
    rot[0][1] = t * ax.x * ax.y + s * ax.z;
    rot[0][2] = t * ax.x * ax.z - s * ax.y;
    
    rot[1][0] = t * ax.x * ax.y - s * ax.z;
    rot[1][1] = t * ax.y * ax.y + c;
    rot[1][2] = t * ax.y * ax.z + s * ax.x;
    
    rot[2][0] = t * ax.x * ax.z + s * ax.y;
    rot[2][1] = t * ax.y * ax.z - s * ax.x;
    rot[2][2] = t * ax.z * ax.z + c;
    
    return rot;
}
