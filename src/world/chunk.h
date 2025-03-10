#ifndef CHUNK_H
#define CHUNK_H

#include "../gfx/gfx.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "../params.h"
#include "fastnoiselite.h"
#include "blocks.h"

#include <vector>
#include <optional>
#include <map>
#include <set>
#include <thread>
#include <future>

enum Direction { BACKWARD, FORWARD, LEFT, RIGHT, DOWN, UP, DIRECTION_COUNT };
glm::ivec3 dir[]{
    glm::ivec3(0, 0, 1), glm::ivec3(0, 0, -1), glm::ivec3(-1, 0, 0),
    glm::ivec3(1, 0, 0), glm::ivec3(0, -1, 0), glm::ivec3(0, 1, 0),
};

struct ChunkMesh {
  vector<glm::ivec4> buffer;
  VAO vao;
  SSBO ssbo;
  int faces_count = 0;
  ChunkMesh() : ssbo(SSBO(nullptr, false)) { assert(false); }
  ChunkMesh(Shader* shader) : vao(VAO()), ssbo(SSBO(shader, false)) {}
  ~ChunkMesh() {}
};

class Chunk {
 public:
  bool dirty = true;
  bool meshing = false;
  glm::ivec3 origin;
  int active_count = 0;
  ChunkMesh mesh[6];
  int nb_blocks = CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE;
  Block blocks[CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE];

  Chunk() { assert(false); }
  Chunk(glm::ivec3 origin, Shader* shader)
      : mesh{ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader),
             ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader)} {
    this->origin = origin * glm::ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE);
    dirty = true;
  }

  ~Chunk() {};

  bool player_sees_face(Camera& camera, Direction dir) {
    glm::vec3 pos = camera.position;
    switch (dir) {
      case BACKWARD:
        return pos.z >= origin.z;
      case FORWARD:
        return pos.z <= origin.z + CHUNKS_SIZE;
      case LEFT:
        return pos.x <= origin.x + CHUNKS_SIZE;
      case RIGHT:
        return pos.x >= origin.x;
      case UP:
      case DOWN:
        return true;
      default:
        assert(false);
    }
  }

  glm::ivec3 index_to_ivec3(int index) {
    int x = index % CHUNKS_SIZE;
    int a = (index - x) / CHUNKS_SIZE;
    int y = a % WORLD_HEIGHT;
    int z = (a - y) / WORLD_HEIGHT;
    return glm::ivec3(x, y, z);
  }

  int ivec3_to_index(glm::ivec3 p) {
    return p.z * CHUNKS_SIZE * WORLD_HEIGHT + p.y * CHUNKS_SIZE + p.x;
  }

  bool in_range(glm::ivec3 p) {
    return p.x >= 0 && p.y >= 0 && p.z >= 0 && p.x < CHUNKS_SIZE &&
           p.y < WORLD_HEIGHT && p.z < CHUNKS_SIZE;
  }

  Block operator[](glm::ivec3 p) {
    assert(in_range(p));
    return blocks[ivec3_to_index(p)];
  }

  glm::ivec3 retrieve_chunk_coords(glm::ivec3 p) {
    // division rounded down for consistency in the negatives
    return glm::floor((glm::vec3)p / glm::vec3(CHUNKS_SIZE));
  }
  
  // Check if a neighboring chunk exists
  bool chunk_exists(glm::ivec3 chunk_coords, unordered_map<glm::ivec3, Chunk>& chunks) {
    return chunks.find(chunk_coords) != chunks.end();
  }
  
  // Get a block from world coordinates, even if it's in another chunk
  std::optional<Block> get_world_block(glm::ivec3 world_pos, unordered_map<glm::ivec3, Chunk>& chunks) {
    glm::ivec3 chunk_coords = retrieve_chunk_coords(world_pos);
    
    if (!chunk_exists(chunk_coords, chunks)) {
      return std::nullopt;
    }
    
    // Convert world position to local position within the chunk
    glm::ivec3 local_pos = world_pos - chunk_coords * glm::ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE);
    
    // Make sure the position is in range
    if (local_pos.y < 0 || local_pos.y >= WORLD_HEIGHT) {
      return std::nullopt;
    }
    
    return chunks[chunk_coords][local_pos];
  }

  void create_faces(unordered_map<glm::ivec3, Chunk>& chunks) {
    if (active_count == 0) return;
    for (int d = (Direction)0; d < DIRECTION_COUNT; d++) {
      mesh[d].faces_count = 0;
      mesh[d].buffer.clear();
      for (int x = 0; x < CHUNKS_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
          for (int z = 0; z < CHUNKS_SIZE; z++) {
            glm::ivec3 p(x, y, z);
            Block block = (*this)[p];
            if (block.type != AIR) {
              glm::ivec3 neigh = p + dir[d];
              
              // Handle block within the same chunk
              if (in_range(neigh)) {
                if ((*this)[neigh].type != AIR && (*this)[neigh].type != LEAVES) continue;
              }
              // Handle block in a neighboring chunk
              else {
                // Convert local position to world position
                glm::ivec3 world_pos = origin + neigh;
                auto neighbor_block = get_world_block(world_pos, chunks);
                
                // Skip face creation if there's a block in the neighboring chunk
                if (neighbor_block.has_value() && neighbor_block.value().type != AIR && neighbor_block.value().type != LEAVES) {
                  continue;
                }
                
                // Special case for bottom blocks (don't render bottom face at y=0)
                if (y == 0 && d == (int)DOWN) continue;
              }
              
              mesh[d].faces_count++;
              set_face_at_coords(p, (Direction)d, block.type);
            }
          }
        }
      }
    }
  }

  void set_face_at_coords(glm::vec3 coords, Direction dir, BlockType type) {
    // TODO enlever la direction d'ici et en faire un autre ssbo chunk-wise
    mesh[dir].buffer.push_back(
        glm::ivec4(coords.x, coords.y, coords.z, dir | type << 4));
  }

  void prepare_mesh_data(unordered_map<glm::ivec3, Chunk>& chunks) {
    create_faces(chunks);
  }

  // Called on main thread only
  void upload_to_gpu() {
    if (active_count == 0) return;
    for (int d = 0; d < 6; d++) {
      mesh[d].ssbo.set_buffer(mesh[d].buffer.data(),
                              mesh[d].buffer.size() * sizeof(glm::ivec4), 0);
    }
  }

  void render(Camera& camera) {
    for (int d = 0; d < 6; d++) {
      if (!player_sees_face(camera, (Direction)d)) continue;
      if (mesh[d].faces_count == 0) continue;
      mesh[d].vao.bind();
      mesh[d].ssbo.bind(0);
      glDrawArrays(GL_TRIANGLES, 0, mesh[d].buffer.size() * 6);
    }
  }
};

#endif