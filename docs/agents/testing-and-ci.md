# Testing and CI confidence

## What exists today

At the initial source review (`861db51`), `fibula_site/test/` contains 23 ExUnit test
files and 195 textual `test` declarations. This is an inventory, not a pass count or
a coverage percentage. The review did not execute the suite.

| Existing area | Test location relative to `fibula_site/test/` |
| --- | --- |
| Accounts/tokens/passwords and characters | `fibula_site/accounts_test.exs`, `fibula_site/character_test.exs` |
| Presence, freshness, cleanup, PubSub | `fibula_site/servers/` |
| Character creation/list/detail/delete, items, stats | `fibula_site_web/controllers/character*_test.exs` |
| Discovery/mode/version/capacity selection | `fibula_site_web/controllers/server_status_controller_test.exs` |
| Status submission/auth | `fibula_site_web/controllers/backend_server_contoller_test.exs` (filename spelling is current) |
| Browser auth/session/account LiveViews | `fibula_site_web/user_auth_test.exs`, user session/controller and `live/user_*` tests |
| Character page and basic HTML/JSON | `live/character_details_live_test.exs` under `fibula_site_web/`, plus controller tests |

The repository has a Phoenix test workflow at
`.github/workflows/backend-tests.yml` and native Unreal Automation tests under
`Source/Fibula/Tests/`. The 25 native cases include a real mode-initialized encounter:
three players join, the Knight targets and swings at an opponent, repeat melee damage
is applied, and the mode rejects an attack against an ally. Other interaction tests
route rune use and ally healing through character server actions and verify resulting
health, mana, inventory, death, score, and respawn state. These run in a local authority
world. A dedicated server bot smoke runner at `scripts/test_bots.ps1` exercises both
supported modes through two real Unreal network clients and verifies bot movement,
targeting, combat, and replicated damage. These checks do not cover Phoenix login or
character persistence, full match lifecycle, or 40-player capacity. Existing backend
tests alone do not demonstrate Unreal replication or Blueprint health.
### Local bot multiplayer smoke

With a UE 5.5 source engine available, run from the repository root:

    $env:FIBULA_UE_ROOT = 'D:\Path\To\UnrealEngine'
    .\scripts\test_bots.ps1
    .\scripts\test_bots.ps1 -Mode FFA

The script builds FibulaEditor, FibulaClient, and FibulaServer, starts the
Ankrahmun dedicated server, then joins two test clients. Both clients send movement
and targeting input toward opposing bots. The server reports PASS only after bots
acquire targets, issue movement and combat actions, make measured progress, and deal
bot-attributed health damage after staging. Both clients must report their inputs, and
at least one must observe replicated incoming damage. Logs are retained under the
local temporary directory and included on failure.

`-FibulaBotTest` is development-only. It creates disposable test characters, skips
the server status HTTP reporter, and suppresses item persistence; Shipping builds
do not include this test login path. The script uses no Phoenix account or production
credentials. Use -TimeoutSeconds to allow slower editor startup. This is a focused
bot smoke test, not a general match lifecycle, cross-service, or load test.
## Commands available now

From `fibula_site/`, with dependencies and test PostgreSQL configured:

```sh
mix format --check-formatted
mix compile
mix test test/fibula_site_web/controllers/server_status_controller_test.exs
mix test
mix assets.build
```

Use focused tests during iteration and the full backend suite for shared auth,
persistence, routing, and context changes. The `test` alias creates/migrates the test
database. [Test config](../../fibula_site/config/test.exs) uses `localhost` and does
not honor development's `DB_HOST`; configure containerized tests deliberately.
Do not hide unrelated baseline format/test failures with broad rewrites or skipped
assertions. See [test guidance](../../fibula_site/test/AGENTS.md).

The repository-wide `mix format --check-formatted` currently fails on existing
backend files. CI does not use it as a gate so its first run can validate compilation
and behavior without requiring unrelated formatting changes.

For Unreal gameplay changes, run the authority-world automation suite with a verified
UE 5.5 installation from the repository root:

```powershell
.\Scripts\run_unreal_tests.ps1 -EngineRoot 'D:\Game\UnrealEngine'
```

The runner builds `FibulaEditor`, runs every `Fibula.*` automation test headlessly,
and fails if the engine exits unsuccessfully or reports failed/not-run tests. Reports
and logs are written under ignored `Saved/Tests/`. For shared runtime changes, also
build affected Client/Server targets using [Source/AGENTS.md](../../Source/AGENTS.md).
The Unreal runners require a provisioned UE 5.5 engine and project assets; the hosted
backend workflow cannot substitute for them. The bot smoke runner above must be run
before claiming its dedicated-session behavior checks pass.
## Required test design for changes

- Add regression tests for gameplay, protocol, persistence, and rule changes. A bug
  fix should demonstrate the original failure and the intended behavior.
- Separate small deterministic rules (formulas, policies, selection, serialization)
  from integration scenarios (actors, replication, timers, HTTP, database, maps).
  Introduce minimal test seams rather than recreating game logic in tests.
- Make time and randomness controllable where practical. Use bounded condition waits
  for integration checks, not long arbitrary sleeps. Record seed and scenario inputs.
- Assert externally meaningful results: exactly-once scoring/consumption, no friendly
  fire, ownership rejection, item conservation, observer state, and persisted output.
- Include negative and boundary cases, lifecycle cleanup, duplicate/out-of-order
  events, and invalid client requests. One happy-path test cannot prove authority.
- Update the relevant feature guide and CI selection when introducing a feature.
  If infrastructure blocks validation, report the blocker and unverified scenarios.

## Planned CI layers (not implemented by this documentation)

| Layer | Intended trigger | What a passing job must demonstrate |
| --- | --- | --- |
| Backend | PRs affecting service/contracts; full suite on shared changes | Pinned dependencies, formatting, compile, migrations on isolated PostgreSQL, ExUnit, asset build |
| Unreal rules/build | PRs affecting source/config; build assets when relevant | Matching UE source toolchain, valid targets/reflection, deterministic gameplay/serialization tests |
| Dedicated multiplayer smoke | Gameplay, maps, networking, contracts; integrated main build | Server plus at least two actual network clients join and observe correct combat/state transitions |
| Cross-service smoke | Login/persistence/protocol changes | Local Phoenix + database + game clients complete login, character load, play, save, reconnect |
| Capacity/soak | Scheduled and on performance/capacity changes | Reproducible 40-player workload, frame/network/memory/database reports and behavior assertions |
| Content/package validation | Relevant asset/config changes and releases | Required maps/Blueprints load, referenced assets cook, packaged server/client launch |

CI needs a provisioned UE 5.5 source engine/toolchain and Git LFS assets. Establish
runner access and compatible caches explicitly; do not commit engine binaries or
invent a hosted runner with an engine already installed. Keep build/test jobs separate
from upload/deployment scripts. Service containers and test credentials must be local
to the run, with no production dependencies.

### Minimum multiplayer smoke acceptance

Cover FFA and Team Battle, introducing cases in achievable stages:

1. Start the intended arena/mode, connect two authenticated characters, verify both
   clients see the expected team, position, health, and identity.
2. Move/target/cast and auto-attack; verify costs/cooldowns, hostile damage and allied
   healing policy, observer updates, and protection-zone rejection.
3. Cause one controlled death; verify score/stats once, corpse/item behavior,
   respawn, target cleanup, and camera recovery.
4. Exercise score/time finish, draw, reward eligibility, disconnect cleanup, and a
   second round. Use a test-controlled clock/threshold seam instead of waiting 25 minutes.
5. Change inventory, disconnect/reconnect, and verify the expected durable snapshot.
   Include backend failure and retry/order cases when those mechanisms exist.

An actor-only simulation is useful for rules/load but is not proof of packet
replication to clients. Include real connections in the network smoke layer.

## Making CI understandable

Each run should report layer/scenario, mode, seed, map, engine revision, game commit,
player count, passed/failed/skipped totals, duration, and artifact locations. Preserve
test reports, client/server logs, crash data, and performance traces on failure.
Set timeouts, clean up spawned processes, and fail on missing expected reports or
zero discovered tests. Distinguish a skipped engine/load job from a passing one.

A green overall result should summarize which behavior was exercised and which
layers were unavailable. Do not label backend-only success as "the game works".
Balance changes also need explicit before/after scenarios and playtest evidence;
performance thresholds need the measured baseline described in [scalability](scalability.md).
