#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"

extern Window window;

class Camera {
 public:
  glm::vec3 world_up;
  glm::vec3 front, up, right;
  glm::vec3 position;
  float yaw, pitch;
  float fov;
  float znear, zfar;
  float aspect_ratio;
  float mvt_speed;
  float mouse_sensitivity;

  Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f,
         float pitch = 0.0f) {
    this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->position = pos;
    this->yaw = yaw;
    this->pitch = pitch;
    this->fov = glm::radians(100.0f);
    this->znear = 0.01f;
    this->zfar = 300.0f;
    this->aspect_ratio = window.width / window.height;
    this->mvt_speed = 10.0f;
    this->mouse_sensitivity = 0.1f;
    update_camera_vectors();
  }

  glm::mat4 get_view_matrix() {
    return glm::lookAt(position, position + front, up);
  }

  glm::mat4 get_perspective_matrix() {
    return glm::perspective(fov, aspect_ratio, znear, zfar);
  }

  void keyboard_move() {
    float vel = mvt_speed * window.frame_delta;
    if (window.keyboard.keys[GLFW_KEY_W].down) position += glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * vel;
    if (window.keyboard.keys[GLFW_KEY_A].down) position -= glm::normalize(glm::vec3(right.x, 0.0f, right.z)) * vel;
    if (window.keyboard.keys[GLFW_KEY_S].down) position -= glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * vel;
    if (window.keyboard.keys[GLFW_KEY_D].down) position += glm::normalize(glm::vec3(right.x, 0.0f, right.z)) * vel;
    if (window.keyboard.keys[GLFW_KEY_SPACE].down) position += world_up * vel;
    if (window.keyboard.keys[GLFW_KEY_LEFT_CONTROL].down)
      position -= world_up * vel;
  }

  void mouse_move() {
    glm::vec2 delta = window.mouse.delta_position;
    delta *= mouse_sensitivity;
    yaw -= delta.x;
    pitch += delta.y;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    window.mouse.delta_position = glm::vec2(0.0f, 0.0f);
    update_camera_vectors();
  }

  void move() {
    keyboard_move();
    mouse_move();
  }

 private:
  void update_camera_vectors() {
    // reset front vector from yaw and pitch
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // gram-shmidt to get new basis
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
  }
};

#endif