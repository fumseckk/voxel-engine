#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstring>
#include "texture.h"

struct VertexAttr
{
  GLuint index;
  std::string name;
};

class Shader
{
public:
  unsigned int id, vs_id, fs_id, gs_id;

  Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);
  ~Shader();
  void use();
  void uniform_bool(const std::string &name, bool value) const;
  void uniform_int(const std::string &name, int value) const;
  void uniform_uint(const std::string &name, int value) const;
  void uniform_float(const std::string &name, float value) const;
  void uniform_vec2(const std::string &name, const glm::vec2 &value) const;
  void uniform_vec2(const std::string &name, float x, float y) const;
  void uniform_vec3(const std::string &name, const glm::vec3 &value) const;
  void uniform_vec3(const std::string &name, float x, float y, float z) const;
  void uniform_vec4(const std::string &name, const glm::vec4 &value) const;
  void uniform_vec4(const std::string &name, float x, float y, float z, float w) const;
  void uniform_mat2(const std::string &name, const glm::mat2 &mat) const;
  void uniform_mat3(const std::string &name, const glm::mat3 &mat) const;
  void uniform_mat4(const std::string &name, const glm::mat4 &mat) const;
  void uniform_texture2D(const std::string &name, Texture tex, GLuint n);
  void log_errors(GLuint shader, const std::string &type);
};

#endif