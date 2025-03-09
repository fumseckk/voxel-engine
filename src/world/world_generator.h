#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "fastnoiselite.h"
#include "blocks.h"
#include "../params.h"
#include <glm/glm.hpp>

class WorldGenerator {
 public:
  FastNoiseLite noise;

  WorldGenerator() {
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalWeightedStrength(2.0f);
  }

  int get_height(int x, int z, glm::ivec3 origin) {
    float height =
        noise.GetNoise((float)(x + origin.x), (float)(z + origin.z)) / 2.0 +
        0.5f;
    height = powf(height, 2);
    return min(WORLD_HEIGHT - 1, (int)((float)WORLD_HEIGHT * height + 1));
  }

  void fill_with_terrain(Block* blocks, glm::ivec3 origin, int& active_count) {
    for (int x = 0; x < CHUNKS_SIZE; x++) {
      for (int z = 0; z < CHUNKS_SIZE; z++) {
        int scaled_height = get_height(x, z, origin);
        for (int y = 0; y + origin.y < scaled_height && y < WORLD_HEIGHT; y++) {
          blocks[ivec3_to_index(glm::ivec3(x, y, z))].type = DIRT;
          active_count++;
        }
        blocks[ivec3_to_index(glm::ivec3(x, scaled_height - origin.y, z))]
            .type = GRASS;
      }
    }
  }

 private:
  int ivec3_to_index(glm::ivec3 p) {
    return p.z * CHUNKS_SIZE * WORLD_HEIGHT + p.y * CHUNKS_SIZE + p.x;
  }
};

#endif
