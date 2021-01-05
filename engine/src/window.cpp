#include "window.h"


Window::Window(int window_height, int window_width, char const *window_name) {
    height = window_height;
    width = window_width;
    name = window_name;
}


void Window::create() {
    // Initialize the GLFW library
    glfwInit();

    // Create the window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pane = glfwCreateWindow(height, width, name, nullptr, nullptr);

}


void Window::destroy() {
    // Destroy the window and terminate the library
    glfwDestroyWindow(pane);
    glfwTerminate();
}