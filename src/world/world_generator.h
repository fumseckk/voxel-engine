#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "fastnoiselite.h"
#include "../params.h"
#include "blocks.h"
#include <glm/glm.hpp>
#include <vector>

enum BiomeType
{
  PLAINS,
  FOREST,
  DESERT,
  MOUNTAINS,
  TUNDRA,
  BIOME_COUNT
};

struct BiomeInfluence
{
  BiomeType type;
  float weight;
};

class WorldGenerator
{
public:
  FastNoiseLite heightNoise;
  FastNoiseLite biomeNoise;
  FastNoiseLite riverNoise;
  FastNoiseLite detailNoise;
  FastNoiseLite treeNoise;
  int waterLevel;

  WorldGenerator();
  ~WorldGenerator() {}
  std::vector<BiomeInfluence> get_biome_influences(int x, int z, glm::ivec3 origin);
  BiomeType biome_from_noise(float noiseValue);
  BiomeType get_dominant_biome(int x, int z, glm::ivec3 origin);
  float get_river_strength(int x, int z, glm::ivec3 origin);
  bool is_river(int x, int z, glm::ivec3 origin);
  float get_biome_height_modifier(BiomeType biome, float baseHeight);
  int get_height(int x, int z, glm::ivec3 origin);
  BlockType get_surface_block(BiomeType biome, bool is_river, int y);
  BlockType get_subsurface_block(BiomeType biome, int depth, int y);
  void fill_with_terrain(Block *blocks, glm::ivec3 origin, int &active_count);
  bool should_place_tree(int x, int z, int height, BiomeType biome, bool is_river, glm::ivec3 origin);
  void place_tree(Block *blocks, int x, int y, int z, int &active_count);

private:
  static float random()
  {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  }

  static int ivec3_to_index(glm::ivec3 p)
  {
    return p.z * CHUNKS_SIZE * WORLD_HEIGHT + p.y * CHUNKS_SIZE + p.x;
  }
};

#endif
