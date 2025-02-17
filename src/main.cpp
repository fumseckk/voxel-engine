
#include <cmath>

#include "gfx/gfx.h"
#include "cube.h"

// TODO
// camera class
// chunk class
// render cubes have fun

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float Z_NEAR = 0.1f;
const float Z_FAR = 100.0f;
const float FOV_DEGREES = 45.0f;
const float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

extern Window window; // todo does this work ? check with input

int main(int argc, char** argv) {
  glEnable(GL_DEPTH_TEST);

  Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
  Shader shader("resources/shaders/default.vert", "resources/shaders/default.frag");
  Texture tex("resources/textures/wooden_container.jpg");

  VAO cubeVAO; 
  VBO cubeVBO(GL_ARRAY_BUFFER, false);
  cubeVBO.buffer(vertices, sizeof(vertices));
  cubeVAO.attr(cubeVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
  cubeVAO.attr(cubeVBO, 1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));

  while (!glfwWindowShouldClose(window)) {
    window.begin_frame();
    camera.move();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Build shader uniforms
    shader.use();
    shader.uniform_texture2D("tex", tex, 0);
    glm::mat4 m = glm::rotate(glm::mat4(1.0f), glm::radians(-40.0f), glm::vec3(1.0f, 1.0f, 1.0f)); // rotate object
    glm::mat4 v = camera.get_view_matrix();
    glm::mat4 p = camera.get_perspective_matrix();
    ;
    shader.uniform_mat4("m", m);
    shader.uniform_mat4("v", v);
    shader.uniform_mat4("p", p);

    // Draw
    cubeVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

    window.end_frame();
  }

  // Terminate
  glfwTerminate();
  exit(EXIT_SUCCESS);
}