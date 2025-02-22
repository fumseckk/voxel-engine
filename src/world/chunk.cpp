#include "chunk.h"
#include "../gfx/gfx.h"
#include <vector>
#include <optional>
#include <map>
#include <set>

#include "../cube.h"

#define CHUNKS_SIZE 16

#define NB_CHUNKS_X 6
#define NB_CHUNKS_Y 1
#define NB_CHUNKS_Z 6

enum BlockType { EMPTY, GRASS, DIRT };

enum Direction { BACKWARD, FORWARD, LEFT, RIGHT, DOWN, UP };
#define FIRST_DIRECTION BACKWARD
#define LAST_DIRECTION UP
glm::uvec3 dir[]{
    glm::uvec3(0, 0, 1), glm::uvec3(0, 0, -1), glm::uvec3(-1, 0, 0),
    glm::uvec3(1, 0, 0), glm::uvec3(0, -1, 0), glm::uvec3(0, 1, 0),
};

struct Face {
 public:
  BlockType type;
  Direction dir;
  glm::uvec3 coords;
  Face(BlockType type, Direction dir, glm::uvec3 coords)
      : type(type), dir(dir), coords(coords) {}
};

class Block {
 public:
  BlockType type;
  Block(BlockType type) : type(type) {}
  void set_active(bool b) {
    if (b)
      type = EMPTY;
    else
      type = (BlockType)1;
  }
  bool is_active() { return type != EMPTY; }
};

class ChunkMesh {
  bool visible;
};

struct ChunkMesh {
  VAO vao;
  VBO vbo, ebo;
  vector<float> buffer;
  vector<Face> faces;
  ChunkMesh()
      : vao(VAO()),
        vbo(VBO(GL_ARRAY_BUFFER, false)),
        ebo(VBO(GL_ELEMENT_ARRAY_BUFFER, false)) {}
  ~ChunkMesh() {}
};

class Chunk {
 public:
  bool dirty; // TODO make use of this
  glm::vec3 origin;
  uint active_count = 0;
  ChunkMesh mesh;
  vector<Block> blocks;
  Chunk(glm::uvec3 origin) : origin(origin) {
    blocks.resize(CHUNKS_SIZE * CHUNKS_SIZE * CHUNKS_SIZE);
  }
  void set_random() {
    for (auto it = blocks.begin(); it != blocks.end(); it++) {
      if (rand() % 2)
        it->set_active(true);
      else
        it->set_active(false);
    }
  }
  ~Chunk() {};
  bool in_range(glm::uvec3 p) {
    return (p.x >= 0 && p.y >= 0 && p.z && p.x < CHUNKS_SIZE &&
            p.y < CHUNKS_SIZE && p.z < CHUNKS_SIZE);
  }
  Block operator[](glm::uvec3 p) {
    assert(in_range(p));
    return blocks[CHUNKS_SIZE * CHUNKS_SIZE * p.x + CHUNKS_SIZE * p.z + p.y];
  }
  void create_faces() {
    mesh.faces.clear();
    if (active_count == 0) return;
    for (uint x = 0; x < CHUNKS_SIZE; x++) {
      for (uint y = 0; y < CHUNKS_SIZE; y++) {
        for (uint z = 0; z < CHUNKS_SIZE; z++) {
          glm::uvec3 p(x, y, z);
          Block block = (*this)[p];
          if ((*this)[p].is_active()) {
            for (uint d = FIRST_DIRECTION; d < LAST_DIRECTION + 1; d++) {
              Block neigh = (*this)[p + d];
              if (!neigh.is_active()) {
                mesh.faces.push_back(Face(block.type, (Direction)d, p));
              }
            }
          }
        }
      }
    }
  }

  void set_face_at_coords(float* dest, Face& face) {
    uint off = face.dir * FACE_SIZE;
    for (int i = 0; i < FACE_SIZE; i += 5) {
      dest[i + 0] = vertices[off + i + 0] + (float)face.coords.x + origin.x;
      dest[i + 1] = vertices[off + i + 1] + (float)face.coords.y + origin.y;
      dest[i + 2] = vertices[off + i + 2] + (float)face.coords.z + origin.z;
      dest[i + 3] = vertices[off + i + 3];
      dest[i + 4] = vertices[off + i + 4];
      dest[i + 5] = (float)face.type;
    }
  }

  void remesh() {
    create_faces();
    if (mesh.faces.size() == 0) return;
    // Copy all face data to buffer, except texture
    size_t sz = mesh.faces.size();
    mesh.buffer.reserve(sz * FACE_SIZE);
    float* ptr = mesh.buffer.data();
    for (Face& face : mesh.faces) {
      set_face_at_coords(ptr, face);
      ptr += FACE_SIZE;
    }
    mesh.vbo.buffer(mesh.buffer.data(), sz * FACE_SIZE);
    mesh.vao.attr(mesh.vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), 0);
    mesh.vao.attr(mesh.vbo, 1, 2, GL_FLOAT, 6 * sizeof(float),
                  3 * sizeof(float));
  }

  void render() {
    if (mesh.faces.size() == 0) return;
    mesh.vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, mesh.faces.size());
  }
};

// TODO: abstract chunk constrol in World class. Then call good functions in
// main.cpp !
// TODO set "origin" for each created chunk
// TODO do not create all chunks, rather make a cache ! do later.

class World {
 public:
  int chunks_size = CHUNKS_SIZE;
  int nb_chunks_x = NB_CHUNKS_X;
  int nb_chunks_y = NB_CHUNKS_Y;
  int nb_chunks_z = NB_CHUNKS_Z;
  map<glm::uvec3, Chunk> chunks;
  vector<Chunk> active_chunks;
  World() {}
  ~World() {}
  // TODO this will be useless for generated world
  bool in_range(int x, int y, int z) {
    bool x_in = (x >= 0 && x <= CHUNKS_SIZE * NB_CHUNKS_X);
    bool y_in = (y >= 0 && y <= CHUNKS_SIZE * NB_CHUNKS_Y);
    bool z_in = (z >= 0 && z <= CHUNKS_SIZE * NB_CHUNKS_Z);
    return x_in && y_in && z_in;
  }
  bool in_range(glm::vec3 p) { return in_range(p.x, p.y, p.z); }
  Chunk retrieve_chunk(glm::uvec3& p) {
    return chunks[p / glm::uvec3(chunks_size)];
  }
  Block operator[](glm::uvec3 p) {
    assert(in_range(p));
    Chunk chunk = retrieve_chunk(p);
    return chunk[p % glm::uvec3(CHUNKS_SIZE, CHUNKS_SIZE, CHUNKS_SIZE)];
  }
  void begin_frame() {
    // TODO
    // Set chunks in render distance as "active"
    // Fill active chunks that didn't yet exist
    // Set chunks that were active but far as inactive
    // For all active chunks :
    for (auto chunk : active_chunks) {
      if (chunk.dirty) {
        chunk.remesh();
      }
    }
  }
  void render() {
    for (auto chunk: active_chunks) {
      // TODO let matrix "m" stay there so that we offset each chunk by its origin?
      chunk.render();
    }
  }
};