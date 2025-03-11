#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <cstddef>

class VBO
{
public:
  GLint type;
  bool dynamic;
  GLuint id;
  VBO(GLint type, bool dynamic);

  ~VBO();

  void bind() const;

  void buffer(void *data, size_t count) const;
};

#endif