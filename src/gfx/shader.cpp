#include "shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>

using namespace std;

const int SIZE = 1024;

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath)
{
  std::string vertexCode, fragmentCode, geometryCode;
  std::ifstream vShaderFile, fShaderFile, gShaderFile;
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try
  {
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vShaderFile.close();
    fShaderFile.close();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    if (geometryPath != nullptr)
    {
      gShaderFile.open(geometryPath);
      std::stringstream gShaderStream;
      gShaderStream << gShaderFile.rdbuf();
      gShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  }
  catch (std::ifstream::failure &e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();
  unsigned int vertex, fragment;
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  log_errors(vertex, "VERTEX");
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  log_errors(fragment, "FRAGMENT");

  unsigned int geometry;
  if (geometryPath != nullptr)
  {
    const char *gShaderCode = geometryCode.c_str();
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    log_errors(geometry, "GEOMETRY");
  }
  // shader Program
  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  if (geometryPath != nullptr)
    glAttachShader(id, geometry);
  glLinkProgram(id);
  log_errors(id, "PROGRAM");
  // delete the shaders as they're linked into our program now and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (geometryPath != nullptr)
    glDeleteShader(geometry);
}

Shader::~Shader()
{
  glDeleteShader(vs_id);
  glDeleteShader(fs_id);
  glDeleteShader(gs_id);
  glDeleteProgram(id);
}

// Activate the shader
void Shader::use()
{
  glUseProgram(id);
}

void Shader::uniform_bool(const string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::uniform_int(const string &name, int value) const
{
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::uniform_uint(const string &name, int value) const
{
  glUniform1ui(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::uniform_float(const string &name, float value) const
{
  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::uniform_vec2(const string &name, const glm::vec2 &value) const
{
  glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::uniform_vec2(const string &name, float x, float y) const
{
  glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::uniform_vec3(const string &name, const glm::vec3 &value) const
{
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::uniform_vec3(const string &name, float x, float y, float z) const
{
  glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::uniform_vec4(const string &name, const glm::vec4 &value) const
{
  glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::uniform_vec4(const string &name, float x, float y, float z, float w) const
{
  glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::uniform_mat2(const string &name, const glm::mat2 &mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::uniform_mat3(const string &name, const glm::mat3 &mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::uniform_mat4(const string &name, const glm::mat4 &mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::uniform_texture2D(const string &name, Texture tex, GLuint n)
{
  glActiveTexture(GL_TEXTURE0 + n);
  tex.bind();
  glUniform1i(glGetUniformLocation(id, name.c_str()), n);
}

void Shader::log_errors(GLuint shader, const string &type)
{
  int success;
  char infolog[SIZE];
  if (type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {

      glGetShaderInfoLog(shader, SIZE, NULL, infolog);
      std::cerr << "Failed to compile shader " << type << ": " << infolog << std::endl;
    }
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader, SIZE, NULL, infolog);
      std::cerr << "Failed to link shader: " << infolog << std::endl;
    }
  }
}