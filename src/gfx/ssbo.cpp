#include "ssbo.h"
#include "shader.h"

SSBO::SSBO(Shader *shader, bool dynamic)
{
  this->shader = shader;
  this->dynamic = dynamic;
  glGenBuffers(1, &id);
}

SSBO::~SSBO()
{
  glDeleteBuffers(1, &id);
}

// note maybe need to bind the shader too when binding
void SSBO::bind(GLuint base) const
{
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, id);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
}

// maybe do not do this to update the buffer ? no need to
void SSBO::set_buffer(void *data, size_t count, GLuint base) const
{
  shader->use();
  this->bind(0);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, id);
  glBufferData(GL_SHADER_STORAGE_BUFFER, count, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void SSBO::update_buffer(void *data, size_t count) const
{
  shader->use();
  this->bind(0);
  glBufferData(GL_SHADER_STORAGE_BUFFER, count, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}