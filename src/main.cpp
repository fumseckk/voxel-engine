#include <cmath>
#include <ctime>
#include "gfx/gfx.h"
#include "ui/ui.h"
#include "cube.h"
#include "world/chunk.h"

// TODO
// chunk class
// render cubes have fun

extern Window window;

int main(int argc, char** argv) {
  std::srand(std::time({}));
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Camera camera = Camera(
      glm::vec3(0.0f, 0.0f, 3.0f));  // Position the camera to view the cube

  World world;


  // TODO WHYW is this necessary ??? DO NOT REMOVE
  VAO vao;
  VBO vbo(GL_ARRAY_BUFFER, false);
  vbo.buffer(vertices, sizeof(vertices));
  vao.attr(vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
  vao.attr(vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),
                3 * sizeof(float));

  world.shader.use();
  world.prepare(camera);



  while (!glfwWindowShouldClose(window)) {
    window.begin_frame();
    camera.move();
    world.texture.bind();
    world.render(camera);
    window.end_frame();
  }

  // Terminate
  glfwTerminate();
  exit(EXIT_SUCCESS);
}