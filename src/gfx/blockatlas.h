#ifndef BLOCKATLAS_H
#define BLOCKATLAS_H

#include <glm/glm.hpp>

struct BlockAtlas {
  static const int BLOCK_PIXEL_LEN = 16;
  static const int BLOCKS_PER_LINE = 16;
  static constexpr float OFFSET = 1.0f / (float)BLOCKS_PER_LINE;
  static std::pair<glm::vec2, glm::vec2> index_to_uv(int index) {
    int x = index % BLOCKS_PER_LINE;
    int y = index / BLOCKS_PER_LINE;
    return std::make_pair(glm::vec2(x * OFFSET, 1.0f - y * OFFSET), glm::vec2((x + 1) * OFFSET, 1.0 - (y + 1) * OFFSET));
  }
};

#endif