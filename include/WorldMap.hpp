#pragma once
#ifndef BIPSY_GAIASIM_WORLDMAP_HPP
#define BIPSY_GAIASIM_WORLDMAP_HPP


#include "entt/entt.hpp"  // entt::registry

#include "SDL3/SDL_stdinc.h" // Sint64 (named integer type)

#include <array>          // std::array
#include <unordered_map>  // std::unordered_map
#include <limits>         // std::numeric_limits
#include <functional>     // std::hash


namespace bipsy::gaiasim
{

// Use 64-bit unsigned integers for block IDs
using BlockID = Uint64;


// ------------------------------ EnTT Components ------------------------------

// A position in chunk-coordinates
struct ChunkPos
{
  Sint64 x = 0;
  Sint64 y = 0;
  Sint64 z = 0;

  ChunkPos(Sint64 x = 0, Sint64 y = 0, Sint64 z = 0)
  : x{x}, y{y}, z{z}
  { }

  bool operator==(const ChunkPos& other) const
  {
    return x == other.x && y == other.y && z == other.z;
  }

  // For use in unordered_map, we need to define a hash function
  // as a separate class in order to pass as template argument to unordered_map
  class Hash
  {
  public:
    std::size_t operator()(const ChunkPos& pos) const
    {
      // Combine the hash of x, y, and z using a common technique
      std::size_t h1 = std::hash<Sint64>{}(pos.x);
      std::size_t h2 = std::hash<Sint64>{}(pos.y);
      std::size_t h3 = std::hash<Sint64>{}(pos.z);
      return h1 ^ (h2 << 1) ^ (h3 << 2); // Simple combination
    }
  };

};


// Voxel/block data for a chunk
struct ChunkData
{
  // size of a chunk (in all 3 dimensions)
  static constexpr int SIZE = 16;

  // 3D array of block IDs (flattened to 1D)
  std::array<BlockID, SIZE * SIZE * SIZE> blocks{};
};


// Hashmap for block entities (block metadata)
// Uses flat 1D indexing for blocks within a local chunk (0 to SIZE^3 - 1)
struct BlockEntities
{
  // 16-bit integer can hold SIZE^3 block indices (0 to SIZE^3 - 1)
  using BlockEntitiesIndex = Uint16;
  // Map from block index (0 to SIZE^3 - 1) to block entity
  std::unordered_map<BlockEntitiesIndex, entt::entity> block_entity_map{};
  // Ensure that the chunk size is small enough to fit within the index size
  static_assert(ChunkData::SIZE * ChunkData::SIZE * ChunkData::SIZE
    <= std::numeric_limits<BlockEntitiesIndex>::max(),
    "Chunk size is too large for block entity map");
};



// ------------------------------ WorldMap class -------------------------------


/**
 * @brief Represents the 3D voxel-based world. Accesses Blocks via Chunks.
 * 
 * Uses EnTT to hold Chunks as Entities
 * 
 */
class WorldMap
{
  // Holds Chunks as Entities
  entt::registry m_chunk_registry;

  // 3D integer coordinates -> chunk entity
  // pass hashing function for ChunkPos to unordered_map
  std::unordered_map<ChunkPos, entt::entity, ChunkPos::Hash> m_chunk_map;


public:

  // Constructor
  WorldMap();

  // Destructor
  ~WorldMap();


  // registry getters
  entt::registry& registry()              { return m_chunk_registry; }
  const entt::registry& registry() const  { return m_chunk_registry; }

  // component getters
  template<typename Component>
  Component& get_component(entt::entity entity)
  { return m_chunk_registry.get<Component>(entity); }


  // Chunk creation functions
  entt::entity create_chunk(const ChunkPos& pos);

  entt::entity create_chunk(Sint64 x, Sint64 y, Sint64 z)
  { return create_chunk(ChunkPos{x, y, z}); }


  // Chunk deletion functions
  bool delete_chunk(const ChunkPos& pos);

  bool delete_chunk(Sint64 x, Sint64 y, Sint64 z)
  { return delete_chunk(ChunkPos{x, y, z}); }

  bool delete_chunk(entt::entity chunk_entity)
  { return delete_chunk(m_chunk_registry.get<ChunkPos>(chunk_entity)); }


  // Chunk retrieval functions
  entt::entity get_chunk(const ChunkPos& pos) const;

  entt::entity get_chunk(Sint64 x, Sint64 y, Sint64 z) const
  { return get_chunk(ChunkPos{x, y, z}); }


  // Block retrieval functions
  BlockID get_block(const ChunkPos& chunk_pos,
                    int local_x, int local_y, int local_z) const;
  
  BlockID get_block(Sint64 global_x, Sint64 global_y, Sint64 global_z) const;


  // Block setting functions
  bool set_block(const ChunkPos& chunk_pos,
                 int local_x, int local_y, int local_z,
                 BlockID block_id);

  bool set_block(Sint64 global_x, Sint64 global_y, Sint64 global_z,
                 BlockID block_id);

}; // class WorldMap

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_WORLDMAP_HPP