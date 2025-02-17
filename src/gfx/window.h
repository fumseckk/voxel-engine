#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

using namespace std;

static void _size_callback(GLFWwindow* _, int width, int height);
static void _cursor_callback(GLFWwindow* _, double pos_x, double pos_y);
static void _mouse_callback(GLFWwindow* _, int button, int action, int mods);
static void _key_callback(GLFWwindow* _, int key, int scancode, int action,
                          int mods);

typedef void VoidFn();

struct Button {
  bool pressed, down, down_before;
};

struct Mouse {
  Button buttons[GLFW_MOUSE_BUTTON_LAST];
  glm::vec2 position, delta_position;
};

struct Keyboard {
  Button keys[GLFW_KEY_LAST];
};

class Window {
 public:
  GLFWwindow* window = NULL;
  float width;
  float height;
  float frame_delta = 0;
  float frame_last = 0;
  long long frames = 0;
  long long last_second = 0;
  Mouse mouse;
  Keyboard keyboard;

 private:
  void _window_init(const string& name) {
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set up callbacks
    glfwSetFramebufferSizeCallback(window, _size_callback);
    glfwSetCursorPosCallback(window, _cursor_callback);
    glfwSetMouseButtonCallback(window, _mouse_callback);
    glfwSetKeyCallback(window, _key_callback);
  }

 public:
  Window(const string name) {
    this->width = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
    this->height = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, width, height,
                         GLFW_DONT_CARE);
    _window_init(name);
  }

  Window(const int width = 1980, const int height = 1080,
         const string name = "OpenGLProgram") {
    this->width = (float)width;
    this->height = (float)height;
    _window_init(name);
  }

  void begin_frame() {
    double now = glfwGetTime();
    frame_delta = now - frame_last;
    frame_last = now;
    frames++;
    if (now - last_second >= 1) {
      std::cout << "FPS : " << frames << std::endl;
      frames = 0;
      last_second = now;
    }
    update_buttons();
  }

  void end_frame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  operator GLFWwindow*() { return window; }

  void size_callback(int width, int height) {
    glViewport(0, 0, width, height);
    this->width = width;
    this->height = height;
  }

  void cursor_callback(int xpos, int ypos) {
    glm::vec2 pos(xpos, ypos);
    mouse.delta_position = mouse.position - pos;
    mouse.position = pos;
  }

  void mouse_callback(int button, int action, int mods) {
    if (button < 0) return;
    switch (action) {
      case GLFW_PRESS:
        keyboard.keys[button].down = true;
        break;
      case GLFW_RELEASE:
        keyboard.keys[button].down = false;
        break;
      default:
        break;
    }
  }

  void key_callback(int key, int scancode, int action, int mods) {
    if (key < 0) return;
    switch (action) {
      case GLFW_PRESS:
        keyboard.keys[key].down = true;
        break;
      case GLFW_RELEASE:
        keyboard.keys[key].down = false;
        break;
      default:
        break;
    }
  }

  void update_buttons() {
    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
      mouse.buttons[i].pressed =
          mouse.buttons[i].down && !mouse.buttons[i].down_before;
      mouse.buttons[i].down_before = mouse.buttons[i].down;
    }
    for (int i = 0; i < GLFW_KEY_LAST; i++) {
      keyboard.keys[i].pressed =
          keyboard.keys[i].down && !keyboard.keys[i].down_before;
      keyboard.keys[i].down_before = keyboard.keys[i].down;
    }

    // Exit if needed
    if (keyboard.keys[GLFW_KEY_Q].pressed) {
      glfwSetWindowShouldClose(window, true);
    }
    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //     cameraPos += cameraSpeed * cameraFront;
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //     cameraPos -= cameraSpeed * cameraFront;
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //     cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) *
    //     cameraSpeed;
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //     cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) *
    //     cameraSpeed;
    // }
  }
};

// Global window state
Window window = Window();
extern Window window;

// Call wrapper functions for the window object.
static void _size_callback(GLFWwindow* _, int width, int height) {
  window.size_callback(width, height);
}
static void _cursor_callback(GLFWwindow* _, double pos_x, double pos_y) {
  window.cursor_callback(pos_x, pos_y);
}
static void _mouse_callback(GLFWwindow* _, int button, int action, int mods) {
  window.mouse_callback(button, action, mods);
}
static void _key_callback(GLFWwindow* _, int key, int scancode, int action,
                          int mods) {
  window.key_callback(key, scancode, action, mods);
}

#endif