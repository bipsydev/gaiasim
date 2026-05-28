#include "WorldMap.hpp"


#include "SDL_utils.hpp" // log_error, etc.

#include "entt/entt.hpp"  // entt::registry

#include "SDL3/SDL_stdinc.h" // Sint64 (named integer type)

#include <unordered_map>  // std::unordered_map


namespace bipsy::gaiasim
{

using namespace bipsy::sdlutils; // log_error, etc.


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
    return entt::null; // Return null entity to indicate failure
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
                            const LocalPos& local_pos) const
{
  // TODO implement
  log_warn("get_block not implemented yet");
  log_info("get_block called with chunk_pos (%i, %i, %i) and local coords (%i, %i, %i)",
    1, chunk_pos.x, chunk_pos.y, chunk_pos.z, local_pos.x, local_pos.y, local_pos.z);
  return 0;
}


BlockID WorldMap::get_block(Sint64 global_x, Sint64 global_y, Sint64 global_z) const
{
  auto [chunk_pos, local_pos] = convert_global_to_chunk_pos(global_x, global_y, global_z);
  return get_block(chunk_pos, local_pos);
}


bool WorldMap::set_block(const ChunkPos& chunk_pos,
                         const LocalPos& local_pos,
                         BlockID block_id)
{
  // TODO implement
  log_warn("set_block not implemented yet");
  return false;
}


bool WorldMap::set_block(Sint64 global_x, Sint64 global_y, Sint64 global_z,
                         BlockID block_id)
{
  auto [chunk_pos, local_pos] = convert_global_to_chunk_pos(global_x, global_y, global_z);
  return set_block(chunk_pos, local_pos, block_id);
}


std::tuple<ChunkPos, LocalPos> WorldMap::convert_global_to_chunk_pos
(Sint64 global_x, Sint64 global_y, Sint64 global_z) const
{
  //TODO replace with simpler calculations -- floor division instead

  // Integer division gets us the chunk coordinates
  // Subtract SIZE before division to account for first negative chunk being -1 instead of 0
  // Add 1 before division to handle the 1 block offset from (0, 0, 0) being owned by chunk {0, 0, 0}
  Sint64 chunk_x = (global_x < 0 ? global_x - ChunkData::SIZE + 1 : global_x) / ChunkData::SIZE;
  Sint64 chunk_y = (global_y < 0 ? global_y - ChunkData::SIZE + 1 : global_y) / ChunkData::SIZE;
  Sint64 chunk_z = (global_z < 0 ? global_z - ChunkData::SIZE + 1 : global_z) / ChunkData::SIZE;
  
  // Modulo gets us the local block coordinates within the chunk
  // Add SIZE after modulo if it's negative to handle negative coordinates correctly
  // BUT make sure 16 % 16 = 0, not 16, so only add SIZE if the result is not zero
  Uint8 local_x = global_x % ChunkData::SIZE + (global_x < 0 && global_x % ChunkData::SIZE != 0 ? ChunkData::SIZE : 0);
  Uint8 local_y = global_y % ChunkData::SIZE + (global_y < 0 && global_y % ChunkData::SIZE != 0 ? ChunkData::SIZE : 0);
  Uint8 local_z = global_z % ChunkData::SIZE + (global_z < 0 && global_z % ChunkData::SIZE != 0 ? ChunkData::SIZE : 0);

  return {ChunkPos{chunk_x, chunk_y, chunk_z},
          LocalPos{local_x, local_y, local_z}};
}


GlobalPos WorldMap::convert_chunk_to_global_pos
(Sint64 chunk_x, Sint64 chunk_y, Sint64 chunk_z,
 Uint8 local_x, Uint8 local_y, Uint8 local_z) const
{
  //TODO implement
  log_error("convert_chunk_to_global_pos not implemented yet, returning (0, 0, 0)");
  return GlobalPos{0, 0, 0};
}



} // namespace bipsy::gaiasim
