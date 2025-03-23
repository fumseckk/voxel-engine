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
  std::vector<BiomeInfluence> get_biome_influences(int x, int z, const glm::ivec3 &origin) const;
  BiomeType biome_from_noise(float noiseValue) const;
  BiomeType get_dominant_biome(int x, int z, const glm::ivec3 &origin) const;
  float get_river_strength(int x, int z, const glm::ivec3 &origin) const;
  bool is_river(int x, int z, const glm::ivec3 &origin) const;
  float get_biome_height_modifier(const BiomeType &biome, float baseHeight) const;
  int get_height(int x, int z, const glm::ivec3 &origin) const;
  BlockType get_surface_block(const BiomeType &biome, bool is_river, int y) const;
  BlockType get_subsurface_block(const BiomeType &biome, int depth, int y) const;
  void fill_with_terrain(Block *blocks, const glm::ivec3 &origin, int &active_count) const;
  bool should_place_tree(int x, int z, int height, const BiomeType &biome, bool is_river, const glm::ivec3 &origin) const;
  void place_tree(Block *blocks, int x, int y, int z, int &active_count) const;

private:
  static float random()
  {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  }

  static int ivec3_to_index(const glm::ivec3 &p)
  {
    return p.z * CHUNKS_SIZE * WORLD_HEIGHT + p.y * CHUNKS_SIZE + p.x;
  }
};

#endif
