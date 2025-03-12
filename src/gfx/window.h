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
  bool pressed: 1;
  bool down: 1;
  bool down_before: 1;
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
  void update_buttons();
  operator GLFWwindow *();
  static void size_callback(GLFWwindow* window, int width, int height);
  static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
  static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif