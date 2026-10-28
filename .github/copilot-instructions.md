# Copilot instructions for this repository

## Project overview

This repository is a Game Boy Advance platformer built with the Butano C++ game engine. The codebase is organized around a few large gameplay systems rather than a framework-heavy app structure:

- `src/main.cpp` is the orchestration entry point. It initializes Butano, shows the intro, then creates the global managers, loads a schema, and runs the main loop for each level.
- The gameplay object layer is split across `include/*.h` and `src/*.cpp`: `dog`, `enemy`, `item`, `door`, `bau`, `cassa`, `piattaforma`, `scala`, `trampolino`, `boss`, and the HUD/healthbar systems.
- Level layout and collision data are centralized in `schemi.h` / `schemi.cpp` and the schema-specific headers (`include/megadog_s1.h`, `include/megadog_s2.h`). Schemas define doors, enemies, items, collisions, parallax backgrounds, and tile maps for each stage.
- Global runtime state lives in `globals.h` / `globals.cpp` (`g_camera`, `g_dog`, `g_enemies`, `g_items`, `g_schema`, etc.) and is intentionally treated as a set of shared singletons for the active run.
- Art and audio assets live under `graphics/`, `audio/`, and `tiled/`; they are consumed through Butano-generated asset headers and names (`bn::sprite_items::*`, `bn::regular_bg_items::*`).

The project builds as a GBA ROM, not as a host-side binary or web app. Most changes are C++ gameplay code plus asset definitions or level schema data.

## Build, test, and lint commands

### Primary build

```bash
make -j$(nproc)
```

This is the main compile step for the ROM. The repo’s `Makefile` expects the Butano engine to be available as a sibling directory (`../butano/butano`) and relies on the devkitARM toolchain.

### Cleaning artifacts

```bash
make clean
```

### Test and lint status

There is no repo-local unit test suite or lint command in this project. The practical validation path is a successful ROM build; CI in `.github/workflows/build.yml` compiles the project with the devkitPro container and uploads the generated `.gba` ROM.

If you need a focused validation pass while iterating, prefer targeted rebuilds with `make -j$(nproc)` rather than larger, unrelated churn.

## Key conventions specific to this codebase

- Follow the existing object ownership pattern. The project uses a mix of global optional singletons (`g_*`) and heap-allocated gameplay objects (`new enemy(...)`, `new door(...)`, etc.) created in `main.cpp` and cleared alongside level transitions. Avoid introducing separate global registries unless the existing pattern is clearly extended.
- Level content is data-driven. New stage content is usually added by updating the relevant schema definitions (via `get_schema_*` accessors and `schema_def`) rather than hardcoding special cases in the main loop. Keep object placement, collisions, and background metadata in the schema data.
- The codebase uses localized naming in several areas (`dog`, `piattaforma`, `scala`, `trampolino`, `cassa`, `boss`) and direct Butano APIs (`bn::camera_ptr`, `bn::random`, `bn::regular_bg_ptr`, etc.). Match the local naming style when editing these modules.
- The project is intentionally split by gameplay responsibility rather than by feature “layer” only. If you add a new behavior, it usually belongs next to the related object type (`dog.*`, `enemy.*`, `door.*`, etc.) and may also require updates to the schema data and the main update flow.
- The `main.cpp` flow resets and recreates per-level runtime state in a specific order: camera, environmental state, item/enemy/collision managers, then the player and stage-specific objects. Preserve that lifecycle when editing level startup or teardown logic.
- Art and tile data is generated through Butano; asset references are compile-time generated headers. If you add or rename generated assets, keep names consistent with the generated item names and the includes that consume them.

## Practical guidance for changes

- Prefer small, targeted gameplay edits in the relevant object file and header instead of rewriting the whole level lifecycle.
- When changing collision, movement, or enemy behavior, check both the object implementation and the schema metadata that spawns it.
- Keep new code aligned with the repo’s direct engine usage; this project is intentionally low-level and performance-sensitive, so avoid introducing unnecessary abstraction layers.
