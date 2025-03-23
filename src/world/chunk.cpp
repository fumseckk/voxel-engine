#include "chunk.h"
#include "world_generator.h"

using namespace glm;

bool Chunk::player_sees_face(const Camera &camera, const Direction &dir)
{
  vec3 pos = camera.position;
  switch (dir)
  {
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
    return false;
  }
}

Block Chunk::operator[](const ivec3 &p)
{
  assert(in_range(p));
  return blocks[ivec3_to_index(p)];
}

ivec3 Chunk::retrieve_chunk_coords(const ivec3 &p)
{
  // division rounded down for consistency in the negatives
  return floor((vec3)p / vec3(CHUNKS_SIZE));
}

bool Chunk::chunk_exists(const ivec3 &chunk_coords, const unordered_map<ivec3, Chunk> &chunks)
{
  return chunks.find(chunk_coords) != chunks.end();
}

std::optional<Block> Chunk::get_world_block(const ivec3 &world_pos, unordered_map<ivec3, Chunk>& chunks)
{
  ivec3 chunk_coords = retrieve_chunk_coords(world_pos);
  if (!chunk_exists(chunk_coords, chunks))
    return std::nullopt;

  ivec3 local_pos = world_pos - chunk_coords * ivec3(CHUNKS_SIZE, 0, CHUNKS_SIZE);
  if (local_pos.y < 0 || local_pos.y >= WORLD_HEIGHT)
    return std::nullopt;

  return chunks[chunk_coords][local_pos];
}

void Chunk::prepare_mesh_data(const WorldGenerator &generator, const unordered_map<ivec3, Chunk> &chunks)
{
  if (origin == ivec3(0)) printf("REMESHING 0,0,0\n");
  static glm::ivec3 dir[]{
      glm::ivec3(0, 0, 1),
      glm::ivec3(0, 0, -1),
      glm::ivec3(-1, 0, 0),
      glm::ivec3(1, 0, 0),
      glm::ivec3(0, -1, 0),
      glm::ivec3(0, 1, 0),
  };
  
  if (active_count == 0)
    return;
  for (int d = (Direction)0; d < DIRECTION_COUNT; d++)
  {
    mesh[d].faces_count = 0;
    mesh[d].buffer.clear();
    for (int x = 0; x < CHUNKS_SIZE; x++)
    {
      for (int y = 0; y < WORLD_HEIGHT; y++)
      {
        for (int z = 0; z < CHUNKS_SIZE; z++)
        {
          ivec3 p(x, y, z);
          Block block = (*this)[p];
          if (block.type != AIR)
          {
            ivec3 neigh = p + dir[d];

            if (in_range(neigh))
            {
              if ((*this)[neigh].type != AIR && (*this)[neigh].type != LEAVES)
                continue;
            }
            else
            {
              // This improves performance by 30%, but not reliable
              if (generator.get_height(neigh.x, neigh.z, origin) > y)
                continue;

              // Convert local position to world position
              // ivec3 world_pos = origin + neigh;
              // auto neighbor_block = get_world_block(world_pos, chunks);

              // // Skip face creation if there's a block in the neighboring chunk
              // if (neighbor_block.has_value() &&
              //     neighbor_block.value().type != AIR &&
              //     neighbor_block.value().type != LEAVES)
              //   continue;

              // Don't render bottom face at y=0
              if (y == 0 && d == (int)DOWN)
                continue;
            }

            mesh[d].faces_count++;
            set_face_at_coords(p, (Direction)d, block.type);
          }
        }
      }
    }
  }
}

void Chunk::set_face_at_coords(const vec3 &coords, const Direction &dir, const BlockType &type)
{
  // TODO enlever la direction d'ici et en faire un autre ssbo chunk-wise
  mesh[dir].buffer.push_back(
      ivec4(coords.x, coords.y, coords.z, dir | type << 4));
}

void Chunk::upload_to_gpu()
{
  if (active_count == 0)
    return;
  for (int d = 0; d < 6; d++)
  {
    mesh[d].ssbo.set_buffer(mesh[d].buffer.data(),
                            mesh[d].buffer.size() * sizeof(ivec4), 0);
  }
}

void Chunk::render(const Camera &camera)
{
  for (int d = 0; d < 6; d++)
  {
    if (!player_sees_face(camera, (Direction)d))
      continue;

    if (mesh[d].faces_count == 0)
      continue;

    mesh[d].vao.bind();
    mesh[d].ssbo.bind(0);
    glDrawArrays(GL_TRIANGLES, 0, mesh[d].buffer.size() * 6);
  }
}