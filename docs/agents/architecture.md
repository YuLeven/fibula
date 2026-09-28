# Runtime architecture

## Ownership and startup

| Layer | Owns | Source anchors |
| --- | --- | --- |
| Unreal client | Input, login UI, action bars, camera, local settings, rendering replicated state | `Source/Fibula/LoginWidget.*`, `FibulaHUD.*`, `StatusWidget.*`, `FibulaCharacter.*` |
| Dedicated server | Combat decisions, actors, teams, match state, runtime inventory and progression | `FibulaGameMode.*`, `FibulaGameState.*`, `FibulaCharacter.*`, `SpellSystem.*` in `Source/Fibula/` |
| Phoenix | Accounts, session tokens, stored items, highscores, server directory, community UI | `fibula_site/lib/fibula_site/`, `fibula_site/lib/fibula_site_web/` |
| PostgreSQL | Durable account/character/equipment/inventory/highscore/server records | `fibula_site/priv/repo/migrations/` |

[DefaultEngine.ini](../../Config/DefaultEngine.ini) starts clients at
`/Game/Maps/LoginScreen`, defaults servers to `/Game/Maps/Ankrahmun`, and uses
`FibulaLoginGameMode` / `FibulaTeamBattleGameMode` respectively. The configured travel
map is `/Game/Maps/TravelMap`. Map Blueprint overrides still require editor inspection.

`AFibulaLoginGameMode::BeginPlay` creates `WBP_LoginWidget` with UI-only input.
`AFibulaGameMode` initially uses a spectator pawn; after a character HTTP response,
it spawns `/Game/Characters/BP_ThirdPersonCharacter`, restores items, possesses the
pawn, assigns a team/vocation, moves to a start, and initializes combat stats.

`AFibulaGameState` initializes item/spell databases and spawns the chat and spell
systems on authority. Clients start battle music. `AFibulaPlayerState` holds player
vocation information; the character carries most replicated gameplay state. Team
Battle adds replicated team scores and start time through its game-state subclass.

Arena bots are server-owned AIController pawns created by the active game mode. They
use the normal replicated character and combat rules, but do not authenticate with
Phoenix or persist inventory. The local bot smoke test uses disposable loopback
players in Development builds only.

## Player lifecycle

1. Register/log in through Phoenix; the client stores a Base64 session token in the
   `AuthData` SaveGame slot and fetches that user's character list.
2. Select character and mode. The login UI polls the directory for a matching server.
3. Travel with `PlayerName` and `Token` options. The dedicated server loads the
   character through Phoenix, except for the existing-character reconnect path.
4. Inputs flow through character RPCs into server rules. Replicated properties,
   `OnRep` handlers, client RPCs, and cosmetic actors update the clients.
5. Death updates runtime stats, handles item loss/corpse creation, and respawns the
   same character actor. A camera delay is distinct from the server respawn timing.
6. Server reporting updates discovery and highscores. Actor destruction posts an
   inventory snapshot. Combat logout can retain an unpossessed actor temporarily.

See [backend contracts](backend-contracts.md) for exact routes, delivery limitations,
and the reconnect ownership gap. Persistent database skill fields do not mean that
the current login path restores those skills into the match.

## Configuration and assets

- [ServerConfig.h](../../Source/Fibula/Config/ServerConfig.h) provides game address,
  login/backend addresses, backend credentials, and the reported game version.
- [DefaultGame.ini](../../Config/DefaultGame.ini) controls project/package settings;
  [DefaultInput.ini](../../Config/DefaultInput.ini) and Enhanced Input assets
  participate in input setup. Check C++ bindings and Blueprint assignments together.
- The target files and [Fibula.Build.cs](../../Source/Fibula/Fibula.Build.cs) use UE
  5.5 settings and Iris. A locally source-built engine is required by the documented
  build workflow; the project engine GUID is machine-specific.
- The two Phoenix endpoints run under one supervision tree with Repo, PubSub,
  Finch, and `ServerCleanup`. Read `config/dev.exs`, `test.exs`, and `runtime.exs`
  for environment-specific behavior; local defaults are not production policy.

## Extension rules

Keep gameplay decisions on authority and presentation on clients. Reuse mode policy
hooks, data definitions, Phoenix contexts, and existing test fixtures. When adding
state, define its owner, replication/persistence shape, lifetime, reconnect behavior,
and reset behavior. When adding a new feature, document its entry points and critical
tests in the feature index; avoid a second independent authority for the same rule.
