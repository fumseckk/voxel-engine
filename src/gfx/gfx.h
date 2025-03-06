#ifndef GFX_H
#define GFX_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TOOD move this
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "shader.h"
#include "vbo.h"
#include "vao.h"
#include "ssbo.h"
#include "texture.h"
#include "camera.h"
#include "blockatlas.h"

GLenum glCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
      std::string error;
      switch (errorCode) {
        case GL_INVALID_ENUM:
          error = "INVALID_ENUM";
          break;
        case GL_INVALID_VALUE:
          error = "INVALID_VALUE";
          break;
        case GL_INVALID_OPERATION:
          error = "INVALID_OPERATION";
          break;
        case GL_OUT_OF_MEMORY:
          error = "OUT_OF_MEMORY";
          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          error = "INVALID_FRAMEBUFFER_OPERATION";
          break;
      }
      std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
  }
  #define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif