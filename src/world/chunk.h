#ifndef CHUNK_H
#define CHUNK_H

#include "../gfx/gfx.h"
#include <vector>
#include <optional>
#include <map>
#include <set>

#include "../cube.h"


#define CHUNKS_SIZE 4

#define NB_CHUNKS_X 5
#define NB_CHUNKS_Y 2
#define NB_CHUNKS_Z 5

#define IN_RANGE(_p, _size_x, _size_y, _size_z)                               \
  (_p.z >= 0 && _p.y >= 0 && _p.z >= 0 && _p.x < _size_x && _p.y < _size_y && \
   _p.z < _size_z)

#define IN_RANGE_EQUI(p, size) IN_RANGE(p, size, size, size)

#define UVEC3_TO_COORDS(p, _size_x, _size_y)           \
  ({                                                   \
    assert(IN_RANGE(p, _size_x, _size_y, UINT_MAX));   \
    (p.z * _size_x * _size_y) + (p.y * _size_x) + p.x; \
  })

#define COORDS_TO_UVEC3(_coords, _size_x, _size_y) \
  ({                                               \
    uint _x = _coords % _size_x;                   \
    uint _a = (_coords - _x) / _size_x;            \
    uint _y = _a % _size_y;                        \
    uint _z = (_a - _y) / _size_y;                 \
    glm::uvec3(_x, _y, _z);                        \
  })

enum BlockType { EMPTY, GRASS, DIRT };

enum Direction { BACKWARD, FORWARD, LEFT, RIGHT, DOWN, UP };
#define FIRST_DIRECTION BACKWARD
#define LAST_DIRECTION UP
glm::uvec3 dir[]{
    glm::uvec3(0, 0, -1), glm::uvec3(0, 0, 1),  glm::uvec3(-1, 0, 0),
    glm::uvec3(1, 0, 0),  glm::uvec3(0, -1, 0), glm::uvec3(0, 1, 0),
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
  Block() : type(EMPTY) {}
  Block(BlockType type) : type(type) {}
  void set_active(bool b) {
    if (b)
      type = GRASS;
    else
      type = EMPTY;
  }
  bool is_active() { return type != EMPTY; }
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
  bool dirty = true;  // TODO make use of this
  glm::uvec3 origin;
  uint active_count = 0;
  ChunkMesh mesh;
  vector<Block> blocks;
  Chunk() { assert(false); }
  Chunk(glm::uvec3 origin) {
    this->origin = origin * glm::uvec3(CHUNKS_SIZE);
    blocks.resize(CHUNKS_SIZE * CHUNKS_SIZE * CHUNKS_SIZE);
    set_random();
  }
  void set_random() {
    active_count = 0;
    for (auto it = blocks.begin(); it != blocks.end(); it++) {
      if (std::rand() % 2) {
        it->set_active(true);
        active_count++;
      } else {
        it->set_active(false);
      }
    }
  }
  ~Chunk() {};

  bool in_range(glm::uvec3& p) { return IN_RANGE_EQUI(p, CHUNKS_SIZE); }

  Block operator[](glm::uvec3 p) {
    assert(in_range(p));
    return blocks[UVEC3_TO_COORDS(p, CHUNKS_SIZE, CHUNKS_SIZE)];
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
              glm::uvec3 neigh = p + dir[d];
              // TODO do not draw if neighbour chunk has active block
              if (in_range(neigh) && (*this)[neigh].is_active()) continue;
              mesh.faces.push_back(Face(block.type, (Direction)d, p));
            }
          }
        }
      }
    }
  }

  void set_face_at_coords(float* dest, Face& face) {
    uint off = face.dir * FACE_SIZE;
    printf("FACE: %d | %d %d %d\n", face.dir, face.coords.x, face.coords.y,
           face.coords.z);
    for (int i = 0; i < FACE_SIZE; i += 5) {
      dest[i + 0] = vertices[off + i + 0] + (float)(face.coords.x + origin.x);
      dest[i + 1] = vertices[off + i + 1] + (float)(face.coords.y + origin.y);
      dest[i + 2] = vertices[off + i + 2] + (float)(face.coords.z + origin.z);
      dest[i + 3] = vertices[off + i + 3];
      dest[i + 4] = vertices[off + i + 4];
    }
  }

  void remesh() {
    create_faces();
    if (mesh.faces.size() == 0) return;
    mesh.buffer.resize(mesh.faces.size() * FACE_SIZE);
    float* ptr = mesh.buffer.data();
    for (Face& face : mesh.faces) {
      set_face_at_coords(ptr, face);
      ptr += FACE_SIZE;
      // assert(abs(ptr - mesh.buffer.data()) < mesh.buffer.size());
    }
  }

  void render(Camera& camera) {
    if (mesh.faces.size() == 0) return;
    mesh.vbo.buffer(mesh.buffer.data(), mesh.buffer.size() * sizeof(float));
    mesh.vao.attr(mesh.vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
    mesh.vao.attr(mesh.vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),
                  3 * sizeof(float));
    glDrawArrays(GL_TRIANGLES, 0, mesh.buffer.size());
  }
};

// TODO: abstract chunk constrol in World class. Then call good functions in
// main.cpp !
// TODO do not create all chunks, rather make a cache ! do later.

class World {
 public:
  const uint chunks_size = CHUNKS_SIZE;
  const uint nb_chunks_x = NB_CHUNKS_X;
  const uint nb_chunks_y = NB_CHUNKS_Y;
  const uint nb_chunks_z = NB_CHUNKS_Z;
  map<uint, Chunk> chunks;
  Shader shader = Shader("resources/shaders/default.vert",
                         "resources/shaders/default.frag");
  Texture texture = Texture("resources/textures/wooden_container.jpg");
  // vector<Chunk&> active_chunks;
  World() {}
  ~World() {}
  // TODO this will be useless for generated world

  bool in_range(glm::uvec3& p) {
    return IN_RANGE(p, chunks_size * nb_chunks_x, chunks_size * nb_chunks_y,
                    chunks_size * nb_chunks_z);
  }

  Chunk retrieve_chunk(glm::uvec3& p) {
    glm::uvec3 pc = p / glm::uvec3(chunks_size);
    assert(IN_RANGE(pc, nb_chunks_x, nb_chunks_y, nb_chunks_z));
    return chunks[UVEC3_TO_COORDS(p, nb_chunks_x, nb_chunks_y)];
  }

  Block operator[](glm::uvec3 p) {
    assert(in_range(p));
    Chunk chunk = retrieve_chunk(p);
    return chunk[p % glm::uvec3(chunks_size)];
  }

  void render(Camera& camera) {
    // TODO change this to have loaded, unloaded chunks.
    for (uint x = 0; x < nb_chunks_x; x++) {
      for (uint y = 0; y < nb_chunks_y; y++) {
        for (uint z = 0; z < nb_chunks_z; z++) {
          uint idx =
              UVEC3_TO_COORDS(glm::uvec3(x, y, z), nb_chunks_x, nb_chunks_y);
          if (chunks.find(idx) == chunks.end()) {
            chunks.emplace(std::make_pair(idx, Chunk(glm::uvec3(x, y, z))));
          }
        }
      }
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
    glm::mat4 p = camera.get_perspective_matrix();
    glm::mat4 v = camera.get_view_matrix();
    shader.uniform_mat4("p", p);
    shader.uniform_mat4("v", v);
    for (auto& [_, chunk] : chunks) {
      if (chunk.dirty) {
        chunk.remesh();
        chunk.dirty = false;
      }
      chunk.render(camera);
    }
  }
};

#endif