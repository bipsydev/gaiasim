/**
 * @file WorldMap.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Contains the 3D voxel-based world based on EnTT chunk entities.
 *
 * Uses EnTT to allocate and deallocate chunks of voxels, and then passes
 * world generation off to `WorldGenerator` (To be implemented).
 * Defines the positional coordinate system as components within
 * the EnTT registry.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-27
 *
 * @copyright Copyright (c) 2026
 *
 * @section License
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 **/
#pragma once
#ifndef BIPSY_GAIASIM_WORLDMAP_HPP
#define BIPSY_GAIASIM_WORLDMAP_HPP


// #region Library Includes

#include "biputils/formatter.hpp"  // FORMATTER macro

#include "entt/entt.hpp"           // entt::registry

#include "SDL3/SDL_stdinc.h"       // Sint64 (named integer type)

#include <array>                   // std::array
#include <unordered_map>           // std::unordered_map
#include <limits>                  // std::numeric_limits
#include <functional>              // std::hash
#include <tuple>                   // std::tuple
#include <concepts>                // std::integral


// #endregion

namespace bipsy::gaiasim
{


// #region BlockID Enumeration Definition

// Use 64-bit unsigned integers for block IDs
// using BlockID = Uint64;
enum BlockID : Uint64
{
  AIR = 0,
  GROUND,
  // ... more types here ...
  INVALID = std::numeric_limits<Uint64>::max()
};


// #endregion
// #region Coordinate Abstraction: Positional 3D local & global coordinate systems

// Abstract base class for all 3D position types
// ensure CoordType is an integer type using concepts (for hashing and indexing)
template <std::integral CoordType>
struct Position3D
{
  CoordType x = 0;
  CoordType y = 0;
  CoordType z = 0;

  Position3D(CoordType x = 0, CoordType y = 0, CoordType z = 0)
  : x{x}, y{y}, z{z}
  {}

  bool operator ==(const Position3D & other) const
  { return x == other.x && y == other.y && z == other.z; }

  virtual ~Position3D() = 0;  // makes it an ABC
};  // struct Position3D
// define template implementation for pure virtual destructor (even though = 0)
template <std::integral CoordType>
Position3D<CoordType>::~Position3D()
{}


// Global world position (positive or negative, theoretically unbounded)
struct GlobalPos : public Position3D<Sint64>
{
  GlobalPos(Sint64 x = 0, Sint64 y = 0, Sint64 z = 0)
  : Position3D{x, y, z}
  {}
};


// Chunk-Local position within a chunk (0 to SIZE - 1 in each dimension)
struct LocalPos : public Position3D<Uint8>
{
  LocalPos(Uint8 x = 0, Uint8 y = 0, Uint8 z = 0)
  : Position3D{x, y, z}
  {}
};


// #endregion
// #region EnTT Components


// A position in chunk-coordinates (also part of the coordinate system)
struct ChunkPos : public Position3D<Sint64>
{
  ChunkPos(Sint64 x = 0, Sint64 y = 0, Sint64 z = 0)
  : Position3D{x, y, z}
  {}

  // For use in unordered_map, we need to define a hash function
  // as a separate class in order to pass as template argument to unordered_map
  class Hash
  {
  public:
    std::size_t operator ()(const ChunkPos & pos) const
    {
      // Combine the hash of x, y, and z using a common technique
      std::size_t h1 = std::hash<Sint64>{}(pos.x);
      std::size_t h2 = std::hash<Sint64>{}(pos.y);
      std::size_t h3 = std::hash<Sint64>{}(pos.z);
      return h1 ^ (h2 << 1) ^ (h3 << 2);  // Simple combination
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


// #endregion


/**
 * @brief Represents the 3D voxel-based world. Accesses Blocks via Chunks.
 *
 * Uses EnTT to hold Chunks as Entities
 *
 */
class WorldMap
{

  // #region Using Types

  // pass hashing function for ChunkPos to unordered_map
  using ChunkMap = std::unordered_map<ChunkPos, entt::entity, ChunkPos::Hash>;
  using ChunkMapConstIterator = typename ChunkMap::const_iterator;


  // #endregion
  // #region Private Data Members

  // Holds Chunks as Entities
  entt::registry m_chunk_registry;
  // 3D integer coordinates -> chunk entity
  ChunkMap m_chunk_map;


  // #endregion

public:

  // #region Constructor & Destructor

  WorldMap();

  ~WorldMap();


  // #endregion
  // #region ECS Registry & Component Getters

  entt::registry &       registry() { return m_chunk_registry; }

  const entt::registry & registry() const { return m_chunk_registry; }

  template <typename Component>
  Component & get_component(entt::entity entity)
  { return m_chunk_registry.get<Component>(entity); }


  // #endregion
  // #region Chunk Operations

  // Chunk creation functions
  entt::entity create_chunk(const ChunkPos & pos);

  entt::entity create_chunk(Sint64 x, Sint64 y, Sint64 z)
  { return create_chunk(ChunkPos{x, y, z}); }

  // Chunk deletion functions
  bool delete_chunk(const ChunkPos & pos);

  bool delete_chunk(Sint64 x, Sint64 y, Sint64 z)
  { return delete_chunk(ChunkPos{x, y, z}); }

  bool delete_chunk(entt::entity chunk_entity);


  // Chunk retrieval functions
  entt::entity get_chunk(const ChunkPos & pos) const;

  entt::entity get_chunk(Sint64 x, Sint64 y, Sint64 z) const
  { return get_chunk(ChunkPos{x, y, z}); }


  // #endregion

  // #region Block Operations
  // Block retrieval functions
  BlockID get_block(const ChunkPos & chunk_pos,
                    const LocalPos & local_pos) const;

  BlockID get_block(const ChunkPos & chunk_pos,
                    Uint8            local_x,
                    Uint8            local_y,
                    Uint8            local_z) const
  { return get_block(chunk_pos, LocalPos{local_x, local_y, local_z}); }

  BlockID get_block(const GlobalPos & global_pos) const
  { return get_block(global_pos.x, global_pos.y, global_pos.z); }

  BlockID get_block(Sint64 global_x, Sint64 global_y, Sint64 global_z) const;


  // Block setting functions
  bool set_block(const ChunkPos & chunk_pos,
                 const LocalPos & local_pos,
                 BlockID          block_id);

  bool set_block(const ChunkPos & chunk_pos,
                 Uint8            local_x,
                 Uint8            local_y,
                 Uint8            local_z,
                 BlockID          block_id)
  {
    return set_block(chunk_pos, LocalPos{local_x, local_y, local_z}, block_id);
  }

  bool set_block(const GlobalPos & global_pos, BlockID block_id)
  { return set_block(global_pos.x, global_pos.y, global_pos.z, block_id); }

  bool set_block(Sint64  global_x,
                 Sint64  global_y,
                 Sint64  global_z,
                 BlockID block_id);


  // #endregion
  // #region Coordinate Conversion Methods

  // Coordinate conversion functions: Global -> Chunk local
  static std::tuple<ChunkPos, LocalPos> convert_global_to_chunk_pos(
          Sint64 global_x, Sint64 global_y, Sint64 global_z
  );

  static std::tuple<ChunkPos, LocalPos> convert_global_to_chunk_pos(
          const GlobalPos & global_pos
  )
  {
    return convert_global_to_chunk_pos(global_pos.x,
                                       global_pos.y,
                                       global_pos.z);
  }

  // Coordinate conversion functions: Chunk local -> Global
  static GlobalPos convert_chunk_to_global_pos(Sint64 chunk_x,
                                               Sint64 chunk_y,
                                               Sint64 chunk_z,
                                               Uint8  local_x,
                                               Uint8  local_y,
                                               Uint8  local_z);

  static GlobalPos convert_chunk_to_global_pos(const ChunkPos & chunk_pos,
                                               const LocalPos & local_pos)
  {
    return convert_chunk_to_global_pos(chunk_pos.x,
                                       chunk_pos.y,
                                       chunk_pos.z,
                                       local_pos.x,
                                       local_pos.y,
                                       local_pos.z);
  }


  // #endregion

};  // class WorldMap


}  // namespace bipsy::gaiasim


// #region Formatting Compatibility: `std::formatter` Specialization

// must be defined in global namespace
REGISTER_FORMATTER_ENUM(bipsy::gaiasim::BlockID);


// #endregion

#endif  // BIPSY_GAIASIM_WORLDMAP_HPP
