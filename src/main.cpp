
#include <cmath>

#include "gfx/gfx.h"
#include "ui/ui.h"
#include "cube.h"

// TODO
// chunk class
// render cubes have fun

extern Window window;

int main(int argc, char** argv) {
  glEnable(GL_DEPTH_TEST);

  Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
  Shader shader("resources/shaders/default.vert",
                "resources/shaders/default.frag");
  Texture tex("resources/textures/wooden_container.jpg");
  // UI ui(window);

  VAO cubeVAO;
  VBO cubeVBO(GL_ARRAY_BUFFER, false);
  cubeVBO.buffer(vertices, sizeof(vertices));
  cubeVAO.attr(cubeVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
  cubeVAO.attr(cubeVBO, 1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));

  shader.use();
  glm::mat4 p = camera.get_perspective_matrix();
  shader.uniform_mat4("p", p);

  while (!glfwWindowShouldClose(window)) {
    window.begin_frame();
    camera.move();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    tex.bind();
    shader.use();
    shader.uniform_texture2D("tex", tex, 0);
    glm::mat4 v = camera.get_view_matrix();
    shader.uniform_mat4("v", v);

    cubeVAO.bind();
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        for (int k = 0; k < 16; k++) {
          glm::mat4 m = glm::translate(glm::mat4(1.0f),
                                       glm::vec3((float)i, (float)j, (float)k));
          shader.uniform_mat4("m", m);
          glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
        }
      }
    }

    // ui.render();

    window.end_frame();
  }

  // Terminate
  glfwTerminate();
  exit(EXIT_SUCCESS);
}