#include "WorldMap.hpp"


#include "SDL3_utils.hpp"     // log_error, etc.

#include "entt/entt.hpp"      // entt::registry
#include "SDL3/SDL_stdinc.h"  // Sint64 (named integer type)

#include <unordered_map>      // std::unordered_map


using bipsy::sdl3_utils::Log;


namespace bipsy::gaiasim
{


WorldMap::WorldMap()
: m_chunk_registry{}, m_chunk_map{}
{
  // Constructor implementation (if needed)
}


WorldMap::~WorldMap()
{
  // Destructor implementation (if needed)
}


entt::entity WorldMap::create_chunk(const ChunkPos & pos)
{
  // Check to make sure a chunk doesn't already exist at this position
  auto it = m_chunk_map.find(pos);
  if (it != m_chunk_map.end())
  {
    Log::error(
            "Chunk already exists at position ({}, {}, {})", pos.x, pos.y, pos.z
    );
    return entt::null;  // Return null entity to indicate failure
  }

  // Create a new chunk entity in the registry
  entt::entity chunk_entity = m_chunk_registry.create();
  // add components (data) to the chunk entity
  m_chunk_registry.emplace<ChunkPos>(
          chunk_entity,
          pos
  );  // Store the chunk's position as a component
  m_chunk_registry.emplace<ChunkData>(
          chunk_entity
  );  // Add chunk data component (with default constructor)
  m_chunk_registry.emplace<BlockEntities>(
          chunk_entity
  );  // Add block entities component (with default
      // constructor)

  // Add the new chunk to the map
  m_chunk_map[pos] = chunk_entity;

  return chunk_entity;
}


bool WorldMap::delete_chunk(const ChunkPos & pos)
{
  // Get the chunk entity if it exists
  auto it = m_chunk_map.find(pos);
  if (it == m_chunk_map.end())
  {
    Log::error("No chunk exists at position ({}, {}, {}) to delete",
               pos.x,
               pos.y,
               pos.z);
    return false;  // Indicate failure to find
  }

  // Destroy the chunk entity in the registry (free up component data)
  m_chunk_registry.destroy(it->second);

  // Remove the chunk from the unordered_map
  m_chunk_map.erase(it);

  return true;  // Indicate successful deletion
}


bool WorldMap::delete_chunk(entt::entity chunk_entity)
{
  // determine if entity exists first
  if (!m_chunk_registry.valid(chunk_entity))
  {
    Log::error("Invalid entity ID {} passed to delete_chunk",
               static_cast<Uint32>(chunk_entity));
    return false;
  }
  // attempt to get the component, fail if it doesn't exist
  auto chunk_pos = m_chunk_registry.try_get<ChunkPos>(
          chunk_entity
  );  // Check if the chunk has a ChunkPos component
  if (chunk_pos == nullptr)
  {
    Log::error("No chunk exists with entity ID {} to delete",
               static_cast<Uint32>(chunk_entity));
    return false;  // Indicate failure to find
  }
  // delete based on chunk position component
  return delete_chunk(*chunk_pos);
}


entt::entity WorldMap::get_chunk(const ChunkPos & pos) const
{
  // Check if the chunk exists at this position
  auto it = m_chunk_map.find(pos);
  if (it == m_chunk_map.end())
  {
    Log::error("No chunk exists at position ({}, {}, {}) to get",
               pos.x,
               pos.y,
               pos.z);
    return entt::null;  // Indicate failure to find
  }

  // Get the chunk entity and return it
  entt::entity chunk_entity = it->second;
  return chunk_entity;
}


BlockID WorldMap::get_block(const ChunkPos & chunk_pos,
                            const LocalPos & local_pos) const
{
  Log::info("get_block called with chunk_pos ({}, {}, {}) and local coords "
            "({}, {}, {})",
            chunk_pos.x,
            chunk_pos.y,
            chunk_pos.z,
            local_pos.x,
            local_pos.y,
            local_pos.z);

  // make sure `local_pos` is within bounds of the chunk size
  if (local_pos.x >= ChunkData::SIZE || local_pos.y >= ChunkData::SIZE
      || local_pos.z >= ChunkData::SIZE)
  {
    Log::error("Local position ({}, {}, {}) is out of bounds for chunk size {}",
               local_pos.x,
               local_pos.y,
               local_pos.z,
               ChunkData::SIZE);
    return INVALID;  // Return invalid block ID to indicate out of bounds
  }

  // get the chunk entity for this chunk position
  auto chunk_entity = get_chunk(chunk_pos);
  if (chunk_entity == entt::null)
  {
    Log::error("Cannot get block because chunk does not exist at position "
               "({}, {}, {})",
               chunk_pos.x,
               chunk_pos.y,
               chunk_pos.z);
    return INVALID;  // Not generated yet
  }

  // get the chunk data component for this chunk entity
  const ChunkData & chunk_data = m_chunk_registry.get<ChunkData>(chunk_entity);
  // get the block ID from the chunk data using the local block coordinates
  int     index = (local_pos.z * ChunkData::SIZE * ChunkData::SIZE)
                + (local_pos.y * ChunkData::SIZE) + local_pos.x;
  BlockID block = chunk_data.blocks[index];
  Log::info("Block ID at local coords ({}, {}, {}) in chunk ({}, {}, {}) is {}",
            local_pos.x,
            local_pos.y,
            local_pos.z,
            chunk_pos.x,
            chunk_pos.y,
            chunk_pos.z,
            block);

  return block;
}


BlockID WorldMap::get_block(Sint64 global_x,
                            Sint64 global_y,
                            Sint64 global_z) const
{
  auto [chunk_pos, local_pos]
          = convert_global_to_chunk_pos(global_x, global_y, global_z);
  return get_block(chunk_pos, local_pos);
}


bool WorldMap::set_block(const ChunkPos & chunk_pos,
                         const LocalPos & local_pos,
                         BlockID          block_id)
{
  Log::info("set_block (block={}) called with chunk_pos ({}, {}, {}) and "
            "local coords ({}, {}, {})",
            block_id,
            chunk_pos.x,
            chunk_pos.y,
            chunk_pos.z,
            local_pos.x,
            local_pos.y,
            local_pos.z);

  // make sure `local_pos` is within bounds of the chunk size
  if (local_pos.x >= ChunkData::SIZE || local_pos.y >= ChunkData::SIZE
      || local_pos.z >= ChunkData::SIZE)
  {
    Log::error("Local position ({}, {}, {}) is out of bounds for chunk size {}",
               local_pos.x,
               local_pos.y,
               local_pos.z,
               ChunkData::SIZE);
    return false;  // Return false to indicate failure to set block
  }

  // get the chunk entity for this chunk position
  auto chunk_entity = get_chunk(chunk_pos);
  if (chunk_entity == entt::null)
  {
    Log::error("Cannot set block because chunk does not exist at position "
               "({}, {}, {})",
               chunk_pos.x,
               chunk_pos.y,
               chunk_pos.z);
    return false;  // Return false to indicate failure to set block
  }

  // get the chunk data component for this chunk entity
  ChunkData & chunk_data   = m_chunk_registry.get<ChunkData>(chunk_entity);
  // set the block ID in the chunk data using the local block coordinates
  int     index            = (local_pos.z * ChunkData::SIZE * ChunkData::SIZE)
                           + (local_pos.y * ChunkData::SIZE) + local_pos.x;
  BlockID old_id           = chunk_data.blocks[index];
  chunk_data.blocks[index] = block_id;
  Log::info("Block ID at local coords ({}, {}, {}) in chunk ({}, {}, {}) "
            "changed from {} to {}",
            local_pos.x,
            local_pos.y,
            local_pos.z,
            chunk_pos.x,
            chunk_pos.y,
            chunk_pos.z,
            old_id,
            block_id);

  // success
  return true;
}


bool WorldMap::set_block(Sint64  global_x,
                         Sint64  global_y,
                         Sint64  global_z,
                         BlockID block_id)
{
  auto [chunk_pos, local_pos]
          = convert_global_to_chunk_pos(global_x, global_y, global_z);
  return set_block(chunk_pos, local_pos, block_id);
}


std::tuple<ChunkPos, LocalPos> WorldMap::convert_global_to_chunk_pos(
        Sint64 global_x, Sint64 global_y, Sint64 global_z
) const
{
  // TODO replace with simpler calculations -- floor division instead

  // Integer division gets us the chunk coordinates
  // Subtract SIZE before division to account for first negative chunk being -1
  // instead of 0 Add 1 before division to handle the 1 block offset from (0, 0,
  // 0) being owned by chunk {0, 0, 0}
  Sint64 chunk_x = (global_x < 0 ? global_x - ChunkData::SIZE + 1 : global_x)
                 / ChunkData::SIZE;
  Sint64 chunk_y = (global_y < 0 ? global_y - ChunkData::SIZE + 1 : global_y)
                 / ChunkData::SIZE;
  Sint64 chunk_z = (global_z < 0 ? global_z - ChunkData::SIZE + 1 : global_z)
                 / ChunkData::SIZE;

  // Modulo gets us the local block coordinates within the chunk
  // Add SIZE after modulo if it's negative to handle negative coordinates
  // correctly BUT make sure 16 % 16 = 0, not 16, so only add SIZE if the result
  // is not zero
  Uint8 local_x
          = global_x % ChunkData::SIZE
          + (global_x < 0 && global_x % ChunkData::SIZE != 0 ? ChunkData::SIZE :
                                                               0);
  Uint8 local_y
          = global_y % ChunkData::SIZE
          + (global_y < 0 && global_y % ChunkData::SIZE != 0 ? ChunkData::SIZE :
                                                               0);
  Uint8 local_z
          = global_z % ChunkData::SIZE
          + (global_z < 0 && global_z % ChunkData::SIZE != 0 ? ChunkData::SIZE :
                                                               0);

  return {
      ChunkPos{chunk_x, chunk_y, chunk_z},
      LocalPos{local_x, local_y, local_z}
  };
}


GlobalPos WorldMap::convert_chunk_to_global_pos(Sint64 chunk_x,
                                                Sint64 chunk_y,
                                                Sint64 chunk_z,
                                                Uint8  local_x,
                                                Uint8  local_y,
                                                Uint8  local_z) const
{
  // TODO implement
  Log::error(
          "convert_chunk_to_global_pos not implemented yet, returning (0, 0, "
          "0)"
  );
  return GlobalPos{0, 0, 0};
}


}  // namespace bipsy::gaiasim
