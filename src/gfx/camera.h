#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "window.h"

class Camera
{
public:
  Window &window;
  glm::vec3 world_up;
  glm::vec3 front, up, right;
  glm::vec3 position;
  float yaw, pitch;
  float fov;
  float znear, zfar;
  float aspect_ratio;
  float mvt_speed;
  float mouse_sensitivity;

  Camera(Window &window, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
  glm::mat4 get_view_matrix();
  glm::mat4 get_perspective_matrix();
  void keyboard_move();
  void mouse_move();
  void move();

private:
  void update_camera_vectors();
};

#endif