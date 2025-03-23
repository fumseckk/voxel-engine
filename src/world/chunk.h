#ifndef CHUNK_H
#define CHUNK_H

#include "../gfx/gfx.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "../params.h"
#include "blocks.h"
#include "world_generator.h"

#include <vector>
#include <optional>
#include <map>
#include <set>
#include <thread>
#include <future>

enum Direction
{
  BACKWARD,
  FORWARD,
  LEFT,
  RIGHT,
  DOWN,
  UP,
  DIRECTION_COUNT
};

struct ChunkMesh
{
  vector<glm::ivec4> buffer;
  VAO vao;
  SSBO ssbo;
  int faces_count = 0;
  ChunkMesh() : ssbo(SSBO(nullptr, false)) { assert(false); }
  ChunkMesh(Shader *shader) : vao(VAO()), ssbo(SSBO(shader, false)) {}
  ~ChunkMesh() {}
};

class Chunk
{
public:
  bool dirty = true;
  bool meshing = false;
  glm::ivec3 origin;
  int active_count = 0;
  ChunkMesh mesh[6];
  int nb_blocks = CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE;
  Block blocks[CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE];

  Chunk() { assert(false); }
  Chunk(glm::ivec3 origin, Shader *shader)
      : mesh{ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader),
             ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader)}
  {
    this->origin = origin * glm::ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE);
    dirty = true;
  }
  ~Chunk() {};

  Block operator[](const glm::ivec3 &p);
  bool player_sees_face(const Camera &camera, const Direction &dir);
  glm::ivec3 retrieve_chunk_coords(const glm::ivec3 &p);
  // Check if a neighboring chunk exists
  bool chunk_exists(const glm::ivec3 &chunk_coords, const unordered_map<glm::ivec3, Chunk> &chunks);
  // Get a block from world coordinates, even if it's in another chunk
  std::optional<Block> get_world_block(const glm::ivec3 &world_pos, unordered_map<glm::ivec3, Chunk> &chunks);
  void prepare_mesh_data(const WorldGenerator &generator, const unordered_map<glm::ivec3, Chunk> &chunks);
  void set_face_at_coords(const glm::vec3& coords, const Direction& dir, const BlockType& type);
  void upload_to_gpu();
  void render(const Camera &camera);

private:
  static glm::ivec3 index_to_ivec3(const int index)
  {
    int x = index % CHUNKS_SIZE;
    int a = (index - x) / CHUNKS_SIZE;
    int y = a % WORLD_HEIGHT;
    int z = (a - y) / WORLD_HEIGHT;
    return glm::ivec3(x, y, z);
  }

  static int ivec3_to_index(const glm::ivec3& p)
  {
    return p.z * CHUNKS_SIZE * WORLD_HEIGHT + p.y * CHUNKS_SIZE + p.x;
  }

  static bool in_range(const glm::ivec3& p)
  {
    return p.x >= 0 && p.y >= 0 && p.z >= 0 && p.x < CHUNKS_SIZE &&
           p.y < WORLD_HEIGHT && p.z < CHUNKS_SIZE;
  }
};

#endif