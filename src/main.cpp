#include <cmath>
#include <ctime>
#include "gfx/gfx.h"
#include "ui/ui.h"
#include "world/world.h"
#include <cassert>

int main(int argc, char **argv)
{
  Window window;
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Camera camera = Camera(window, glm::vec3(0.0f, (float)(WORLD_HEIGHT), 3.0f));

  World world;

  world.shader.use();
  world.prepare(camera);
  while (!glfwWindowShouldClose(window))
  {
    window.begin_frame();
    camera.move();
    world.render(camera);
    window.end_frame();
    glCheckError();
  }

  // Terminate
  glfwTerminate();
  exit(EXIT_SUCCESS);
}