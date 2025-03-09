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

enum Direction { BACKWARD, FORWARD, LEFT, RIGHT, DOWN, UP };
#define FIRST_DIRECTION BACKWARD
#define LAST_DIRECTION UP
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
  FastNoiseLite* noise;
  bool dirty = true;
  bool meshing = false;
  glm::ivec3 origin;
  int active_count = 0;
  ChunkMesh mesh[6];
  int nb_blocks = CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE;
  Block blocks[CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE];

  Chunk() { assert(false); }
  Chunk(glm::ivec3 origin, Shader* shader, FastNoiseLite* noise)
      : noise(noise),
        mesh{ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader),
             ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader)} {
    this->origin = origin * glm::ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE);
    fill_with_terrain();
    dirty = true;
  }

  ~Chunk() {};

  int get_height(int x, int z) {
    float height =
        noise->GetNoise((float)(x + origin.x), (float)(z + origin.z)) / 2.0 +
        0.5f;
    height = powf(height, 2);
    return min(WORLD_HEIGHT - 1, (int)((float)WORLD_HEIGHT * height + 1));
  }

  void fill_with_terrain() {
    for (int x = 0; x < CHUNKS_SIZE; x++) {
      for (int z = 0; z < CHUNKS_SIZE; z++) {
        int scaled_height = get_height(x, z);
        for (int y = 0; y + origin.y < scaled_height && y < WORLD_HEIGHT; y++) {
          blocks[ivec3_to_index(glm::ivec3(x, y, z))].type = DIRT;
          active_count++;
        }
        blocks[ivec3_to_index(glm::ivec3(x, scaled_height - origin.y, z))]
            .type = GRASS;
      }
    }
  }

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

  void set_random() {
    active_count = 0;
    for (int i = 0; i < nb_blocks; i++) {
      if (std::rand() % 2) {
        blocks[i].set_active(true);
        active_count++;
      } else {
        blocks[i].set_active(false);
      }
    }
  }

  void set_floor() {
    active_count = 0;
    if (origin.y != 0) return;
    for (int x = 0; x < CHUNKS_SIZE; x++) {
      for (int z = 0; z < CHUNKS_SIZE; z++) {
        blocks[ivec3_to_index(glm::ivec3(x, 0, z))].set_active(true);
        active_count++;
      }
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

  void create_faces(unordered_map<glm::ivec3, Chunk>& chunks) {
    if (active_count == 0) return;
    for (int d = FIRST_DIRECTION; d < LAST_DIRECTION + 1; d++) {
      mesh[d].faces_count = 0;
      mesh[d].buffer.clear();
      for (int x = 0; x < CHUNKS_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
          for (int z = 0; z < CHUNKS_SIZE; z++) {
            glm::ivec3 p(x, y, z);
            Block block = (*this)[p];
            if ((*this)[p].is_active()) {
              glm::ivec3 neigh = p + dir[d];
              if (in_range(neigh) && (*this)[neigh].is_active()) continue;
              if (y == 0 && d == (int)DOWN) continue;
              // FIXME this will break someday
              if (!in_range(neigh) && get_height(neigh.x, neigh.z) > y)
                continue;
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

  glm::ivec3 retrieve_chunk_coords(glm::ivec3 p) {
    // division rounded down for consistency in the negatives
    return glm::floor((glm::vec3)p / glm::vec3(CHUNKS_SIZE));
  }
  Chunk retrieve_chunk(glm::ivec3 p, unordered_map<glm::ivec3, Chunk>& chunks) {
    return chunks[retrieve_chunk_coords(p)];
  }
  Block get_world(glm::ivec3 p, unordered_map<glm::ivec3, Chunk>& chunks) {
    Chunk chunk = retrieve_chunk(p, chunks);
    return chunk[p & glm::ivec3(CHUNKS_SIZE - 1)];
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