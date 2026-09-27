# Working on Fibula

## Project direction

Fibula is an independent, Tibia-inspired 3D arena PvP game. This repository contains
the Unreal Engine client and dedicated server, plus the Phoenix service required
for login, characters, persistence, server discovery, and community pages.

Build engaging, Tibia-styled PvP around positioning, targeting, resource management,
and coordinated combat. Prioritize responsive play, server authority, and reliable
character data. Preserve the existing game design unless the task changes it.

- **Competitive balance:** assess changes across all four vocations, FFA and Team
  Battle, and both teams. Consider damage, healing, control, mobility, equipment,
  spawn access, and rewards together; equal headcounts alone do not prove balance.
- **Tests and CI:** meaningful behavior changes need regression coverage. The goal
  is for CI to demonstrate that players can join, fight, finish matches, and retain
  the intended data. Compilation and backend tests alone cannot establish this.
- **Extensibility:** extend existing definitions and mode hooks first. Separate
  reusable rules from actor/UI/HTTP side effects when the task benefits from it;
  avoid new frameworks or broad rewrites without a concrete need.
- **Capacity:** design and measure for roughly 40 active players per dedicated
  server, including a crowded team fight. This is a target, not a verified capacity
  claim; current mode limits differ. Treat CPU, replication, client rendering,
  database work, and persistence bursts as parts of the same workload.

The [agent feature index](docs/agents/README.md) maps implemented behavior, extension
points, validation expectations, and known gaps. Read only the relevant feature
guides for a task. Update them alongside behavior, contract, or workflow changes.

## Find the right code

| Area | Entry points | Additional guidance |
| --- | --- | --- |
| Gameplay, networking, UI | `Source/Fibula/`, `Source/*.Target.cs` | `Source/AGENTS.md` |
| Login, persistence, website | `fibula_site/lib/`, `fibula_site/test/` | `fibula_site/AGENTS.md` |
| Engine settings and assets | `Config/`, `Content/`, `Fibula.uproject` | Read `Source/AGENTS.md` for engine and asset changes |
| Build and operations | `build_server.sh`, `deploy_site.sh`, `backup_db.sh` | Read each script before execution |
| Game modes and balance | Mode/state classes, `GameFormulas.*`, spell/item definitions | [Modes](docs/agents/game-modes.md), [combat](docs/agents/combat-and-balance.md) |
| Testing and performance | `fibula_site/test/`, Unreal runtime paths | [CI roadmap](docs/agents/testing-and-ci.md), [40-player target](docs/agents/scalability.md) |

Read the applicable guide before editing. Keep shared guidance here and
component-specific instructions in the scoped files. Use `AGENTS.md` as the
canonical filename for repository agent instructions.

## Work efficiently

1. Inspect `git status --short --branch` and the relevant files first. Preserve
   existing user changes; do not reset, clean, or overwrite unrelated work.
2. State the intended result briefly. For multi-step work, outline a short plan;
   carry straightforward, reversible work through implementation and validation.
   Ask only when a missing decision materially affects scope or correctness.
3. Search narrowly with `rg` or `git ls-files`. Start in `Source/Fibula`, `Config`,
   or `fibula_site/lib` and `fibula_site/test`, rather than scanning binary assets,
   generated directories, or dependencies.
4. Trace the relevant caller, implementation, and tests before changing behavior.
   Follow nearby naming and formatting. Avoid unrelated cleanup, dependency
   upgrades, or whole-file reformatting.
5. Validate the changed behavior with the smallest useful checks, then expand
   when the change affects shared systems. Add regression coverage for meaningful
   behavior changes; documentation-only edits need no game build or database.
   For balance changes, include before/after scenarios and the affected matchups.
   For hot-path changes, include workload and performance evidence where available.
6. Review the final diff and run `git diff --check`. Report what changed, checks
   actually run, and any remaining limitation or manual verification step.
   Do not report unrun checks as passing.

When asked to sync, fetch and fast-forward the requested branch; inspect divergence
instead of discarding commits. Do not automatically change branches for every task.
Use `codex/` for new task branches unless another name is requested. Commit, push,
and deploy when the task authorizes those actions.

## Cross-component changes

- Treat the client, dedicated server, and Phoenix API as one end-to-end flow.
  For payload, authentication, item, stat, or version changes, inspect both the
  Unreal callers and Phoenix routes/controllers/schemas and update them together.
- Keep authoritative combat and inventory decisions on the server. Validate
  ownership and input at API/RPC boundaries; preserve authentication checks.
- Use the existing configuration entry points: `Source/Fibula/Config/ServerConfig.h`
  and `fibula_site/config/`. Keep credentials out of new code, logs, and fixtures
  that use real accounts; use local/test data for verification.
- Preserve stored character data and compatibility unless a migration or protocol
  change is explicitly part of the task. Explain any required rollout order.

## Files and operational boundaries

- Treat `.uasset` and `.umap` as binary Unreal assets. Use the Unreal Editor for
  changes and preserve the Git LFS rules in `.gitattributes`.
- Do not hand-edit generated outputs in `Binaries/`, `Intermediate/`, `Saved/`,
  `DerivedDataCache/`, `.vs/`, `fibula_site/_build/`, or `fibula_site/deps/`.
  Edit source assets in `fibula_site/assets/`, not digested web build outputs.
- `build_server.sh` clears a configured release directory and uploads via SFTP.
  `deploy_site.sh` builds, uploads, and replaces remote deployment contents.
  `backup_db.sh` connects to the remote database host. These are operational
  scripts, not routine local validation commands; run them only within a task
  that authorizes those effects and after checking their paths and destinations.
- When a required engine, runtime, database, or editor is unavailable, complete
  independent checks and state the exact validation that remains blocked.
