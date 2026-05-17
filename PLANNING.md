# Gaiasim Project Planner


## Current Tasks

- [x] Create `Screen` abstract interface for loading different screens (with their own init/deinit, update/render, events, etc).

- [x] Create system for loading and presenting `Screen`s in the `Game` implementation code.

- [x] Move example screen (gradient squares) into an `ScreenTest` class.

- [ ] Create an initial game screen (shows the default interaction GUI).


## Planned Tasks

- Fix the `android_init.sh` script to preserve project-specific files and only copy over the template files we don't have.
  - Maybe give it a `--clean` option or something like that?
- Remove system GUI (taskbar and home button, etc) from Android app.
  - use Android's Immersive Mode in a subclass of `SDLActivity.java`? probably!

- Create basic 3D voxel world and render basic example with some varying heights.

- Create the Scenario loader and generate a World save file from a Scenario file.

