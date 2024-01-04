#ifndef FINAL_PROJECT_NBRADDOC_RROSE1_ENGINE_H
#define FINAL_PROJECT_NBRADDOC_RROSE1_ENGINE_H

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include "shaderManager.h"
#include "../shapes/circle.h"
#include "../shapes/rect.h"
#include "../shapes/shape.h"
#include "../shapes/triangle.h"
#include "FontRenderer.h"

using std::vector, std::unique_ptr, std::make_unique, glm::ortho, glm::mat4, glm::vec3, glm::vec4;
//enum State {start, path, mountain, beach, city, list, over};

/**
 * @brief The Engine class.
 * @details The Engine class is responsible for initializing the GLFW window, loading shaders, and rendering the game state.
 */
class Engine {
private:
    /// @brief The actual GLFW window.
    GLFWwindow* window{};

//    State screen;
    const int SIDE_LENGTH = 20;

    /// @brief The width and height of the window.
    const unsigned int width = 800, height = 600; // Window dimensions

    //TODO: added these lines of code
    /// @brief Responsible for rendering text on the screen.
    /// @details Initialized in initShaders()
    unique_ptr<FontRenderer> fontRenderer;
    // Shapes
    vector<unique_ptr<Shape>> endScreen;
    vector<unique_ptr<Shape>> startScreen;
    vector<unique_ptr<Shape>> beachChecklistScreen;
    vector<unique_ptr<Shape>> cityChecklistScreen;
    vector<unique_ptr<Shape>> mountainChecklistScreen;
    //vector<unique_ptr<Shape>> Checklist;

    // Shapes
    vector<unique_ptr<Circle>> bubbles;
    unique_ptr<Circle> specialBubble;
    unique_ptr<Rect> specialBubble2;
    const int RADIUS = 50;

    /// @brief Keyboard state (True if pressed, false if not pressed).
    /// @details Index this array with GLFW_KEY_{key} to get the state of a key.
    bool keys[1024];

    /// @brief Responsible for loading and storing all the shaders used in the project.
    /// @details Initialized in initShaders()
    unique_ptr <ShaderManager> shaderManager;

    unique_ptr <Rect> grass;
    unique_ptr <Rect> mainPath;
    unique_ptr <Rect> sidePathTopLeft;
    unique_ptr <Rect> sidePathTopRight;
    unique_ptr <Rect> sidePathBottomRight;
    unique_ptr <Rect> sidePathTopLeftOverlap;
    unique_ptr <Rect> sidePathTopRightOverlap;
    unique_ptr <Rect> sidePathBottomRightOverlap;
    vector <unique_ptr<Rect>> invisibleKeepOnPath;
    unique_ptr<Rect> checkList;
    unique_ptr<Rect> checkBoxBeach;
    unique_ptr<Rect> checkBoxCity;
    unique_ptr<Rect> checkBoxMountains;
    vector <unique_ptr<Rect>> buildingWindows;
    vector <unique_ptr<Rect>> buildings;
    unique_ptr<Rect> specialWindow;
    unique_ptr <Rect> user;
    vector <unique_ptr<Triangle>> mountains;
    unique_ptr <Triangle> snow;
    vector <unique_ptr<Rect>> sidePaths;

    bool alreadyMountain = false;
    bool alreadyCity = false;
    bool alreadyBeach = false;

    Shader shapeShader;

    double MouseX, MouseY;

public:
    /// @brief Constructor for the Engine class.
    /// @details Initializes window and shaders.
    Engine();

    /// @brief Destructor for the Engine class.
    ~Engine();

    /// @brief Initializes the GLFW window.
    /// @return 0 if successful, -1 otherwise.
    unsigned int initWindow(bool debug = false);

    void message();

    /// @brief Loads shaders from files and stores them in the shaderManager.
    /// @details Renderers are initialized here.
    void initShaders();

    /// @brief Initializes the shapes to be rendered.
    void initShapes();

    /// @brief Processes input from the user.
    /// @details (e.g. keyboard input, mouse input, etc.)
    void processInput();

    /// @brief Updates the game state.
    /// @details (e.g. collision detection, delta time, etc.)
    void update();

    //TODO: added here
    /// @brief Populates squares vector with input from file.
    void readFromFile1(string filepath);
    void readFromFile2(string filepath);

    /// @brief displays diff checklist based on enum state.
    void displayChecklist();

    /// @brief Renders the game state.
    /// @details Displays/renders objects on the screen.
    void render();

    /* deltaTime variables */
    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame (used to calculate deltaTime)

    /// @brief Returns true if the window should close.
    /// @details (Wrapper for glfwWindowShouldClose()).
    /// @return true if the window should close
    /// @return false if the window should not close
    bool shouldClose();

    /// @brief Projection matrix used for 2D rendering (orthographic projection).
    /// @details OpenGL uses the projection matrix to map the 3D scene to a 2D viewport.
    /// @details The projection matrix transforms coordinates in the camera space into
    /// normalized device coordinates. (view space to clip space).
    /// @note The projection matrix is used in the vertex shader.
    /// @note We don't have to change this matrix since the screen size never changes.
    mat4 PROJECTION = ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
};
#endif //FINAL_PROJECT_NBRADDOC_RROSE1_ENGINE_H