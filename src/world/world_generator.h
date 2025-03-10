#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include "fastnoiselite.h"
#include "blocks.h"
#include "../params.h"
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

enum BiomeType {
  PLAINS,
  FOREST,
  DESERT,
  MOUNTAINS,
  TUNDRA,
  BIOME_COUNT
};

struct BiomeInfluence {
  BiomeType type;
  float weight;
};

class WorldGenerator {
 public:
  FastNoiseLite heightNoise;
  FastNoiseLite biomeNoise;
  FastNoiseLite riverNoise;
  FastNoiseLite detailNoise;
  
  int waterLevel;

  WorldGenerator() {
    // Setup height noise
    heightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    heightNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    heightNoise.SetFractalWeightedStrength(2.0f);
    
    // Setup biome noise (larger scale)
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    biomeNoise.SetFrequency(0.01f);
    
    // Setup river noise
    riverNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    riverNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    riverNoise.SetFractalOctaves(4);
    riverNoise.SetFrequency(0.008f);
    
    // Detail noise for small terrain variations
    detailNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    detailNoise.SetFrequency(0.05f);
    
    // Set water level (about 40% up from the bottom)
    waterLevel = static_cast<int>(WORLD_HEIGHT * 0.4f);
  }

  std::vector<BiomeInfluence> get_biome_influences(int x, int z, glm::ivec3 origin) {
    // Sample biome noise at multiple nearby points to detect transitions
    std::vector<BiomeInfluence> influences;
    const float BIOME_BLEND_RADIUS = 8.0f; // Blend radius in blocks
    
    // Get central biome
    float centralValue = biomeNoise.GetNoise((float)(x + origin.x), (float)(z + origin.z));
    BiomeType centralBiome = biome_from_noise(centralValue);
    influences.push_back({centralBiome, 1.0f});
    
    // Sample points in a circle to detect nearby biomes
    for (int i = 0; i < 8; i++) {
      float angle = i * (2.0f * 3.14159f / 8.0f);
      float sampleX = x + cosf(angle) * BIOME_BLEND_RADIUS;
      float sampleZ = z + sinf(angle) * BIOME_BLEND_RADIUS;
      
      float biomeValue = biomeNoise.GetNoise((float)(sampleX + origin.x), (float)(sampleZ + origin.z));
      BiomeType biomeSample = biome_from_noise(biomeValue);
      
      // If this is a different biome than center, add it with reduced weight
      if (biomeSample != centralBiome) {
        // Calculate distance-based weight
        float weight = 0.5f; // Lower influence from neighbors
        
        // Try to find if this biome is already in our list
        bool found = false;
        for (auto& influence : influences) {
          if (influence.type == biomeSample) {
            influence.weight += weight;
            found = true;
            break;
          }
        }
        
        // If not found, add it
        if (!found) {
          influences.push_back({biomeSample, weight});
        }
      }
    }
    
    // Normalize weights
    float totalWeight = 0.0f;
    for (const auto& influence : influences) {
      totalWeight += influence.weight;
    }
    
    for (auto& influence : influences) {
      influence.weight /= totalWeight;
    }
    
    return influences;
  }
  
  BiomeType biome_from_noise(float noiseValue) {
    // Custom distribution to favor grass biomes and reduce desert coverage
    // Map the noise value (-1 to 1) to a biome
    float mappedValue = (noiseValue + 1.0f) * 0.5f;
    
    // Adjusted biome distribution
    if (mappedValue < 0.30f) return PLAINS;      // 30% chance
    if (mappedValue < 0.60f) return FOREST;      // 30% chance
    if (mappedValue < 0.70f) return DESERT;      // 10% chance
    if (mappedValue < 0.85f) return MOUNTAINS;   // 15% chance
    return TUNDRA;                               // 15% chance
  }

  BiomeType get_dominant_biome(int x, int z, glm::ivec3 origin) {
    float biomeValue = biomeNoise.GetNoise((float)(x + origin.x), (float)(z + origin.z));
    return biome_from_noise(biomeValue);
  }
  
  // Check if a position is in a river and returns river strength (0 = not in river, 1 = center of river)
  float get_river_strength(int x, int z, glm::ivec3 origin) {
    // Get biome at this location - rivers shouldn't appear in mountains
    BiomeType biome = get_dominant_biome(x, z, origin);
    if (biome == MOUNTAINS) {
      return 0.0f; // No rivers in mountains
    }

    float riverValue = riverNoise.GetNoise((float)(x + origin.x), (float)(z + origin.z));
    float riverStrength = 1.0f - (std::abs(riverValue) / 0.05f);
    return std::max(0.0f, riverStrength);
  }

  bool is_river(int x, int z, glm::ivec3 origin) {
    return get_river_strength(x, z, origin) > 0.0f;
  }
  
  float get_biome_height_modifier(BiomeType biome, float baseHeight) {
    switch (biome) {
      case MOUNTAINS:
        return powf(baseHeight, 2.0f) * 0.9f;
      case PLAINS:
        return powf(baseHeight, 0.8f) * 0.7f; // Flatter terrain
      case DESERT:
        return powf(baseHeight, 0.9f) * 0.6f; // Low dunes
      case FOREST:
        return powf(baseHeight, 1.1f) * 0.9f; // Slightly hilly
      case TUNDRA:
        return powf(baseHeight, 1.0f) * 0.8f; // Medium height
      default:
        return baseHeight;
    }
  }

  int get_height(int x, int z, glm::ivec3 origin) {
    // Get base height
    float baseHeight = heightNoise.GetNoise((float)(x + origin.x), (float)(z + origin.z)) / 2.0f + 0.5f;
    baseHeight += (detailNoise.GetNoise((float)(x + origin.x), (float)(z + origin.z)) / 2.0f + 0.5f) * 0.01f;
    // Biome influences to create smooth transitions
    std::vector<BiomeInfluence> biomeInfluences = get_biome_influences(x, z, origin);
    
    float blendedHeight = 0.0f;
    for (const auto& influence : biomeInfluences) {
      float biomeHeight = get_biome_height_modifier(influence.type, baseHeight);
      blendedHeight += biomeHeight * influence.weight;
    }
    
    int naturalTerrainHeight = min(WORLD_HEIGHT - 1, (int)((float)WORLD_HEIGHT * blendedHeight + 1));
    
    float riverStrength = get_river_strength(x, z, origin);
    if (riverStrength > 0.0f) {
        float riverDepth = 0.1f + blendedHeight * 0.1f; // Make deeper rivers in higher terrain
        float riverDepthAtPoint = riverDepth * riverStrength;
        float minRiverBottom = (float)waterLevel / WORLD_HEIGHT - 0.05f;
        blendedHeight = std::max(minRiverBottom, blendedHeight - riverDepthAtPoint);
        int terrainHeight = min(WORLD_HEIGHT - 1, (int)((float)WORLD_HEIGHT * blendedHeight + 1));
        return std::min(terrainHeight, naturalTerrainHeight);
    }

    return naturalTerrainHeight;
  }

  BlockType get_surface_block(BiomeType biome, bool is_river, int y) {
    if (is_river) {
      return STONE; // River beds are stone/gravel
    }
    
    switch (biome) {
      case DESERT:
        return SAND;
      case TUNDRA:
        return SNOW;
      case MOUNTAINS:
        return y > WORLD_HEIGHT * 0.7f ? SNOW : STONE;
      case FOREST:
      case PLAINS:
      default:
        return GRASS;
    }
  }

  BlockType get_subsurface_block(BiomeType biome, int depth, int y) {
    // First few blocks below surface
    if (depth <= 3) {
      switch (biome) {
        case DESERT:
          return SAND;
        case TUNDRA:
        case MOUNTAINS:
        case FOREST:
        case PLAINS:
        default:
          return DIRT;
      }
    }
    // Deeper blocks
    return STONE;
  }

  void fill_with_terrain(Block* blocks, glm::ivec3 origin, int& active_count) {
    for (int x = 0; x < CHUNKS_SIZE; x++) {
      for (int z = 0; z < CHUNKS_SIZE; z++) {
        BiomeType biome = get_dominant_biome(x, z, origin);
        float riverStrength = get_river_strength(x, z, origin);
        bool is_river_block = riverStrength > 0.0f;
        int scaled_height = get_height(x, z, origin);
        
        // Fill terrain blocks
        for (int y = 0; y + origin.y < scaled_height && y < WORLD_HEIGHT; y++) {
          int depth = scaled_height - (y + origin.y);
          BlockType blockType;
          
          // River beds get special treatment
          if (is_river_block && y + origin.y >= scaled_height - 1) {
            blockType = DIRT; // River beds are dirt
          } else {
            blockType = get_subsurface_block(biome, depth, y + origin.y);
          }
          
          blocks[ivec3_to_index(glm::ivec3(x, y, z))].type = blockType;
          active_count++;
        }
        
        // Set surface blocks
        if (scaled_height - origin.y >= 0 && scaled_height - origin.y < WORLD_HEIGHT) {
          BlockType surfaceType = get_surface_block(biome, is_river_block, scaled_height);
          blocks[ivec3_to_index(glm::ivec3(x, scaled_height - origin.y, z))].type = surfaceType;
          active_count++;
        }
        
        // Add water for rivers only if it's below the water level and at or below the terrain height
        if (is_river_block) {
          int water_top = std::min(waterLevel, scaled_height + 1);
          for (int y = scaled_height + 1 - origin.y; y <= water_top - origin.y; y++) {
            if (y >= 0 && y < WORLD_HEIGHT) {
              blocks[ivec3_to_index(glm::ivec3(x, y, z))].type = WATER;
              active_count++;
            }
          }
        }
      }
    }
  }

 private:
  int ivec3_to_index(glm::ivec3 p) {
    return p.z * CHUNKS_SIZE * WORLD_HEIGHT + p.y * CHUNKS_SIZE + p.x;
  }
};

#endif
