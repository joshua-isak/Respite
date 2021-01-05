#pragma once

#include <engine/lib/glfw3.h>

class Window {
public:
    GLFWwindow *pane;   //The GLFW window object
    const char *name;
    int height;
    int width;

    // Constructor
    Window(int window_height, int window_width, char const *window_name);

    // Create the window
    void create();

    // Destroy the window
    void destroy();
};