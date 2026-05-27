#include "WorldMap.hpp"


#include "SDL_utils.hpp" // log_error, etc.

#include "entt/entt.hpp"  // entt::registry

#include "SDL3/SDL_stdinc.h" // Sint64 (named integer type)

#include <unordered_map> // std::unordered_map


namespace bipsy::gaiasim
{

using namespace sdlutils; // log_error, etc.


WorldMap::WorldMap()
: m_chunk_registry{},
  m_chunk_map{}
{
  // Constructor implementation (if needed)
}


WorldMap::~WorldMap()
{
  // Destructor implementation (if needed)
}


entt::entity WorldMap::create_chunk(const ChunkPos& pos)
{
  // Check to make sure a chunk doesn't already exist at this position
  auto it = m_chunk_map.find(pos);
  if (it != m_chunk_map.end())
  {
    log_error("Chunk already exists at position (%i, %i, %i)", 0, pos.x, pos.y, pos.z);
    return it->second; // Return existing chunk entity
  }

  // Create a new chunk entity in the registry
  entt::entity chunk_entity = m_chunk_registry.create();
  // add components (data) to the chunk entity
  m_chunk_registry.emplace<ChunkPos>(chunk_entity, pos); // Store the chunk's position as a component
  m_chunk_registry.emplace<ChunkData>(chunk_entity);     // Add chunk data component (with default constructor)
  m_chunk_registry.emplace<BlockEntities>(chunk_entity); // Add block entities component (with default constructor)

  // Add the new chunk to the map
  m_chunk_map[pos] = chunk_entity;

  return chunk_entity;
}


bool WorldMap::delete_chunk(const ChunkPos& pos)
{
  // TODO implement
  log_warn("delete_chunk not implemented yet");
  return false;
}


entt::entity WorldMap::get_chunk(const ChunkPos& pos) const
{
  // TODO implement
  log_warn("get_chunk not implemented yet");
  return entt::null;
}


BlockID WorldMap::get_block(const ChunkPos& chunk_pos,
                             int local_x, int local_y, int local_z) const
{
  // TODO implement
  log_warn("get_block not implemented yet");
  return 0;
}


BlockID WorldMap::get_block(Sint64 global_x, Sint64 global_y, Sint64 global_z) const
{
  return get_block(
    // Integer division gets us the chunk coordinates
    ChunkPos{
      global_x / ChunkData::SIZE,
      global_y / ChunkData::SIZE,
      global_z / ChunkData::SIZE
    },
    // Modulo gets us the local block coordinates within the chunk
    global_x % ChunkData::SIZE,
    global_y % ChunkData::SIZE,
    global_z % ChunkData::SIZE
  );
}


bool WorldMap::set_block(const ChunkPos& chunk_pos,
                         int local_x, int local_y, int local_z,
                         BlockID block_id)
{
  // TODO implement
  log_warn("set_block not implemented yet");
  return false;
}


bool WorldMap::set_block(Sint64 global_x, Sint64 global_y, Sint64 global_z,
                         BlockID block_id)
{
  return set_block(
    // Integer division again
    ChunkPos{
      global_x / ChunkData::SIZE,
      global_y / ChunkData::SIZE,
      global_z / ChunkData::SIZE
    },
    // Modulo again
    global_x % ChunkData::SIZE,
    global_y % ChunkData::SIZE,
    global_z % ChunkData::SIZE,
    // pass through block_id to set
    block_id);
}


} // namespace bipsy::gaiasim
