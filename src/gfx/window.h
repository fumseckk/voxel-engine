#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace std;

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
  void _window_init(const string &name);

public:
  Window(const string name);
  Window(const int width = 1980, const int height = 1080, const string name = "OpenGLProgram");
  void begin_frame();
  void end_frame();
  operator GLFWwindow *();
  void size_callback(int width, int height);
  void cursor_callback(int xpos, int ypos);
  void mouse_callback(int button, int action, int mods);
  void key_callback(int key, int scancode, int action, int mods);
  void update_buttons();

  // Call wrapper functions for the window object.
  static void _size_callback(GLFWwindow *window, int width, int height);
  static void _cursor_callback(GLFWwindow *window, double pos_x, double pos_y);
  static void _mouse_callback(GLFWwindow *window, int button, int action, int mods);
  static void _key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
};

#endif