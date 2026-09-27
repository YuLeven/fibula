# Unreal configuration changes

Read the root and `Source/AGENTS.md`, then the relevant
[feature guide](../docs/agents/README.md).

- `DefaultEngine.ini` owns default maps/game modes, mode aliases, renderer and
  collision settings. Map/alias changes must agree with server launch, login travel,
  discovery mode names, and cooked assets.
- `DefaultGame.ini` owns project version and packaging settings. Coordinate game
  compatibility versions with `Source/Fibula/Config/ServerConfig.h`; do not assume
  the project version automatically controls the directory's version comparison.
- Inspect `DefaultInput.ini`, C++ bindings, and Enhanced Input/Blueprint assets
  together. Collision/profile changes affect targeting, protection zones, walls,
  projectiles, and looting; check both teams and client/server behavior.
- Keep edits narrow. Do not commit machine-specific engine associations, generated
  editor settings, credentials, or unrelated settings produced by opening the editor.
- Validate relevant config paths/aliases and run map/load, multiplayer, or package
  checks when behavior warrants them. A text-only diff check cannot prove a map
  loads or a collision profile behaves correctly.
- For performance tuning, record reference hardware/map/build and before/after
  measurements using [the 40-player plan](../docs/agents/scalability.md).
