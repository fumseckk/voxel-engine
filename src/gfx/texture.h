#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <string>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include "stb_image.h"

class TextureArray {
 public:
  GLuint id;
  int layer_count;

  TextureArray(const std::string& folder_path) {
    // Get all image files in the directory
    std::vector<std::filesystem::path> image_paths;
    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
      if (entry.is_regular_file()) {
        // Check if it's an image file
        std::string ext = entry.path().extension().string();
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" ||
            ext == ".tga") {
          image_paths.push_back(entry.path());
        }
      }
    }

    if (image_paths.empty()) {
      std::cerr << "No valid image files found in: " << folder_path
                << std::endl;
      return;
    }

    // Sort images
    std::sort(image_paths.begin(), image_paths.end());
    layer_count = image_paths.size();

    // Load the first image to determine dimensions
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* first_data = stbi_load(image_paths[0].string().c_str(),
                                          &width, &height, &channels, 0);

    if (!first_data) {
      std::cerr << "Failed to load first texture: " << image_paths[0].string()
                << std::endl;
      return;
    }

    // Determine format
    GLenum format;
    if (channels == 1)
      format = GL_RED;
    else if (channels == 3)
      format = GL_RGB;
    else if (channels == 4)
      format = GL_RGBA;
    else {
      std::cerr << "Unsupported number of channels: " << channels << std::endl;
      stbi_image_free(first_data);
      return;
    }

    // Create the texture array
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    // Set texture parameters (no interpolation)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Allocate storage for the texture array
    glTexImage3D(GL_TEXTURE_2D_ARRAY,
                 0,                 // Mipmap level
                 format,            // Internal format
                 width, height,     // Width, height
                 layer_count,       // Number of layers
                 0,                 // Border
                 format,            // Format
                 GL_UNSIGNED_BYTE,  // Type
                 nullptr            // Data (will be uploaded separately)
    );

    // Upload the first image
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, format,
                    GL_UNSIGNED_BYTE, first_data);
    stbi_image_free(first_data);

    // Load and upload the rest of the images
    for (int i = 1; i < layer_count; ++i) {
      int img_width, img_height, img_channels;
      unsigned char* data =
          stbi_load(image_paths[i].string().c_str(), &img_width, &img_height,
                    &img_channels, 0);

      if (!data) {
        std::cerr << "Failed to load texture: " << image_paths[i].string()
                  << std::endl;
        continue;
      }

      // Verify consistent dimensions and format
      if (img_width != width || img_height != height ||
          img_channels != channels) {
        std::cerr << "Inconsistent image dimensions or channels: "
                  << image_paths[i].string() << std::endl;
        stbi_image_free(data);
        continue;
      }

      // Upload this layer
      glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, format,
                      GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    }

    std::cout << "Loaded texture array with " << layer_count << " layers from "
              << folder_path << std::endl;
  }

  ~TextureArray() {
    if (id != 0) {
      glDeleteTextures(1, &id);
    }
  }

  void bind(GLuint textureUnit = 0) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
  }
};

class Texture {
 public:
  GLuint id;

  // Texture(const string& path)

  Texture(const string& path) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // GL_NEAREST for no interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    assert(width % 16 == 0);
    assert(height % 16 == 0);
    if (!data) {
      std::cerr << "Failed to load texture" << std::endl;
    } else {
      GLenum format;
      if (nrChannels == 1)
        format = GL_RED;
      else if (nrChannels == 3)
        format = GL_RGB;
      else if (nrChannels == 4)
        format = GL_RGBA;
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
  }

  ~Texture() { glDeleteTextures(1, &id); }

  void bind() { glBindTexture(GL_TEXTURE_2D, id); }
};

#endif