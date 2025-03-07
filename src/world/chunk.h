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
#include <thread>
#include <future>

#define MAX_ACTIVE_THREADS 16
#define CHUNKS_SIZE 32
#define WORLD_HEIGHT 50
#define RENDER_DISTANCE 20

enum BlockType { EMPTY, GRASS, DIRT };

enum Direction { BACKWARD, FORWARD, LEFT, RIGHT, DOWN, UP };
#define FIRST_DIRECTION BACKWARD
#define LAST_DIRECTION UP
glm::ivec3 dir[]{
    glm::ivec3(0, 0, 1), glm::ivec3(0, 0, -1), glm::ivec3(-1, 0, 0),
    glm::ivec3(1, 0, 0), glm::ivec3(0, -1, 0), glm::ivec3(0, 1, 0),
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
  SSBO ssbo;
  vector<glm::ivec4> buffer;
  int faces_count = 0;
  ChunkMesh() : ssbo(SSBO(nullptr, false)) { assert(false); }
  ChunkMesh(Shader* shader) : vao(VAO()), ssbo(SSBO(shader, false)) {}
  ~ChunkMesh() {}
};

class Chunk {
 public:
  FastNoiseLite noise;
  bool dirty = true;
  bool meshing = false;
  glm::ivec3 origin;
  int active_count = 0;
  int highest_block = 0;
  ChunkMesh mesh[6];
  int nb_blocks = CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE;
  Block blocks[CHUNKS_SIZE * WORLD_HEIGHT * CHUNKS_SIZE];

  Chunk() { assert(false); }
  Chunk(glm::ivec3 origin, Shader* shader)
      : mesh{ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader),
              ChunkMesh(shader), ChunkMesh(shader), ChunkMesh(shader)} {
    this->origin = origin * glm::ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE);
    fill_with_terrain();
    dirty = true;
  }

  ~Chunk() {};

  bool player_sees_face(Camera& camera, Direction dir) {
    glm::vec3 pos = camera.position;
    switch(dir) {
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

  int get_height(int x, int z) {
    float height =
        noise.GetNoise((float)(x + origin.x), (float)(z + origin.z)) / 2.0 +
        0.5f;
    return min(WORLD_HEIGHT - 1, (int)((float)WORLD_HEIGHT * height + 1));
  }

  void fill_with_terrain() {
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

class World {
 public:
  const int render_distance = RENDER_DISTANCE;
  const int chunks_size = CHUNKS_SIZE;
  unordered_map<glm::ivec3, Chunk> chunks;
  unordered_map<glm::ivec3, Chunk*> loaded_chunks;
  vector<pair<Chunk*, std::future<void>>> active_threads;
  Shader shader = Shader("resources/shaders/default.vert",
                         "resources/shaders/default.frag");
  Texture texture = Texture("resources/textures/grass_block.png");
  World() {}
  ~World() {
    cleanup();
  }

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

  template<typename T>
  bool thread_is_done(std::future<T>& t) {
    return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
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
                         std::forward_as_tuple(chunk_coords, &shader));
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



    

    for (auto& [coords, chunk] : loaded_chunks) {
      if (chunk->dirty && !chunk->meshing && active_threads.size() < MAX_ACTIVE_THREADS) {
        chunk->meshing = true;
        active_threads.emplace_back(make_pair(chunk, std::async(std::launch::async, [chunk, this]() {
          chunk->prepare_mesh_data(this->chunks);
        })));
      }
    }
    
    // TODO change this to just wait until it has finished
    // right now the join time is the same for every chunk no matter when it started loading
    for (auto it = active_threads.begin(); it != active_threads.end(); ) {
      auto& [chunk, t] = *it;
      if (thread_is_done(t)) {
        chunk->upload_to_gpu();
        chunk->dirty = false;
        chunk->meshing = false;
        it = active_threads.erase(it);
      }
      else {
        ++it;
      }
    }
    
    // Render all chunks
    for (auto& [coords, chunk] : loaded_chunks) {
      if (chunk->dirty) continue;

      shader.uniform_vec3("chunkOrigin", chunk->origin);
      chunk->render(camera);
    }
  }

  void cleanup() {
    // for (auto& [chunk, t] : active_threads) {
    //   if (t.joinable()) {
    //     t.join();
    //   }
    // }
    // active_threads.clear();
  }
};

#endif