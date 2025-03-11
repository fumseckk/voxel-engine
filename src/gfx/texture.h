#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <string>


class TextureArray
{
public:
  GLuint id;
  int layer_count;

  TextureArray(const std::string &folder_path);
  ~TextureArray()
  {
    if (id != 0)
      glDeleteTextures(1, &id);
  }

  void bind(GLuint textureUnit = 0)
  {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
  }
};

class Texture
{
public:
  GLuint id;

  Texture(const std::string &path);
  ~Texture() { glDeleteTextures(1, &id); }
  void bind() { glBindTexture(GL_TEXTURE_2D, id); }
};

#endif