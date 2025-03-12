
#include "window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace std;

void Window::_window_init(const string &name)
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
  glfwSetFramebufferSizeCallback(window, size_callback);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetMouseButtonCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);
}

Window::Window(const string name)
{
  this->width = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
  this->height = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
  glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, width, height,
                       GLFW_DONT_CARE);
  _window_init(name);
}

Window::Window(const int width, const int height, const string name)
{
  this->width = (float)width;
  this->height = (float)height;
  _window_init(name);
}

void Window::begin_frame()
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

void Window::end_frame()
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

Window::operator GLFWwindow *() { return window; }

void Window::size_callback(GLFWwindow* window, int width, int height)
{
  Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
  glViewport(0, 0, width, height);
  this_->width = width;
  this_->height = height;
}

void Window::cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
  Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
  glm::vec2 pos(xpos, ypos);
  this_->mouse.delta_position = glm::clamp(this_->mouse.position - pos, -100.0f, 100.0f);
  this_->mouse.position = pos;
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
  Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (button < 0)
    return;
  switch (action)
  {
  case GLFW_PRESS:
    this_->keyboard.keys[button].down = true;
    break;
  case GLFW_RELEASE:
    this_->keyboard.keys[button].down = false;
    break;
  default:
    break;
  }
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  Window *this_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (key < 0)
    return;
  switch (action)
  {
  case GLFW_PRESS:
    this_->keyboard.keys[key].down = true;
    break;
  case GLFW_RELEASE:
    this_->keyboard.keys[key].down = false;
    break;
  default:
    break;
  }
}

void Window::update_buttons()
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
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}