#include "engine.h"
#include <iostream>
#include <fstream>


const color skyBlue(135/255.0, 206/255.0, 235/255.0);
const color grassGreen(26/255.0, 176/255.0, 56/255.0);
const color darkGreen(27/255.0, 81/255.0, 45/255.0);
const color darkGreen2(24/255.0, 150/255.0, 60/255.0);
const color white(1, 1, 1);
const color brickRed(201/255.0, 20/255.0, 20/255.0);
const color darkBlue(1/255.0, 110/255.0, 214/255.0);
const color purple(119/255.0, 11/255.0, 224/255.0);
const color mediumPurple(119/255.0, 11/255.0, 224/255.0, 0.7);
const color lightPurple(119/255.0, 11/255.0, 224/255.0, 0.3);
const color invisiblePurple(119/255.0, 11/255.0, 224/255.0, 0);


const color black(0, 0, 0);
const color gray(75/255.0, 75/255.0, 75/255.0);
const color lightGray(140/255.0, 140/255.0, 140/255.0);
const color orange(1, 163/255.0, 22/255.0);

//TODO: each list will have to be its own state
const int userSize = 20;
int locationsVisited = 0;
int allLocationsVisited = 3;

enum State {start, end, path, mountain, beach, city, list, over,
    beachCheckList, completeCheckList, cityCheckList,
    mountainCheckList, restartScreen};
State screen;

Engine::Engine() : keys() {
    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(width, height, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert", "../res/shaders/shape.frag",  nullptr, "shape");

    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    // Set uniforms that never change
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    // Relative file path from inside cmake-build-debug folder to the txt file
    //readFromFile("../res/art/beachCheckList.txt");
    //readFromFile("../res/art/completeCheckList.txt");
    readFromFile2("../res/art/endScreen.txt");
    //readFromFile("../res/art/mountainCheckList.txt");
    //readFromFile("../res/art/restartScreen.txt");
    readFromFile1("../res/art/startScreen.txt");

    // TODO: Initialize the user to be a 10x10 white block centered at (0, 0)
    user = make_unique<Rect>(shapeShader, vec2(0, 0), vec2(10, 10), white);

    // Init grass
    grass = make_unique<Rect>(shapeShader, vec2(width/2, 50), vec2(width, height / 5), grassGreen);

    /*path*/
    user = make_unique<Rect>(shapeShader, vec2(width/25, height/2), vec2(userSize,userSize), white);
    mainPath = make_unique<Rect>(shapeShader, vec2(width/4, height/2), vec2(width, height/10), darkGreen);
    sidePathTopLeft = make_unique<Rect>(shapeShader, vec2(width/4, height), vec2(width/10, height/1.11), mediumPurple);
    sidePathTopRight = make_unique<Rect>(shapeShader, vec2(width/1.428, height), vec2(width/10, height/1.11), lightPurple);
    sidePathBottomRight = make_unique<Rect>(shapeShader, vec2(width/2, height/4), vec2(width/10, height/2.5), purple);

    //invisible overlapping shape to trigger user into next screen
    sidePathTopLeftOverlap = make_unique<Rect>(shapeShader, vec2(width/4, height), vec2(width/10, height/5), brickRed);
    sidePathTopRightOverlap = make_unique<Rect>(shapeShader, vec2(width/1.428, height), vec2(width/10, height/5), brickRed);
    sidePathBottomRightOverlap = make_unique<Rect>(shapeShader, vec2(width/2, height/10), vec2(width/10, height/10), brickRed);

    //force user to stay on path w/ invisible shapes around path
    invisibleKeepOnPath.push_back(make_unique<Rect>(shapeShader, vec2(0, height), vec2(width/2.5, height/1.11), skyBlue));
    invisibleKeepOnPath.push_back(make_unique<Rect>(shapeShader, vec2(width/2.13, height), vec2(width/2.75, height/1.11), skyBlue));
    invisibleKeepOnPath.push_back(make_unique<Rect>(shapeShader, vec2(width, height), vec2(width/2, height), skyBlue));
    invisibleKeepOnPath.push_back(make_unique<Rect>(shapeShader, vec2(width, 0), vec2(width/2, height), skyBlue));
    invisibleKeepOnPath.push_back(make_unique<Rect>(shapeShader, vec2(0, 0), vec2(width/1.11, height/1.11), skyBlue));
    float newRectX = sidePathBottomRight->getRight();
    float newRectWidth = mainPath->getRight() - newRectX;
    invisibleKeepOnPath.push_back(make_unique<Rect>(shapeShader, vec2(newRectX + 80, 0), vec2(newRectWidth/1, height/1.11), skyBlue));
    invisibleKeepOnPath.push_back(make_unique<Rect>(shapeShader, vec2(width/2, 0), vec2(width/10, height/10), skyBlue));

    /*checklist*/
//    checkList = make_unique<Rect>(shapeShader, vec2(width/2, height/2), vec2(width/2, height/1.5), lightPurple);

    /*mountains*/
    // Init mountains and snow
    mountains.push_back(make_unique<Triangle>(shapeShader, vec2(width/4, 300), vec2(width, 400), darkGreen));
    mountains.push_back(make_unique<Triangle>(shapeShader, vec2(2*width/3, 300), vec2(width, 500), darkGreen2));
    snow = make_unique<Triangle>(shapeShader, vec2(2*width/3, 510), vec2(width/6, 75), white);
//    mountains.push_back(make_unique<Triangle>(shapeShader, vec2(2*width/3, 510), vec2(width/6, 75), white));

    /*beach: add bubbles and special bubbles */
    int numberOfBubbles = 30;
    float minRadius = 30;
    float maxRadius = 40;

    for (int i = 0; i < numberOfBubbles; ++i) {
        float x = rand() % width;
        float y = rand() % height;
        float radius = rand() % int(maxRadius - minRadius) + minRadius;
        vec2 position(x, y);
        vec2 size (radius, radius);
        unique_ptr<Circle> bubble = make_unique<Circle>(shapeShader, position, size, mediumPurple);
        bubbles.push_back(std::move(bubble));
    }

    //special bubble
    float x = 200;
    float y = 125;
    //size
    float radius = 45;
    vec2 position2 (x,y);
    vec2 size2 (radius, radius);
    specialBubble = make_unique<Circle>(shapeShader, position2, size2, lightPurple);
    specialBubble2 = make_unique<Rect>(shapeShader, vec2(200.0, 125.0), vec2(45.0, 45.0), invisiblePurple);

//    bubbles.push_back(std::move(bubble2));

    /*city: add buildings*/
    // Init buildings from closest to furthest
    int totalBuildingWidth = 0;
    vec2 buildingSize;
    int i = 0; //building number

    //TODO: vec of diff colors used for the windows
    //note tha dark blue is the special window color
//    vector<color> windowColor = vector<skyBlue, grassGreen, darkGreen, purple, lightPurple>;
    //create a vector of colors to loop through for the windows
    // get 3 random floats between 0 and 1 for RGB
    vec4 randomColor(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 120 + 135);
    //if the color has not been selected from, then produce another color as the special window color
//    if(randomColor)
    while (totalBuildingWidth < width + 200) {
        // TODO: Populate this vector of gray buildings
        // Building height between 200-400
        buildingSize.y = rand() % 201 + 200;
        // Building width between 100-200
        buildingSize.x = rand() % 101 + 50;
        buildings3.push_back(make_unique<Rect>(shapeShader,
                                               vec2(totalBuildingWidth + (buildingSize.x / 2.0) + 5,
                                                    ((buildingSize.y / 2.0) + 50)),
                                               buildingSize, gray));

        //TODO: if you have time put this in a for loop
        for(int j = 20; j < buildings3[i]->getTop(); j += 40){
            //left windows (+)
            buildingWindows2.push_back(make_unique<Rect>(shapeShader,
                                                         vec2(buildings3[i]->getLeft() + 40,
                                                              buildings3[i]->getTop() - j),
                                                         vec2(20, 20), white));
            //right windows (-)
            buildingWindows2.push_back(make_unique<Rect>(shapeShader,
                                                         vec2(buildings3[i]->getRight() - 20,
                                                              buildings3[i]->getTop() - j),
                                                         vec2(20, 20), white));
            //special window
            specialBuilding = make_unique<Rect>(shapeShader,
                                                vec2(buildings3[0]->getLeft() + 40,
                                                     buildings3[0]->getTop() - 20),
                                                vec2(20, 20), darkBlue);
        }
        ++i;
        totalBuildingWidth += buildingSize.x + 5;
    }
}

void Engine::processInput() {
    glfwPollEvents();
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    //TODO: change found to actually work if the user clicks the item asked.
//    bool found = true;
//    if (screen == scavengerHunt1 && found)
//        screen = checkList; //change the state to the checklist if another item has been checked off
    //don't let the user go off the path
    switch (screen) {
        case start: {
            if (keys[GLFW_KEY_P] || keys[GLFW_KEY_S]) {
                screen = path;
            }
            break;
        }
        case path: {
            bool move= true;
            if (keys[GLFW_KEY_UP] && user->getPosY() < height - userSize / 2) {
                for (const unique_ptr<Rect>& r : invisibleKeepOnPath) {
                    if (r->isOverlapping(*user)) {
                        user->moveY(-5);
                        move= false;
                    }
                    if(move){
                        user->moveY(1);
                    }
                }
            }
            if (keys[GLFW_KEY_DOWN] && user->getPosY() > userSize / 2) {
                for (const unique_ptr<Rect>& r : invisibleKeepOnPath) {
                    if (r->isOverlapping(*user)) {
                        user->moveY(5);
                        move= false;
                    }
                    if(move){
                        user->moveY(-1);
                    }
                }
            }
            if (keys[GLFW_KEY_LEFT] && user->getPosX() > userSize / 2) {
                for (const unique_ptr<Rect>& r : invisibleKeepOnPath) {
                    if (r->isOverlapping(*user)) {
                        user->moveX(5);
                        move= false;
                    }
                    if(move){
                        user->moveX(-1);
                    }
                }
            }
            if (keys[GLFW_KEY_RIGHT] && user->getPosX() < width - userSize / 2) {
                for (const unique_ptr<Rect>& r : invisibleKeepOnPath) {
                    if (r->isOverlapping(*user)) {
                        user->moveX(-5);
                        move= false;
                    }
                    if(move)
                    {
                        user->moveX(1);
                    }
                }
            }
            break;
        }
        case mountain: {
            glfwGetCursorPos(window, &MouseX, &MouseY);
            MouseY = height - MouseY; // Flip y-axis
            mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            if (MouseX > snow->getLeft() &&
                MouseX < snow->getRight() &&
                MouseY > snow->getBottom() &&
                MouseY < snow->getTop() &&
                mousePressed) {
                locationsVisited+=1;
                cout <<"1 I have been to this many locations: " <<endl;
                cout <<locationsVisited <<endl;
                screen = path;
            }
            if (keys[GLFW_KEY_P]) {
                screen = path;
            }
            break;
        }
        case beach: {
            //if overlap and click the correct bubble then go to checklist
            glfwGetCursorPos(window, &MouseX, &MouseY);
            MouseY = height - MouseY; // Flip y-axis
            mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            if (MouseX > specialBubble2->getLeft() &&
                MouseX < specialBubble2->getRight() &&
                MouseY > specialBubble2->getBottom() &&
                MouseY < specialBubble2->getTop() &&
                mousePressed) {
                locationsVisited+=1;
                cout <<"1 I have been to this many locations: " <<endl;
                cout <<locationsVisited <<endl;
                screen = path;
            }
            if (keys[GLFW_KEY_P]) {
                screen = path;
            }
            break;
        }
        case city: {
            //TODO: eventually change to the 8-bit screen and then add chrono timer to bring back to the path
            //TODO: add displayChecklist() fnc to shorten process input checking the enum state first to see which checklist to display
            glfwGetCursorPos(window, &MouseX, &MouseY);
            MouseY = height - MouseY; // Flip y-axis
            mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            if (MouseX > specialBuilding->getLeft() &&
                MouseX < specialBuilding->getRight() &&
                MouseY > specialBuilding->getBottom() &&
                MouseY < specialBuilding->getTop() &&
                mousePressed) {
                locationsVisited+=1;
                cout <<"1 I have been to this many locations: " <<endl;
                cout <<locationsVisited <<endl;
                screen = path;
            }

            if (keys[GLFW_KEY_P]) {
                screen = path;
            }
            break;
        }
        case over: {
            break;
        }
    }

    // Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);
}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

/*
void Engine::message(){
    cout << "at the start" << endl;
    string message = "Press s to start";
    string message2 = "Rules:";
    string message3 = "Game consists 5x5 grid of lights. ";
    string message4 = "Pressing any lights will toggle";
    string message5 = "four adjacent lights.";
    string message6 = "Goal is to switch all lights off";
    // (12 * message.length()) is the offset to center text.
    // 12 pixels is the width of each character scaled by 1.
    this->fontRenderer->renderText(message, width / 2 - (5 * message.length()), height / 2, 1, vec3{1, 1, 1});
    this->fontRenderer->renderText(message2, 10, height / 2.5, 1, vec3{1, 1, 1});
    this->fontRenderer->renderText(message3, 10, height / 3, 1, vec3{1, 1, 1});
    this->fontRenderer->renderText(message4, 10, height / 3.5, 1, vec3{1, 1, 1});
    this->fontRenderer->renderText(message5, 10, height / 4, 1, vec3{1, 1, 1});
    this->fontRenderer->renderText(message6, 10, height / 6, 1, vec3{1, 1, 1});
}
 */

void Engine::readFromFile1(string filepath) {
    ifstream ins(filepath);
    if (!ins) {
        cout << "Error opening file" << endl;
    }
    ins >> std::noskipws;
    int xCoord = 0, yCoord = height - SIDE_LENGTH;
    char letter;
    bool draw;
    color c;
    while (ins >> letter) {
        draw = true;
        switch(letter) {
            case 'r': c = color(1, 0, 0); break;
            case 'g': c = color(0, 1, 0); break;
            case 'b': c = color(0, 0, 1); break;
            case 'y': c = color(1, 1, 0); break;
            case 'm': c = color(1, 0, 1); break;
            case 'c': c = color(0, 1, 1); break;
            case ' ': c = color(0, 0, 0); break;
            default: // newline
                draw = false;
                xCoord = 0;
                yCoord -= SIDE_LENGTH;
        }
        if (draw) {
            //TODO: work on reading each screen state in at a time, vector?
            startScreen.push_back(make_unique<Rect>(shapeShader, vec2(xCoord + SIDE_LENGTH/2, yCoord + SIDE_LENGTH/2), vec2(SIDE_LENGTH, SIDE_LENGTH), c));
            xCoord += SIDE_LENGTH;
        }
    }
    ins.close();
}

void Engine::readFromFile2(string filepath) {
    ifstream ins(filepath);
    if (!ins) {
        cout << "Error opening file" << endl;
    }
    ins >> std::noskipws;
    int xCoord = 0, yCoord = height - SIDE_LENGTH;
    char letter;
    bool draw;
    color c;
    while (ins >> letter) {
        draw = true;
        switch(letter) {
            case 'r': c = color(1, 0, 0); break;
            case 'g': c = color(0, 1, 0); break;
            case 'b': c = color(0, 0, 1); break;
            case 'y': c = color(1, 1, 0); break;
            case 'm': c = color(1, 0, 1); break;
            case 'c': c = color(0, 1, 1); break;
            case ' ': c = color(0, 0, 0); break;
            default: // newline
                draw = false;
                xCoord = 0;
                yCoord -= SIDE_LENGTH;
        }
        if (draw) {
            //TODO: work on reading each screen state in at a time, vector?
            endScreen.push_back(make_unique<Rect>(shapeShader, vec2(xCoord + SIDE_LENGTH/2, yCoord + SIDE_LENGTH/2), vec2(SIDE_LENGTH, SIDE_LENGTH), c));
            xCoord += SIDE_LENGTH;
        }
    }
    ins.close();
}

//TODO: change checklist screen state
void Engine::displayChecklist(){

}

void Engine::render() {
    switch (screen) {
        case start: {
            // Set shader to use for all shapes
            shapeShader.use();
            for (unique_ptr<Shape> &square : startScreen) {
                square->setUniforms();
                square->draw();
            }
            break;
        }
        case end: {
            // Set shader to use for all shapes
            glClearColor(black.red, black.green, black.blue, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            shapeShader.use();
            for (unique_ptr<Shape> &square : endScreen) {
                square->setUniforms();
                square->draw();
            }
            break;
        }
        case beachCheckList: {
            // Set shader to use for all shapes
            glClearColor(black.red, black.green, black.blue, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            shapeShader.use();
            for (unique_ptr<Shape> &square : beachChecklistScreen) {
                square->setUniforms();
                square->draw();
            }
            break;
        }
        //Add the rest of the cases here for the checklist Screens
        case path: {
            glClearColor(skyBlue.red, skyBlue.green, skyBlue.blue, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            mainPath->setUniforms();
            mainPath->draw();
            sidePathTopLeft->setUniforms();
            sidePathTopLeft->draw();
            sidePathTopRight->setUniforms();
            sidePathTopRight->draw();
            sidePathBottomRight->setUniforms();
            sidePathBottomRight->draw();

            //draw each segment of the invisible path
            for(const unique_ptr<Rect>& i : invisibleKeepOnPath){
                i->setUniforms();
                i->draw();
            }

            //user
            user->setUniforms();
            user->draw();

            //get to correct location
            if ((sidePathTopLeftOverlap->isOverlapping(*user)) && !alreadyMountain){ //m is the diff side paths
                screen = mountain; //change to mountainCheckList
                alreadyMountain = true;
            }
            if ((sidePathTopRightOverlap->isOverlapping(*user)) && !alreadyBeach){ //m is the diff side paths
                screen = beachCheckList;
                alreadyBeach = true;
            }
            if ((sidePathBottomRightOverlap->isOverlapping(*user)) && !alreadyCity) { //m is the diff side paths
                screen = city;
                alreadyCity = true;
            }
            if(locationsVisited == allLocationsVisited){
                screen = end;
                break;
            }
            break;
        }
        case mountain: {
            cout << "at the mountain" << endl;
            glClearColor(skyBlue.red, skyBlue.green, skyBlue.blue, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            for (const unique_ptr<Triangle>& m : mountains) {
                m->setUniforms();
                m->draw();
            }
            snow->setUniforms();
            snow->draw();

            grass->setUniforms();
            grass->draw();
            break;
        }
        case beach: {
            cout << "at the beach" << endl;
            glClearColor(skyBlue.red, skyBlue.green, skyBlue.blue, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            //draw half as brown sand

            //bubbles
            for (const unique_ptr<Circle>& bub : bubbles) {
                bub->setUniforms();
                bub->draw();
            }
            specialBubble->setUniforms();
            specialBubble->draw();
            //overlap
            specialBubble2->setUniforms();
            specialBubble2->draw();
            break;
        }
        case city: {
            glClearColor(skyBlue.red, skyBlue.green, skyBlue.blue, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            cout << "at the city" << endl;
            for (const unique_ptr<Rect> &b3: buildings3) { //green
                b3->setUniforms();
                b3->draw();
            }

            for (const unique_ptr<Rect> &bW2: buildingWindows2) { //blue
                bW2->setUniforms();
                bW2->draw();
            }

            specialBuilding->setUniforms();
            specialBuilding->draw();

            grass->setUniforms();
            grass->draw();
            break;
        }
        /*
        case beachCheckList: {
            //the following is just a placeholder
            // Set shader to use for all shapes
            shapeShader.use();
            for (unique_ptr<Shape> &square : startScreen) {
                square->setUniforms();
                square->draw();
            }
            ++locationsVisited;
            if(locationsVisited == allLocationsVisited){
                screen = completeCheckList;
            }
            if(keys[GLFW_KEY_SPACE]){
                screen = path;
            }
            break;
        }
        case cityCheckList: {
            //the following is just a placeholder
            // Set shader to use for all shapes
            shapeShader.use();
            for (unique_ptr<Shape> &square : startScreen) {
                square->setUniforms();
                square->draw();
            }
            ++locationsVisited;
            if(locationsVisited == allLocationsVisited){
                screen = completeCheckList;
            }
            if(keys[GLFW_KEY_SPACE]){
                screen = path;
            }
            break;
        }
        case completeCheckList: {
            //the following is just a placeholder
            // Set shader to use for all shapes
            shapeShader.use();
            for (unique_ptr<Shape> &square : startScreen) {
                square->setUniforms();
                square->draw();
            }
            break;
        }
         */
            //TODO: eventually git rid of and combine it with a case for overlapping and clicking to get to this stage
        case over: {

            break;
        }
    }
    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}