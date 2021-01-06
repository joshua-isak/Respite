#include "window.h"

#include <stdio.h>


void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        printf("hey you pressed the escape key!\n");
    }
}


void Window::init(int window_height, int window_width, char const *window_name) {
    height = window_height;
    width = window_width;
    name = window_name;

    // Initialize the GLFW library
    glfwInit();

    // Create the window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    this_window = glfwCreateWindow(height, width, name, nullptr, nullptr);

    // Set key_callback function
    glfwSetKeyCallback(this_window, keyCallback);

}


void Window::destroy() {
    // Destroy the window and terminate the library
    glfwDestroyWindow(this_window);
    glfwTerminate();
}


void Window::processInput() {
    // Get all the events from the input queue and call them (ie keyCallback)
    glfwPollEvents();

    int state = glfwGetKey(this_window, GLFW_KEY_E);

    if (state == GLFW_PRESS) {
        printf("The E key is being pressed\n");
    }
}


int Window::getKeyState(int key) {
    return glfwGetKey(this_window, key);
}
