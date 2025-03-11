#include "vao.h"

VAO::VAO()
{
  glGenVertexArrays(1, &id);
}

VAO::~VAO()
{
  glDeleteVertexArrays(1, &id);
}

void VAO::bind() const
{
  glBindVertexArray(id);
}

void VAO::attr(const VBO &vbo, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset) const
{
  this->bind();
  vbo.bind();
  glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *)offset);
  glEnableVertexAttribArray(index);
}
