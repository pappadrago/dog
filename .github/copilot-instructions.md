# Copilot instructions for this repository

## Project overview

This repository is a Game Boy Advance platformer built with the Butano C++ game engine. The codebase is organized around a few large gameplay systems rather than a framework-heavy app structure:

- `src/main.cpp` is the orchestration entry point. It initializes Butano, shows the intro, then creates the global managers, loads a schema, and runs the main loop for each level.
- The gameplay object layer is split across `include/*.h` and `src/*.cpp`: `dog`, `enemy`, `item`, `door`, `bau`, `cassa`, `piattaforma`, `scala`, `trampolino`, `boss`, and the HUD/healthbar systems.
- `include/megadog_s1.h` and `include/megadog_s2.h` hold each stage's `constexpr` entity definitions, collision grid, and `schema_def`; `src/schemi.cpp` exposes them through the schema accessors. `tiled/` contains the editable maps. Collision grids use 16-pixel tiles, with values 0 (empty), 1 (solid), and 2 (one-way platform).
- `src/platform_map.cpp` copies a stage background map into writable storage for in-game platform changes; this is separate from the runtime collision-grid copy managed by `schemi.cpp`.
- Global runtime state lives in `globals.h` / `globals.cpp` (`g_camera`, `g_dog`, `g_enemies`, `g_items`, `g_schema`, etc.) and is intentionally treated as a set of shared singletons for the active run.
- Art and audio assets live under `graphics/`, `audio/`, and `tiled/`; they are consumed through Butano-generated asset headers and names (`bn::sprite_items::*`, `bn::regular_bg_items::*`).
- `main.cpp` owns the run and level lifecycles: it emplaces shared managers for a run, loads collision data before spawning level objects, updates gameplay in a fixed per-frame sequence, then destroys level objects and resets run state between lives/attempts. Preserve these dependency and teardown orders when changing initialization.
- The runtime uses fixed-capacity `bn::vector` containers and explicit limits for gameplay objects; keep new capacity and memory use appropriate for GBA hardware.

The project builds as a GBA ROM, not as a host-side binary or web app. Most changes are C++ gameplay code plus asset definitions or level schema data.

## Build, test, and lint commands

### Primary build

```bash
make -j$(nproc)
```

This builds the GBA ROM. The repo’s `Makefile` expects Butano at `../butano/butano` and the devkitARM toolchain to be available. The build also processes assets in `graphics/` and `audio/` into generated Butano headers.

### Cleaning artifacts

```bash
make clean
```

### Tests and lint

There is no repo-local test suite, single-test command, or lint target. The practical validation path is a successful ROM build; `.github/workflows/build.yml` builds with the devkitPro container and uploads the generated `.gba` ROM. The separate `.github/workflows/makefile.yml` refers to `configure`, `make check`, and `make distcheck`, which are not provided by this project’s Makefile.

## Key conventions

- Follow the existing object ownership pattern. The project uses a mix of global optional singletons (`g_*`) and heap-allocated gameplay objects (`new enemy(...)`, `new door(...)`, etc.) created in `main.cpp` and cleared alongside level transitions. Avoid introducing separate global registries unless the existing pattern is clearly extended.
- Level content is data-driven. New stage content is usually added by updating the relevant schema definitions (via `get_schema_*` accessors and `schema_def`) rather than hardcoding special cases in the main loop. Keep object placement, collisions, and background metadata in the schema data; when changing collision or entity behavior, check both the object implementation and its schema data.
- The codebase uses localized naming in several areas (`dog`, `piattaforma`, `scala`, `trampolino`, `cassa`, `boss`) and direct Butano APIs (`bn::camera_ptr`, `bn::random`, `bn::regular_bg_ptr`, etc.). Match the local naming style when editing these modules.
- The project is intentionally split by gameplay responsibility rather than by feature “layer” only. If you add a new behavior, it usually belongs next to the related object type (`dog.*`, `enemy.*`, `door.*`, etc.) and may also require updates to the schema data and the main update flow.
- The `main.cpp` flow resets and recreates per-level runtime state in a specific order: camera, environmental state, item/enemy/collision managers, then the player and stage-specific objects. Preserve that lifecycle when editing level startup or teardown logic.
- Art and tile data is generated through Butano; asset references are compile-time generated headers. If you add or rename generated assets, keep names consistent with the generated item names and the includes that consume them.
- Some performance-sensitive implementations are in paired `*.iwram.cpp` files and use `BN_CODE_IWRAM`; keep those definitions aligned with their corresponding class declarations and normal source files.
