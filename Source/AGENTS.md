# Unreal client and dedicated server

Read the root `AGENTS.md` first. This guide covers `Source/` and should also be
consulted for changes to `Config/`, `Content/`, and `Fibula.uproject`.

## Choose the feature guide

The game module is currently flat; feature boundaries do not correspond to C++
subdirectories. Use the relevant guide rather than loading every feature document:

- Mode lifecycle, teams, spawning: [game modes](../docs/agents/game-modes.md).
- Spells, health, control, targeting: [combat and balance](../docs/agents/combat-and-balance.md).
- Gear, loot, death, progression: [inventory and progression](../docs/agents/inventory-and-progression.md).
- Login, discovery, persistence: [backend contracts](../docs/agents/backend-contracts.md).
- HUD, input, Blueprints: [UI and content](../docs/agents/ui-and-content.md).
- Validation: [testing and CI](../docs/agents/testing-and-ci.md),
  [scalability](../docs/agents/scalability.md), and relevant
  [known gaps](../docs/agents/known-gaps.md).

For each gameplay extension, identify server-owned state, validation boundaries,
client presentation, reset/cleanup behavior, balance impact, and regression cases.
Prefer definition-driven additions and existing virtual mode hooks. Extract small,
testable rules when needed without moving unrelated code out of `FibulaCharacter`.

## Architecture and conventions

- The project targets a source-built Unreal Engine 5.5. Targets are `Fibula`,
  `FibulaEditor`, `FibulaClient`, and `FibulaServer`; their definitions live in
  `Source/*.Target.cs`. Module dependencies live in `Fibula/Fibula.Build.cs`.
  Keep dedicated-server compatibility and the existing Iris setup intact.
- Start with `FibulaCharacter.*`, `FibulaPlayerController.*`, and the relevant
  game mode/state for player behavior; `SpellSystem.*` and `GameFormulas.*` for
  combat; `ItemContainer.*` and item/equipment types for inventory.
- Login and backend integration entry points include `AuthTokenManager.*`,
  `ServerStatusReporter.*`, and `Config/ServerConfig.h`. Search callers and
  Phoenix controllers before changing URLs, JSON fields, credentials, or versions.
- Follow the local Unreal naming, reflection, and ownership patterns. Keep a
  header's `.generated.h` as its last include. Reflect UObject references when
  required for garbage collection and respect actor/component lifetimes.
- For replicated state, check authority, RPC ownership and input validation,
  replication registration, and `OnRep` behavior together. Clients must not become
  authoritative for damage, rewards, inventory, or persistent character stats.
- Keep visual/audio work safe on headless servers. Avoid unnecessary per-frame
  work, synchronous network calls, and new reliable RPCs on hot gameplay paths.
- Blueprint-visible class/property/function renames may break binary content.
  Inspect references and plan asset updates or redirects when such changes are
  needed. Do not rewrite assets with text tools or regenerate unrelated assets.

## Build and verify

Locate the installed source-built engine first; `Fibula.uproject` contains a local
engine association, not a portable engine path. Do not change it just to fit the
current machine. Run commands from the repository root.

For a Windows source-engine installation, set `$ueRoot` to its verified absolute
path, then use the engine build wrapper:

```powershell
$projectFile = Join-Path (Get-Location) 'Fibula.uproject'
& "$ueRoot/Engine/Build/BatchFiles/Build.bat" FibulaEditor Win64 Development "-Project=$projectFile" -WaitMutex
```

For shared runtime or networking changes, also build the affected `FibulaClient`
and `FibulaServer` targets using the same command with the target name replaced.
Inspect the exit code and log for each build. Use the corresponding engine wrapper
on other platforms. Do not use the root upload script as a local compile check.

Use existing automated coverage where available. For multiplayer changes, verify
the affected flow with a dedicated server and at least two clients when the runtime
is available: check both the owning and observing client, server authority, and
replication. For persistence changes, include login and reconnect behavior with a
local Phoenix service. For UI/content changes, inspect the result in the editor or
client. Record what was tested and what still requires a manual run.

Full cooking and packaging are appropriate for packaging-specific changes or an
explicit release task; they are not the default check for every C++ edit.
