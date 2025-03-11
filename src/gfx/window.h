#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

using namespace std;

static void _size_callback(GLFWwindow *window, int width, int height);
static void _cursor_callback(GLFWwindow *_, double pos_x, double pos_y);
static void _mouse_callback(GLFWwindow *_, int button, int action, int mods);
static void _key_callback(GLFWwindow *_, int key, int scancode, int action,
                          int mods);

typedef void VoidFn();

struct Button
{
  bool pressed, down, down_before;
};

struct Mouse
{
  Button buttons[GLFW_MOUSE_BUTTON_LAST] = {0};
  glm::vec2 position, delta_position = glm::vec2(0.0f, 0.0f);
};

struct Keyboard
{
  Button keys[GLFW_KEY_LAST] = {0};
};

class Window
{
public:
  GLFWwindow *window = NULL;
  float width;
  float height;
  bool wireframe = false;
  float frame_delta = 0;
  float frame_last = 0;
  long long frames = 0;
  long long last_second = 0;
  Mouse mouse;
  Keyboard keyboard;

private:
  void _window_init(const string &name)
  {
    // Init Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    window = NULL;
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == NULL)
    {
      std::cout << "Failed to create OpenGL window" << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    // Init GLAD
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      std::cout << "Failed to initialize GLAD" << std::endl;
      exit(EXIT_FAILURE);
    }

    // Set up viewport
    glViewport(0, 0, width, height);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    mouse.position = glm::vec2(xpos, ypos);

    // Set up callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, _size_callback);
    glfwSetCursorPosCallback(window, _cursor_callback);
    glfwSetMouseButtonCallback(window, _mouse_callback);
    glfwSetKeyCallback(window, _key_callback);
  }

public:
  Window(const string name)
  {
    this->width = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
    this->height = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, width, height,
                         GLFW_DONT_CARE);
    _window_init(name);
  }

  Window(const int width = 1980, const int height = 1080,
         const string name = "OpenGLProgram")
  {
    this->width = (float)width;
    this->height = (float)height;
    _window_init(name);
  }

  void begin_frame()
  {
    double now = glfwGetTime();
    frame_delta = now - frame_last;
    frame_last = now;
    frames++;
    if (now - last_second >= 1)
    {
      std::cout << "FPS : " << frames << std::endl;
      frames = 0;
      last_second = now;
    }
    update_buttons();
  }

  void end_frame()
  {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  operator GLFWwindow *() { return window; }

  void size_callback(int width, int height)
  {
    glViewport(0, 0, width, height);
    this->width = width;
    this->height = height;
  }

  void cursor_callback(int xpos, int ypos)
  {
    glm::vec2 pos(xpos, ypos);
    mouse.delta_position = glm::clamp(mouse.position - pos, -100.0f, 100.0f);
    mouse.position = pos;
  }

  void mouse_callback(int button, int action, int mods)
  {
    if (button < 0)
      return;
    switch (action)
    {
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

  void key_callback(int key, int scancode, int action, int mods)
  {
    if (key < 0)
      return;
    switch (action)
    {
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

  void update_buttons()
  {
    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
    {
      mouse.buttons[i].pressed = mouse.buttons[i].down && !mouse.buttons[i].down_before;
      mouse.buttons[i].down_before = mouse.buttons[i].down;
    }
    for (int i = 0; i < GLFW_KEY_LAST; i++)
    {
      keyboard.keys[i].pressed = keyboard.keys[i].down && !keyboard.keys[i].down_before;
      keyboard.keys[i].down_before = keyboard.keys[i].down;
    }
    // Exit if needed, move this ?
    if (keyboard.keys[GLFW_KEY_Q].pressed)
      glfwSetWindowShouldClose(window, true);
    if (keyboard.keys[GLFW_KEY_LEFT_ALT].pressed)
    {
      wireframe = !wireframe;
      if (wireframe)
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      else
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
    }
  }

  // Call wrapper functions for the window object.
  static void _size_callback(GLFWwindow *window, int width, int height)
  {
    Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
    this_->size_callback(width, height);
  }
  static void _cursor_callback(GLFWwindow *window, double pos_x, double pos_y)
  {
    Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
    this_->cursor_callback(pos_x, pos_y);
  }
  static void _mouse_callback(GLFWwindow *window, int button, int action, int mods)
  {
    Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
    this_->mouse_callback(button, action, mods);
  }
  static void _key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
  {
    Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
    this_->key_callback(key, scancode, action, mods);
  }
};

#endif