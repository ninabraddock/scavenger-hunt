#include "circle.h"
#include "rect.h"

Circle::Circle(Shader & shader, vec2 pos, vec2 size, struct color color)
    : Shape(shader, pos, size, color) {
    initVectors();
    initVAO();
    initVBO();
}

Circle::Circle(Shader &shader, vec2 pos, vec2 size, struct color color, float radius)
    : Shape(shader, pos, size, color), radius(radius) {
    initVectors();
    initVAO();
    initVBO();
}

Circle::~Circle() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Circle::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2); // +2 for center and last vertex
    glBindVertexArray(0);
}

void Circle::initVectors() {
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        vertices.push_back(radius * cosf(theta)); // x = r*cos(theta)
        vertices.push_back(radius * sinf(theta)); // y = r*sin(theta)
    }
}

float Circle::getLeft() const   { return pos.x - radius; }
float Circle::getRight() const  { return pos.x + radius; }
float Circle::getTop() const    { return pos.y + radius; }
float Circle::getBottom() const { return pos.y - radius; }

// static function makes this easier to read
bool Circle::isOverlapping(const Circle &c1, const Circle &c2) {
    // EASY STUDENT TODO: Implement this function
    // Calculate Euclidean distance between the two circles
    // Distance formula: sqrt((x2 - x1)^2 + (y2 - y1)^2)
    float distance = fabs(powf(c2.pos.x - c1.pos.x, 2) + powf(c2.pos.y - c1.pos.y, 2));
    return distance < powf(c1.radius + c2.radius, 2);
}

bool Circle::isOverlapping(const Shape& other) const {
    // If the other shape is a circle, use the static function
    if (const auto* c = dynamic_cast<const Circle*>(&other)) {
        return isOverlapping(*this, *c);
    }
    // Otherwise, it is a rect
    if (const auto* r = dynamic_cast<const Rect*>(&other)) {
//        // HARD STUDENT TODO (maybe don't ask them to do this)
//        // Calculate the closest point to the circle within the rectangle
//        // by clamping the circle's center to the rectangle
//        float closestX = glm::clamp(pos.x, r->getLeft(), r->getRight());
//        float closestY = glm::clamp(pos.y, r->getBottom(), r->getTop());
//
//        // Calculate the distance between the circle's center and this closest point
//        float distanceX = pos.x - closestX;
//        float distanceY = pos.y - closestY;
//
//        // If the distance is less than the circle's radius, an intersection occurs
//        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
//        return distanceSquared < (radius * radius);
    }
    return false;
}