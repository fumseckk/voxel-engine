#ifndef VAO_H
#define VAO_H

#include "vbo.h"

class VAO
{
public:
  GLuint id;

  VAO();
  ~VAO();
  void bind() const;
  void attr(const VBO &vbo, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset) const;
};

#endif