#ifndef FINAL_PROJECT_NBRADDOC_RROSE1_CIRCLE_H
#define FINAL_PROJECT_NBRADDOC_RROSE1_CIRCLE_H

#include "shape.h"
#include "../framework/shader.h"
using std::vector, glm::vec2, glm::vec3, glm::normalize, glm::dot;


class Circle : public Shape {
private:

/// @brief Number of x,y points to draw the circle
    const static int segments = 100;
    /// @brief Radius of the circle (half of screen width
    float radius = 0.5f;

public:
    /// @brief Construct a new Circle object
    /// @details This constructor will call the InitRenderData function.
    /// @param shader The shader to use
    /// @param pos The position of the circle
    /// @param size The size of the circle
    /// @param color The color of the circle
    Circle(Shader &shader, vec2 pos, vec2 size, struct color color);

    Circle(Shader &shader, vec2 pos, vec2 size, struct color color, float radius);

    /// @brief Destroy the Circle object
    /// @details destroys the VAO and VBO associated with the circle
    ~Circle();

    /// @brief Draws the circle
    void draw() const override;

    /// @brief Computes the border of the circle, and stores the vertices in the circleVertices array.
    void initVectors();

    // --------------------------------------------------------
    // Overloaded functions
    // --------------------------------------------------------
    // Position/Movement Functions
    virtual float getLeft() const override;
    virtual float getRight() const override;
    virtual float getTop() const override;
    virtual float getBottom() const override;

    /// @brief Checks if two circles are overlapping
    static bool isOverlapping(const Circle &c1, const Circle &c2);
    bool isOverlapping(const Shape& other) const override;
};


#endif //FINAL_PROJECT_NBRADDOC_RROSE1_CIRCLE_H
