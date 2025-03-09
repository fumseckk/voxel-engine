#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "world_generator.h"

class World {
 public:
  const int render_distance = RENDER_DISTANCE;
  const int chunks_size = CHUNKS_SIZE;
  FastNoiseLite noise;
  unordered_map<glm::ivec3, Chunk> chunks;
  unordered_map<glm::ivec3, Chunk*> loaded_chunks;
  vector<pair<Chunk*, std::future<void>>> active_threads;
  Shader shader = Shader("resources/shaders/default.vert",
                         "resources/shaders/default.frag");
  TextureArray texture_array = TextureArray("resources/textures");
  WorldGenerator generator;
  World() : generator() {
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalWeightedStrength(2.0f);
  }
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

  template <typename T>
  bool thread_is_done(std::future<T>& t) {
    return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
  }

  void unload_far_chunks(glm::ivec3& player_chunk_coords) {
    for (auto it = loaded_chunks.cbegin(); it != loaded_chunks.cend();) {
      Chunk* chunk = it->second;
      glm::ivec3 orig = chunk->origin - player_chunk_coords;
      if (orig.x * orig.x + orig.y * orig.y + orig.z * orig.z >
          render_distance * render_distance)
        loaded_chunks.erase(it++);
      else
        it++;
    }
  }

  void load_close_chunks(glm::ivec3& player_chunk_coords) {
    for (int x = -render_distance; x < render_distance; x++) {
      for (int z = -render_distance; z < render_distance; z++) {
        if (x * x + z * z > render_distance * render_distance) continue;
        glm::ivec3 chunk_coords =
            glm::vec3(player_chunk_coords.x + x, 0, player_chunk_coords.z + z);
        if (chunks.find(chunk_coords) == chunks.end()) {
          chunks.emplace(std::piecewise_construct,
                         std::forward_as_tuple(chunk_coords),
                         std::forward_as_tuple(chunk_coords, &shader));
          generator.fill_with_terrain(chunks[chunk_coords].blocks, chunk_coords * glm::ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE),
                                      chunks[chunk_coords].active_count);
          loaded_chunks.emplace(chunk_coords, &chunks[chunk_coords]);
        } else if (loaded_chunks.find(chunk_coords) == loaded_chunks.end()) {
          loaded_chunks.emplace(chunk_coords, &chunks[chunk_coords]);
        }
      }
    }
  }

  void set_view_clear(Camera& camera) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.63f, 0.86f, 1.0f, 1.0f);
    glm::mat4 v = camera.get_view_matrix();

    shader.use();
    shader.uniform_mat4("v", v);
    shader.uniform_vec3("viewPos", camera.position);
  }

  void add_chunks_to_render_queue() {
    for (auto& [coords, chunk] : loaded_chunks) {
      if (chunk->dirty && !chunk->meshing &&
          active_threads.size() < MAX_ACTIVE_THREADS) {
        chunk->meshing = true;
        active_threads.emplace_back(
            make_pair(chunk, std::async(std::launch::async, [chunk, this]() {
                        chunk->prepare_mesh_data(this->chunks);
                      })));
      }
    }
  }

  void cleanup_meshed_chunks() {
    for (auto it = active_threads.begin(); it != active_threads.end();) {
      auto& [chunk, t] = *it;
      if (thread_is_done(t)) {
        chunk->upload_to_gpu();
        chunk->dirty = false;
        chunk->meshing = false;
        it = active_threads.erase(it);
      } else {
        ++it;
      }
    }
  }

  void render_chunks(Camera& camera) {
    for (auto& [coords, chunk] : loaded_chunks) {
      if (chunk->dirty) continue;
      shader.uniform_vec3("chunkOrigin", chunk->origin);
      chunk->render(camera);
    }
  }

  void render(Camera& camera) {
    glm::ivec3 player_coords = glm::ivec3(camera.position);
    glm::ivec3 player_chunk_coords = retrieve_chunk_coords(player_coords);

    unload_far_chunks(player_chunk_coords);
    load_close_chunks(player_chunk_coords);
    set_view_clear(camera);
    add_chunks_to_render_queue();
    cleanup_meshed_chunks();
    render_chunks(camera);
  }
};

#endif