#include "window.h"

#include <stdio.h>


void Window::init(int window_height, int window_width, char const *window_name, bool *engine_running) {
    height = window_height;
    width = window_width;
    name = window_name;
    running = engine_running;

    // Initialize the GLFW library
    glfwInit();

    // Create the window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    this_window = glfwCreateWindow(height, width, name, nullptr, nullptr);
}


void Window::destroy() {
    // Destroy the window and terminate the library
    glfwDestroyWindow(this_window);
    glfwTerminate();
}


void Window::processInput() {
    // Get all the events from the input queue and call them (ie keyCallback)
    glfwPollEvents();

    // Shut down the game if the window was told to close
    if (glfwWindowShouldClose(this_window) ) { *running = false; }
}


int Window::getKeyState(int key) {
    return glfwGetKey(this_window, key);
}
