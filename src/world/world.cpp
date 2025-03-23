#include "world.h"
#include "chunk.h"
#include "world_generator.h"
#include <algorithm>

using namespace glm;

World::World() : generator() {}
World::~World() {}

void World::prepare(const Camera &camera)
{
  shader.use();
}

ivec3 World::retrieve_chunk_coords(const ivec3 &p)
{
  // Efficient chunk coordinate calculation for negative and positive coordinates
  // Assumes chunks_size is a power of 2 (like 16, 32, etc.)
  const int shift = glm::log2((float)chunks_size);
  return ivec3(
      (p.x >> shift) - (p.x < 0 ? 1 : 0),
      0,
      (p.z >> shift) - (p.z < 0 ? 1 : 0));
  return floor((vec3)p / vec3(chunks_size));
}
Chunk World::retrieve_chunk(const ivec3 &p)
{
  return chunks[retrieve_chunk_coords(p)];
}

Block World::operator[](const ivec3 &p)
{
  Chunk chunk = retrieve_chunk(p);
  return chunk[p & ivec3(chunks_size - 1)];
}

template <typename T>
bool World::thread_is_done(const std::future<T> &t)
{
  return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool World::inside_frustum(const Frustum &frustum, const ivec3 &chunk_coords)
{
  // Convert to world coordinates
  vec3 min = vec3(chunk_coords * CHUNKS_SIZE);
  vec3 max = min + vec3(CHUNKS_SIZE, WORLD_HEIGHT, CHUNKS_SIZE);

  // Calculate center and half-extents of the AABB
  vec3 center = (min + max) * 0.5f;
  vec3 extents = (max - min) * 0.5f;

  const float EPSILON = 0.1f;

  // Test AABB against all frustum planes
  for (int i = 0; i < 6; i++)
  {
    const vec4 &plane = frustum.planes[i];
    vec3 normal = vec3(plane);

    // TODO il y a un pb avec un des coins qui doit etre calculé par un min au lieu d'un max pour le z négatif, surely.

    // Calculate radius in the direction of the plane normal
    float radius = extents.x * std::abs(normal.x) +
                   extents.y * std::abs(normal.y) +
                   extents.z * std::abs(normal.z);

    // Calculate distance from center to plane
    float distance = dot(normal, center) + plane.w;

    // If distance is negative and greater than radius, the AABB is outside
    if (distance < -radius - EPSILON)
    {
      return false;
    }
  }

  return true;
}

void World::load_close_chunks(const Frustum &frustum, const ivec3 &player_chunk_coords)
{
  visible_chunks.clear();

  for (int x = -render_distance; x < render_distance; x++)
    for (int z = -render_distance; z < render_distance; z++)
    {
      float dist_sq = x * x + z * z;
      ivec3 coords = ivec3(x, 0, z) + player_chunk_coords;
      if (dist_sq >= render_distance * render_distance)
        continue;
      if (coords != player_chunk_coords && !inside_frustum(frustum, coords))
        continue;
      visible_chunks.emplace_back(coords, dist_sq);
    }

  std::sort(visible_chunks.begin(), visible_chunks.end(),
            [&](const auto &a, const auto &b)
            { return a.second < b.second; });

  for (const auto &[chunk_coords, dist_sq] : visible_chunks)
  {
    if (chunks.find(chunk_coords) == chunks.end())
    {
      chunks.emplace(std::piecewise_construct,
                     std::forward_as_tuple(chunk_coords),
                     std::forward_as_tuple(chunk_coords, &shader));
    }
  }
}

void World::set_view_clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.63f, 0.86f, 1.0f, 1.0f);
}

void World::add_chunks_to_render_queue()
{
  for (auto &[coords, _] : visible_chunks)
  {
    Chunk *chunk = &chunks[coords];
    if (chunk->dirty && !chunk->meshing &&
        active_threads.size() < MAX_ACTIVE_THREADS)
    {
      chunk->meshing = true;
      active_threads.emplace_back(
          make_pair(chunk, std::async(std::launch::async, [chunk, this]()
                                      {
                        generator.fill_with_terrain(chunk->blocks, chunk->origin, chunk->active_count);
                        chunk->prepare_mesh_data(generator, this->chunks); })));
    }
  }
}

void World::cleanup_meshed_chunks()
{
  for (auto it = active_threads.begin(); it != active_threads.end();)
  {
    auto &[chunk, t] = *it;
    if (thread_is_done(t))
    {
      chunk->upload_to_gpu();
      chunk->dirty = false;
      chunk->meshing = false;
      it = active_threads.erase(it);
    }
    else
      ++it;
  }
}

void World::render_chunks(const Frustum &frustum, const ivec3 &player_chunk_coords, const Camera &camera)
{
  for (auto &[coords, _] : visible_chunks)
  {
    Chunk &chunk = chunks[coords];
    if (!chunk.dirty)
    {
      shader.uniform_vec3("chunkOrigin", chunk.origin);
      chunk.render(camera);
    }
  }
}

void World::render(const Camera &camera)
{
  ivec3 player_coords = ivec3(camera.position);
  ivec3 player_chunk_coords = retrieve_chunk_coords(player_coords);

  mat4 v = camera.get_view_matrix();
  mat4 p = camera.get_perspective_matrix();
  mat4 pv = p * v;
  shader.use();
  shader.uniform_mat4("m_PerspectiveView", pv);
  shader.uniform_vec3("viewPos", camera.position);
  Frustum frustum(pv);
  load_close_chunks(frustum, player_chunk_coords);
  set_view_clear();
  add_chunks_to_render_queue();
  cleanup_meshed_chunks();
  render_chunks(frustum, player_chunk_coords, camera);
  // TODO every minutes, remove from memory very far chunks (burn to memory any change made to them).
}