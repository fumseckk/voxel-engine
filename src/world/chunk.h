#ifndef CHUNK_H
#define CHUNK_H

#include "../gfx/gfx.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "fastnoiselite.h"
#include <vector>
#include <optional>
#include <map>
#include <set>

#include "../cube.h"

#define CHUNKS_SIZE 16
#define WORLD_HEIGHT 50
#define RENDER_DISTANCE 15

GLenum glCheckError_(const char* file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (errorCode) {
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }
  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

enum BlockType { EMPTY, GRASS, DIRT };

enum Direction { BACKWARD, FORWARD, LEFT, RIGHT, DOWN, UP };
#define FIRST_DIRECTION BACKWARD
#define LAST_DIRECTION UP
glm::ivec3 dir[]{
    glm::ivec3(0, 0, -1), glm::ivec3(0, 0, 1),  glm::ivec3(-1, 0, 0),
    glm::ivec3(1, 0, 0),  glm::ivec3(0, -1, 0), glm::ivec3(0, 1, 0),
};

struct Face {
 public:
  BlockType type;
  Direction dir;
  glm::ivec3 coords;
  Face(BlockType type, Direction dir, glm::ivec3 coords)
      : type(type), dir(dir), coords(coords) {}
  int get_blockatlas_index() {
    switch (type) {
      case EMPTY:
        assert(false);
        break;
      case DIRT:
        return 2;
      case GRASS:
        switch (dir) {
          case UP:
            return 0;
          case DOWN:
            return 1;
          default:
            return 2;
        }
      default:
        assert(false);
    }
  }
  // array<float, 12> get_uv() {
  //   auto [topleft, botright] =
  //   BlockAtlas::index_to_uv(get_blockatlas_index()); return array<float,
  //   12>({
  //     topleft.x,
  //   });
  // }
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
  VBO vbo;
  vector<float> buffer;
  vector<Face> faces;
  ChunkMesh() : vao(VAO()), vbo(VBO(GL_ARRAY_BUFFER, false)) {}
  ~ChunkMesh() {}
};

class Chunk {
 public:
  FastNoiseLite noise;
  bool dirty = true;
  glm::ivec3 origin;
  int active_count = 0;
  int highest_block = 0;
  ChunkMesh mesh;
  int nb_blocks = CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE;
  Block blocks[CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE];

  Chunk() { assert(false); }
  Chunk(glm::ivec3 origin) {
    this->origin = origin * glm::ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE);
    // set_random();
    set_blocks();
    mesh.vao.attr(mesh.vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
    mesh.vao.attr(mesh.vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),
                  3 * sizeof(float));
    dirty = true;
  }

  ~Chunk() {};

  int get_height(int x, int z) {
    float height =
      noise.GetNoise((float)(x + origin.x), (float)(z + origin.z)) / 2.0 +
      0.5f;
    return (int)((float)WORLD_HEIGHT * height);
  } 
  void set_blocks() {
    float mx = 0.0f, mn = 10.0F;
    for (int x = 0; x < CHUNKS_SIZE; x++) {
      for (int z = 0; z < CHUNKS_SIZE; z++) {
        int scaled_height = get_height(x, z);
        for (int y = 0; y + origin.y < scaled_height && y < WORLD_HEIGHT; y++) {
          blocks[ivec3_to_index(glm::ivec3(x, y, z))].set_active(true);
          active_count++;
        }
        highest_block = max(highest_block, scaled_height);
      }
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
    mesh.faces.clear();
    if (active_count == 0) return;
    for (int x = 0; x < CHUNKS_SIZE; x++) {
      for (int y = 0; y < highest_block; y++) {
        for (int z = 0; z < CHUNKS_SIZE; z++) {
          glm::ivec3 p(x, y, z);
          Block block = (*this)[p];
          if ((*this)[p].is_active()) {
            for (int d = FIRST_DIRECTION; d < LAST_DIRECTION + 1; d++) {
              glm::ivec3 neigh = p + dir[d];
              if (in_range(neigh) && (*this)[neigh].is_active()) continue;
              // FIXME this will break someday
              if (!in_range(neigh) && get_height(neigh.x, neigh.z) > y) continue;
              mesh.faces.push_back(Face(block.type, (Direction)d, p));
            }
          }
        }
      }
    }
    assert(mesh.faces.size());
  }

  void set_face_at_coords(float* dest, Face& face) {
    int off = face.dir * FACE_SIZE;
    for (int i = 0; i < FACE_SIZE; i += 5) {
      dest[i + 0] = vertices[off + i + 0] + (float)(face.coords.x + origin.x);
      dest[i + 1] = vertices[off + i + 1] + (float)(face.coords.y + origin.y);
      dest[i + 2] = vertices[off + i + 2] + (float)(face.coords.z + origin.z);
      dest[i + 3] = vertices[off + i + 3];
      dest[i + 4] = vertices[off + i + 4];
    }
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

  void remesh(unordered_map<glm::ivec3, Chunk>& chunks) {
    create_faces(chunks);
    if (mesh.faces.size() == 0) return;
    mesh.buffer.resize(mesh.faces.size() * FACE_SIZE);
    float* ptr = mesh.buffer.data();
    for (Face& face : mesh.faces) {
      set_face_at_coords(ptr, face);
      ptr += FACE_SIZE;
    }
    mesh.vbo.buffer(mesh.buffer.data(), mesh.buffer.size() * sizeof(float));
  }

  void render(Camera& camera) {
    if (mesh.faces.size() == 0) return;
    // glCheckError();
    mesh.vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, mesh.buffer.size() / VERTEX_SIZE);
  }
};

class World {
 public:
  const int render_distance = RENDER_DISTANCE;
  const int chunks_size = CHUNKS_SIZE;
  unordered_map<glm::ivec3, Chunk> chunks;
  unordered_map<glm::ivec3, Chunk*> loaded_chunks;
  Shader shader =
      Shader("resources/shaders/default.vert", "resources/shaders/default.frag",
             "resources/shaders/default.geom");
  Texture texture = Texture("resources/textures/atlas.png");
  World() {}
  ~World() {}

  void prepare(Camera& camera) {
    shader.use();
    glm::mat4 p = camera.get_perspective_matrix();
    shader.uniform_mat4("p", p);
  }

  glm::ivec3 retrieve_chunk_coords(glm::ivec3 p) {
    // division rounded down for consistency in the negatives
    return glm::floor((glm::vec3)p / glm::vec3(chunks_size));
  }
  Chunk retrieve_chunk(glm::ivec3 p) {
    return chunks[retrieve_chunk_coords(p)];
  }

  Block operator[](glm::ivec3 p) {
    Chunk chunk = retrieve_chunk(p);
    return chunk[p & glm::ivec3(chunks_size - 1)];
  }

  void render(Camera& camera) {
    glm::ivec3 player_coords = glm::ivec3(camera.position);
    glm::ivec3 player_chunk_coords = retrieve_chunk_coords(player_coords);

    // Unload chunks that are too far
    for (auto it = loaded_chunks.cbegin(); it != loaded_chunks.cend();) {
      Chunk* chunk = it->second;
      glm::ivec3 orig = chunk->origin - player_chunk_coords;
      if (orig.x * orig.x + orig.y * orig.y + orig.z * orig.z >
          render_distance * render_distance)
        loaded_chunks.erase(it++);
      else
        it++;
    }

    // Load chunks that became close, create them if needed
    for (int x = -render_distance; x < render_distance; x++) {
      for (int z = -render_distance; z < render_distance; z++) {
        if (x * x + z * z > render_distance * render_distance) continue;
        glm::ivec3 chunk_coords =
            glm::vec3(player_chunk_coords.x + x, 0, player_chunk_coords.z + z);
        if (chunks.find(chunk_coords) == chunks.end()) {
          chunks.emplace(std::piecewise_construct,
                         std::forward_as_tuple(chunk_coords),
                         std::forward_as_tuple(chunk_coords));
          loaded_chunks.emplace(chunk_coords, &chunks[chunk_coords]);
        } else if (loaded_chunks.find(chunk_coords) == loaded_chunks.end()) {
          loaded_chunks.emplace(chunk_coords, &chunks[chunk_coords]);
        }
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.63f, 0.86f, 1.0f, 1.0f);
    glm::mat4 v = camera.get_view_matrix();
    shader.use();
    shader.uniform_mat4("v", v);
    shader.uniform_vec3("viewPos", camera.position);

    // TODO chunk render queue
    for (auto& [coords, chunk] : loaded_chunks) {
      if (chunk->dirty) {
        chunk->remesh(this->chunks);
        chunk->dirty = false;
      }
      chunk->render(camera);
    }
  }
};

#endif