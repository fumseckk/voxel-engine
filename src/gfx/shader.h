#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include "texture.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>

using namespace std;

const int SIZE = 1024;

struct VertexAttr {
  GLuint index;
  string name;
};

class Shader {
 public:
  unsigned int prog_id, vs_id, fs_id;

  // Constructor reads and builds the shader
  Shader(const char* vertexPath, const char* fragmentPath) {
    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;
    vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    try 
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        vShaderFile.close();
        fShaderFile.close();
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    } catch (const ifstream::failure& e) {
      std::cerr << "Failed to read shader file: " << e.what() << endl;
    }

    const char* cVertCode = vertexCode.c_str();
    const char* cFragCode = fragmentCode.c_str();

    // Vertex shader
    int success;
    char infolog[SIZE];
    vs_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs_id, 1, &cVertCode, NULL);
    glCompileShader(vs_id);
    log_errors(vs_id, "VERTEX_SHADER");

    // Fragment shader
    fs_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs_id, 1, &cFragCode, NULL);
    glCompileShader(fs_id);
    log_errors(fs_id, "FRAGMENT_SHADER");

    prog_id = glCreateProgram();
    glAttachShader(prog_id, vs_id);
    glAttachShader(prog_id, fs_id);
    glLinkProgram(prog_id);
    log_errors(prog_id, "PROGRAM");
  }

  ~Shader() {
    glDeleteShader(vs_id);
    glDeleteShader(fs_id);
    glDeleteProgram(prog_id);
  }

  // Activate the shader
  void use() {
    glUseProgram(prog_id);
  }
  void uniform_bool(const string& name, bool value) const {
    glUniform1i(glGetUniformLocation(prog_id, name.c_str()), value);
  }
  void uniform_int(const string& name, int value) const {
    glUniform1i(glGetUniformLocation(prog_id, name.c_str()), value);
  }
  void uniform_uint(const string& name, int value) const {
    glUniform1ui(glGetUniformLocation(prog_id, name.c_str()), value);
  } 
  void uniform_float(const string& name, float value) const {
    glUniform1f(glGetUniformLocation(prog_id, name.c_str()), value);
  }
  void uniform_vec2(const string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(prog_id, name.c_str()), 1, &value[0]);
  }
  void uniform_vec2(const string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(prog_id, name.c_str()), x, y);
  }
  void uniform_vec3(const string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(prog_id, name.c_str()), 1, &value[0]);
  }
  void uniform_vec3(const string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(prog_id, name.c_str()), x, y, z);
  }
  void uniform_vec4(const string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(prog_id, name.c_str()), 1, &value[0]);
  }
  void uniform_vec4(const string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(prog_id, name.c_str()), x, y, z, w);
  }
  void uniform_mat2(const string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(prog_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  void uniform_mat3(const string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(prog_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  void uniform_mat4(const string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(prog_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  void uniform_texture2D(const string& name, Texture tex, GLuint n) {
    glActiveTexture(GL_TEXTURE0 + n);
    tex.bind();
    glUniform1i(glGetUniformLocation(prog_id, name.c_str()), n);
  }

private:
  void log_errors(GLuint shader, const string& type) {
    int success;
    char infolog[SIZE];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, SIZE, NULL, infolog);
        std::cerr << "Failed to compile shader: " << infolog
                  << std::endl;
      }
    }
    else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shader, SIZE, NULL, infolog);
        std::cerr << "Failed to link shader: " << infolog
                  << std::endl;
      }
    }
  }
};

#endif