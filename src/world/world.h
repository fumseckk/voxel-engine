#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "world_generator.h"
#include <algorithm>

// TODO move
struct Frustum
{
  glm::vec4 planes[6];
  Frustum() = default;
  ~Frustum() {};
  explicit Frustum(const glm::mat4 &pv)
  {
    // Left plane
    planes[0] = glm::vec4(pv[0][3] + pv[0][0], pv[1][3] + pv[1][0],
                          pv[2][3] + pv[2][0], pv[3][3] + pv[3][0]);
    // Right plane
    planes[1] = glm::vec4(pv[0][3] - pv[0][0], pv[1][3] - pv[1][0],
                          pv[2][3] - pv[2][0], pv[3][3] - pv[3][0]);
    // Bottom plane
    planes[2] = glm::vec4(pv[0][3] + pv[0][1], pv[1][3] + pv[1][1],
                          pv[2][3] + pv[2][1], pv[3][3] + pv[3][1]);
    // Top plane
    planes[3] = glm::vec4(pv[0][3] - pv[0][1], pv[1][3] - pv[1][1],
                          pv[2][3] - pv[2][1], pv[3][3] - pv[3][1]);
    // Near plane
    planes[4] = glm::vec4(pv[0][3] - pv[0][2], pv[1][3] - pv[1][2],
                          pv[2][3] - pv[2][2], pv[3][3] - pv[3][2]);
    // Far plane
    planes[5] = glm::vec4(pv[0][3] + pv[0][2], pv[1][3] + pv[1][2],
                          pv[2][3] + pv[2][2], pv[3][3] + pv[3][2]);
    for (int i = 0; i < 6; i++)
    {
      float length = glm::length(glm::vec3(planes[i]));
      planes[i] /= length;
    }
  }
};

class World
{
public:
  const int render_distance = RENDER_DISTANCE;
  const int chunks_size = CHUNKS_SIZE;
  unordered_map<glm::ivec3, Chunk> chunks;
  vector<pair<glm::ivec3, float>> visible_chunks;
  vector<pair<Chunk *, std::future<void>>> active_threads;
  Shader shader = Shader("resources/shaders/default.vert", "resources/shaders/default.frag");
  TextureArray texture_array = TextureArray("resources/textures");
  WorldGenerator generator;

  World();
  ~World();
  void prepare(const Camera &camera);
  glm::ivec3 retrieve_chunk_coords(const glm::ivec3 &p);
  Chunk retrieve_chunk(const glm::ivec3 &p);
  Block operator[](const glm::ivec3 &p);
  template <typename T>
  bool thread_is_done(const std::future<T> &t);
  void unload_far_chunks(const glm::ivec3 &player_chunk_coords);
  bool inside_frustum(const Frustum &frustum, const glm::ivec3 &coords);
  void load_close_chunks(const Frustum &frustum, const glm::ivec3 &player_chunk_coords);
  void set_view_clear();
  void add_chunks_to_render_queue();
  void cleanup_meshed_chunks();
  void render_chunks(const Frustum &frustum, const glm::ivec3 &player_chunk_coords, const Camera &camera);
  void render(const Camera &camera);
};

#endif