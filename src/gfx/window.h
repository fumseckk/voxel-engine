#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

using namespace std;

// This class uses OpenGL 3.3

class Window {
 public:
  GLFWwindow* window = NULL;

  Window(const int width, const int height, const string name) {
    // Init Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = NULL;
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create OpenGL window" << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    // Init GLAD
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD loader" << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    // Set up viewport
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback((GLFWwindow*)window,
                                   framebuffer_size_callback);
  }

  operator GLFWwindow*() { return window; }

  void processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, true);
    }
  }

 private:
  static void framebuffer_size_callback(GLFWwindow* window, int width,
                                        int height) {
    glViewport(0, 0, width, height);
  }
};

#endif