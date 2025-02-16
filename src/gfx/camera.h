// #ifndef CAMERA_H
// #define CAMERA_H

// enum Direction {
//   FORWARD,
//   BACKWARD,
//   LEFT,
//   RIGHT,
// };

// class Camera {
// public:
//   glm::vec3 position;
//   glm::vec3 front, up, right;
//   glm::vec3 world_up;
//   float yaw, pitch;
//   float mvt_speed, mvt_sensitivity;

//   Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 world_up = glm::vec3(1.0f, 1.0f, 1.0f), float yaw=YAW, floatpitch=PITCH, ) {
//     position = pos;
//     world_up = up;
//   }

//   glm::mat4 get_view_matrix() {
//     return glm::lookAt(position, position + front, up);
//   }

//   void move_direction(Direction dir, float delta_time) {
//     float vel = mvt_speed * delta_time;
//     switch (dir) {
//       case FORWARD:
//         position += front * vel;
//         break;
//       case BACKWARD:
//         position -= front * vel;
//         break;
//       case LEFT:
//         position -= right * vel;
//         break;
//       case RIGHT:
//         position += right * vel;
//         break;
//     }

//     update_camera_vectors();
//   }

// private:
//   void update_camera_vectors() {
//     return;
//   }
// };

// #endif