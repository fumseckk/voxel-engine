#include "vbo.h"

#include <glad/glad.h>

VBO::VBO(GLint type, bool dynamic)
{
  this->type = type;
  this->dynamic = dynamic;
  glGenBuffers(1, &id);
}

VBO::~VBO()
{
  glDeleteBuffers(1, &id);
}

void VBO::bind() const
{
  glBindBuffer(type, id);
}

void VBO::buffer(void *data, size_t count) const
{
  this->bind();
  glBufferData(type, count, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}