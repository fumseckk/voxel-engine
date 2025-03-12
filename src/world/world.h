#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "world_generator.h"
#include <algorithm>

class World
{
public:
  const int render_distance = RENDER_DISTANCE;
  const int chunks_size = CHUNKS_SIZE;
  FastNoiseLite noise;
  unordered_map<glm::ivec3, Chunk> chunks;
  unordered_map<glm::ivec3, Chunk *> loaded_chunks;
  vector<pair<Chunk *, std::future<void>>> active_threads;
  Shader shader = Shader("resources/shaders/default.vert", "resources/shaders/default.frag");
  TextureArray texture_array = TextureArray("resources/textures");
  WorldGenerator generator;

  World();
  ~World();
  void prepare(Camera &camera);
  glm::ivec3 retrieve_chunk_coords(glm::ivec3 p);
  Chunk retrieve_chunk(glm::ivec3 p);
  Block operator[](glm::ivec3 p);
  template <typename T> bool thread_is_done(std::future<T> &t);
  void unload_far_chunks(glm::ivec3 &player_chunk_coords);
  void load_close_chunks(glm::ivec3 &player_chunk_coords);
  void set_view_clear(Camera &camera);
  void add_chunks_to_render_queue();
  void cleanup_meshed_chunks();
  void render_chunks(Camera &camera);
  void render(Camera &camera);
};

#endif