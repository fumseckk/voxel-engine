#ifndef SSBO_H
#define SSBO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

class SSBO
{
public:
  Shader *shader;
  bool dynamic;
  GLuint id;

  SSBO(Shader *shader, bool dynamic);
  ~SSBO();
  void bind(GLuint base) const;
  void set_buffer(void *data, size_t count, GLuint base) const;
  void update_buffer(void *data, size_t count) const;
};

#endif