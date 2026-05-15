# Gaiasim - Game Design Document (GDD)
__Version 0.0.1a-prerelease__

> A fantasy-realism hardcore-or-cozy __life survival simulator__ with RPG and modular elements.

*Project Start Date*: May 4th, 2026

*GDD Start Date*: May 15th, 2026

*Version 0.0.1a Projected Release*: June 15th, 2026

### Platforms
- __PC__: Linux, Windows, ~~Mac (maybe)~~
- __Mobile__: Android, ~~iOS (maybe, we'll see if we can)~~
- ~~Web: HTML5 (possible, but not a high-priority target)~~


## High Concept
*Gaiasim* is a __life simulation engine__ that can load __modular simulation scenarios__. This engine creates a 3D voxel-based world from a simulation scenario full of biomes, structures, entities, NPCs, quests, history, and environmental/social systems. You can then select to live as an entity in the world, control a group, or watch it simulate with minimal interaction from a distance.

*Gaiasim*'s base simulation scenario, called __*Gaia*__, is a familiar earthly world with a hidden magical heart. You'll begin in the wilderness attempting to create shelter with realistic natural progression systems. In order to survive the harsh winter, you'll need to master farming, hunting, prospecting, and more. Far from the player's origin in *Gaia* will be sparse towns and cities, full of NPCs that will help guide the player towards hidden underground dungeons.

The *Gaia* scenario is highly customizable. You can create a world that would be difficult and unforgiving, or you can start and stay in a safe cozy town with no worries of survival. Scenarios such as *Gaia* can be extended upon to create modified scenarios, or custom scenarios from scratch can also be created. *Gaiasim* allows for the easy addition of a single item or for a complete overhaul of the game!

## Core Gameplay

### Perspective/Interface
__Retro GUI terminal interface__, with a few different perspectives the user can switch between:
- __Top-Down__ *(Default)*: Top-level voxels and items are displayed. Best for general/tactical navigation.
  - `x-axis`: West-East
  - `y-axis`: North-South
  - `z-axis`: Up-Down
- __Side-View__: Side-scrolling style of view, where a cross-section is displayed. Best for combat/cutscenes.
  - `x-axis`: Backwards-Forwards
  - `y-axis`: Up-Down
  - `z-axis`: Left-Right
- __First-Person__ *(Planned, may not happen)*: A first-person view from a selected entity (usually the one being controlled by the user).
- __Unique System-Work Views__: These perspectives are varied based on how an entity works or interacts with a system. This could be a tool-binding system view that attaches handles to tool heads, or it could be an metalworking alloying system view for mixing liquid metals together.

### Main Mechanics
The game engine presents the interface and world to the user, and based on the __user interaction mode__ and __world difficulty configuration__ there are a few main ways to play:
- __Interaction Modes__: One must be active at any time, with options to switch between them during play if desired.
  1. __Role-Playing__ *(Single-Entity)*: Fully control a single entity, like a typical RPG game. Control where you go and what you do at every moment. Can only perceive what is around you.
  2. __Party__ *(Multi-Entity)*: Lead a group of semi-autonomous entities. Commands can be given and they'll do their best to follow them. Can only perceive what the entities in your party can see. Similar to Dwarf Fortress or The Sims.
  3. __Free Sim__ *(No-Entity)*: No direct control over any entity, but everything is visible and the entire map can be freely viewed.
- __World Difficulty Configuration__: Any of these can be enabled/disabled, or modified in value.
  - __Survival Systems__: Hunger, Thirst, Stamina, etc.
    - These can kill you or limit your abilities in some way.
  - __RPG Systems__: Attack, Defense, Magic, Wisdom, Luck, etc.
    - These modify combat to be more complex.
  - __Society Frequency__: How frequent towns/cities are spawned in the world.
    - Determine area of increased safety from the wilderness.
  - __Creature Frequency__: How frequent prey or predator creatures are found in the wilderness.
    - Determines the chance of finding food from creatures or hostile encounters.
  - __Environmental Harshness__: How dangerous the Summer and Winter are to the unprepared.
    - Determines chance of dying from heat stroke or hypothermia.


## *Gaia* Scenario *(Default game)*
...Detailed description of the default *Gaia* scenario...


## Engine Implementation Details
Main engine is implemented using SDL3 for cross-platform rendering and loading of scenarios and save files.
- `Game` class holds all the main game engine state & logic:
  - Initialization/Deinitialization
  - Update/Render
  - Event handling
  - Starting the main menu screen
    - Allow the user to select a Scenario to load and begin the simulation.

...More description here...

