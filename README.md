# gaiasim

> A fantasy-realism life simulation survival game engine built in C++ using [SDL3](https://github.com/libsdl-org/SDL) for desktop, mobile, and web.

- **Goal**: Learn SDL3 and cross-platform graphics programming while developing a modular, hackable life simulator engine with options for hardcore survival.
- **Inspirations**:
  * **Base systems** inspired by Caves of Qud, Moonring, Dwarf Fortress, RimWorld, Project Zomboid, and Rogue.
  * **Default game world/objectives** inspired by Vintage Story, TerraFirmaCraft, Terraria, and Don't Starve.

### [Living Game Design Document](https://github.com/bipsydev/gaiasim/wiki/Game-Design-Document-(GDD))

### Progress to Next Milestones

[![GitHub milestone details](https://img.shields.io/github/milestones/progress-percent/bipsydev/gaiasim/1)](https://github.com/bipsydev/gaiasim/milestone/1)
[![GitHub milestone details](https://img.shields.io/github/milestones/progress-percent/bipsydev/gaiasim/2?color=%23f67)](https://github.com/bipsydev/gaiasim/milestone/2)
[![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/bipsydev/gaiasim)](https://github.com/bipsydev/gaiasim/issues)

---

## Technical Architecture

This project is in active early development. The current codebase establishes the engine foundation; game content, world generation, and the scripting system are next on the roadmap.

### Engine Core (`Game`, `Screen`)

The `Game` class manages the full SDL3 application lifecycle and owns a collection of `Screen` objects — an abstract base interface representing discrete application states (main menu, gameplay, loading, settings, etc.). Screens are allocated via a templated `add_screen<T>()` factory method and activated through a `switch_screen()` mechanism that calls `hide()`/`show()` lifecycle hooks on transition.

Initialization is staged using an `InitRequest` enum with ordered dependency semantics: `LIBRARIES → SYSTEM_OBJECTS → GAME_STATE`, so partial initialization is safe and explicit. Frame timing, delta time, and FPS are tracked at the `Game` level and exposed to all screens.

### Voxel World (`WorldMap`, `ChunkData`)

The world is represented as an infinite 3D grid of **chunks**, each a 16×16×16 flat array of `BlockID` values. Chunks are managed as **EnTT entities** with components:

- `ChunkPos` — 64-bit signed integer chunk coordinates (supports negative-space worlds)
- `ChunkData` — flat `std::array<BlockID, 4096>` for cache-friendly block access
- `BlockEntities` — sparse `unordered_map` for blocks that carry metadata (ECS entities)

A `ChunkPos::Hash` functor enables O(1) average-case chunk lookup via `std::unordered_map`. A `static_assert` at compile time ensures `ChunkData::SIZE³` never overflows the 16-bit block entity index.

**Coordinate system**: `GlobalPos` (unbounded signed 64-bit) ↔ `ChunkPos` + `LocalPos` (0–15 per axis) conversion is implemented correctly for negative coordinates using floor-division semantics — a common correctness pitfall in voxel engines.

### C++ Highlights

- **Concepts**: `Position3D<CoordType>` is constrained with `std::integral` to enforce integer coordinate types at compile time
- **Structured bindings**: used throughout for clean tuple unpacking (e.g. coordinate decomposition)
- **Perfect forwarding**: `add_screen<T>(Args&&...)` forwards constructor arguments with zero unnecessary copies
- **RAII**: SDL objects and textures are managed with explicit lifetime tied to owning class destructors
- **Logging**: a custom indented SDL logging utility (`SDL_utils.hpp`) wraps SDL's log system with variadic templates and indent-level formatting

### Planned Systems (In Active Design)

- **World generation** — procedural terrain algorithms operating on the `WorldMap` chunk structure
- **Save/load system** — serialization of world state and game progress
- **Scripting language** — custom lexer, parser, and tree-walk interpreter for an embedded domain-specific language; will drive world generation scripts, NPC behavior, and entity definitions
- **ECS entity system** — full player, NPC, and item entities built on EnTT alongside the existing chunk entity model

---

## Building & Debugging

### Desktop

```bash
# Clone the repo (with submodules)
git clone --recurse-submodules https://github.com/bipsydev/gaiasim
cd gaiasim
# Configure
cmake -S . -B build
# Build a debug executable
cmake --build build --config Debug --target all
# Run with GNU debugger
gdb ./build_output/Debug/gaiasim
```

Or load the project into VS Code and use the CMake plugin to configure automatically, then run a debug build with `Shift+F5`.

### Android

`CMakeLists.txt` contains configuration for Android builds alongside other platforms. The `android_debug.sh` script builds an APK via Gradle (which calls the CMake config), installs it to a connected ADB device, and streams `adb logcat` output until the app terminates.

See [SDL's `README-android.md`](https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md) for more on Android debugging.

---

## File Structure

| Path | Description |
|---|---|
| `src/` | Main C++ source files, shared across all platforms |
| `include/` | C++ header files |
| `assets/` | Static resources (images, fonts, sounds, etc.) |
| `external_libs/` | External libraries as Git submodules |
| `android-project/` | Android project wrapper; bundles native code into an APK |
| `build/` | CMake intermediate build files |
| `build_output/` | Final built executable or APK, organized by platform |

### Scripts

| Script | Description |
|---|---|
| `android_init.sh` | Initializes `android-project/` from the SDL template |
| `android_debug.sh` | Builds APK, installs via ADB, streams logcat output |

---

## SDL3 Resources

- [SDL with CMake](https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-cmake.md)
- [Android build guide](https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md)
- [Ravbug/sdl3-sample](https://github.com/Ravbug/sdl3-sample)

---

## Asset Credits

`PixelCode.ttf` — PixelCode Font v2.2 — [qwerasd205/PixelCode](https://github.com/qwerasd205/PixelCode)
