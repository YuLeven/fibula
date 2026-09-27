# Agent feature index

Start with [root instructions](../../AGENTS.md), then the scoped `AGENTS.md` for the
files being changed. These feature guides are explicitly referenced task context;
they do not rely on automatic discovery outside their directories.

## Read by task

| Task | Guide |
| --- | --- |
| Understand startup and ownership | [Architecture](architecture.md) |
| Change team assignment, score, spawns, match lifecycle, or add a mode | [Game modes](game-modes.md) |
| Add/tune spells, damage, healing, targeting, or control | [Combat and balance](combat-and-balance.md) |
| Change equipment, loot, rewards, death, or progression | [Inventory and progression](inventory-and-progression.md) |
| Change login, characters, server discovery, or persistence | [Backend contracts](backend-contracts.md) |
| Change HUD, action bars, settings, maps, or web pages | [UI and content](ui-and-content.md) |
| Add tests or CI, or determine what a green build proves | [Testing and CI](testing-and-ci.md) |
| Change network/timer/query hot paths or server capacity | [Scalability](scalability.md) |
| Check assumptions about incomplete or inconsistent behavior | [Known gaps](known-gaps.md) |

## Reading the guides

The initial review examined source at `861db51`. **Current behavior** describes
checked-in C++/Elixir/configuration, not a successful runtime test. Blueprint and map
internals were not inspected in Unreal Editor. **Required practice** describes the
project goals for future changes. **Planned coverage** is not an existing test job.

The product goals are engaging Tibia-styled PvP, competitive teams, extensible
features, useful CI confidence, and roughly 40 players per server. Existing constants
are implementation facts, not proof that the current design meets those goals.

Keep documentation close to code: update relevant guides when changing behavior,
retain source paths and function names as navigation anchors, and remove known-gap
entries only with a fix and appropriate validation. Do not copy the complete item or
spell catalog here; the definitions remain the source of truth for individual values.
