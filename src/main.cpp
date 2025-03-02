#include <cmath>
#include <ctime>
#include "gfx/gfx.h"
#include "ui/ui.h"
#include "cube.h"
#include "world/chunk.h"

// TODO Cube types with block atlas
// TODO edit noise to add water etc

int main(int argc, char** argv) {
  std::srand(std::time({}));
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Camera camera = Camera(glm::vec3(0.0f, (float)(WORLD_HEIGHT), 3.0f));

  World world;

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