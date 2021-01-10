#pragma once

#include <engine/lib/glfw3.h>

class Window {
public:
    GLFWwindow *this_window;   //The GLFW window object
    const char *name;
    int height;
    int width;

    // Callback function for when a keyboard key is interacted with
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Process window events (callback functions)
    void processInput();

    // Create the window and initialize GLFW library
    void init(int window_height, int window_width, char const *window_name);

    // Destroy the window and terminate GLFW library
    void destroy();

    // Return the state of a key
    int getKeyState(int key);

};

// Corresponds 1 to 1 with GLFW_RELEASED and GLFW_PRESSED
enum key_state {KEY_RELEASED, KEY_PRESSED};