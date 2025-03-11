#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VBO
{
public:
  GLint type;
  bool dynamic;
  GLuint id;
  VBO(GLint type, bool dynamic)
  {
    this->type = type;
    this->dynamic = dynamic;
    glGenBuffers(1, &id);
  }

  ~VBO()
  {
    glDeleteBuffers(1, &id);
  }

  void bind() const
  {
    glBindBuffer(type, id);
  }

  void buffer(void *data, size_t count) const
  {
    this->bind();
    glBufferData(type, count, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
  }
};

#endif