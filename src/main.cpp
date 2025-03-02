#include <cmath>
#include <ctime>
#include "gfx/gfx.h"
#include "ui/ui.h"
#include "cube.h"
#include "world/chunk.h"

// TODO
// edit noise to add water etc
// face shadow
// Cube types ? Block atlas or not ?

int main(int argc, char** argv) {
  std::srand(std::time({}));
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Camera camera = Camera(
      glm::vec3(0.0f, (float)(CHUNKS_SIZE * VERTICAL_CHUNKS), 3.0f));  // Position the camera to view the cube

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