#ifndef VAO_H
#define VAO_H

#include "vbo.h"
#include <stdio.h>

class VAO {
public:
  GLuint id;

  VAO() {
    glGenVertexArrays(1, &id);
  }

  ~VAO() {
    glDeleteVertexArrays(1, &id);
  }

  void bind() {
    glBindVertexArray(id);
  }

  void attr(VBO& vbo, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset) {
    bind();
    vbo.bind();
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(index);
  }
};

#endif