#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture {
public:
  GLuint id;
  glm::vec2 size;

  // Texture(const string& path)

  Texture(const string& path) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for no interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    assert(width % 16 == 0);
    assert(height % 16 == 0);
    if (!data) {
      std::cerr << "Failed to load texture" << std::endl;
    }
    else {
      GLenum format;
      if (nrChannels == 1)
          format = GL_RED;
      else if (nrChannels == 3)
          format = GL_RGB;
      else if (nrChannels == 4)
          format = GL_RGBA;
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
  }

  ~Texture() {
    glDeleteTextures(1, &id);
  }

  void bind() {
    glBindTexture(GL_TEXTURE_2D, id);
  }
};

#endif