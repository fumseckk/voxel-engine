#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VBO {
public:
  GLint type;
  bool dynamic;
  GLuint id;
  VBO(GLint type, bool dynamic) {
    this->type = type;
    this->dynamic = dynamic;
    glGenBuffers(1, &id);
  }

  ~VBO() {
    glDeleteBuffers(1, &id);
  }

  void bind() {
    glBindBuffer(type, id);
  }

  void buffer(void* data, size_t count) {
    this->bind();
    glBufferData(type, count, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
  }
};

#endif