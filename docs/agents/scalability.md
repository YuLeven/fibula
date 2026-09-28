# Roughly 40 players per dedicated server

## Target and current limits

The supported design workload should be about 40 active players, including a
concentrated team fight and joins/leaves during play. Current C++ mode caps are 44
for base/FFA and 24 for Team Battle. Recommendation only returns a server with at
least four reported free slots. Those constants and test fixtures using 100-player
servers are not measured capacity guarantees.

Do not raise a cap to declare scalability complete. Coordinate mode admission,
directory headroom, stale reports, map/spawn capacity, team balance, client rendering,
replication, and backend load. Capacity races also need correctness tests.

## Bot workload

Each active bot performs a bounded target scan on a staggered 0.65–0.95 second
decision interval. Initial complexity is approximately O(B × P), where B is active
bots and P is character actors. The default desired population is eight total
participants with at most twelve bots, so profile at the configured maximum and
alongside crowded human matches before raising these settings. Measure server frame
time, navigation/path-follow requests, actor scans, and replicated character state;
do not treat the bot cap as a measured capacity claim.
## Source-backed measurement targets

| Path | Current cost pattern | What to inspect when changing it |
| --- | --- | --- |
| `GameplayUtils::GetPlayersWithinRadius` | Enumerates all character actors then filters distance | Repeated scans per attack/effect and allocations |
| `SpellEffectDisplay`, character damage/auto-attack feedback | Nearby-player selection and client RPC fan-out | Event rate times recipients, payload size, reliable traffic |
| `SpellSystem::ApplyAreaSpellEffects` / healing | Overlap candidates plus grid/tolerance checks per target | Dense AoE bursts and duplicated queries/effect work |
| `ASpellProjectile` | Always relevant, movement replication, authority overlap query every tick | Simultaneous projectiles, lifetime, collision and network cost |
| Character replication | Numerous properties, inventory and equipment state | Owner versus observer needs, update frequency, serialization completeness |
| `ScoreboardWidget::UpdateScoreboard` | Actor scan, sort, destroy/recreate rows every five seconds | Client allocation and UI cost at 40 players |
| Login/target cycling/team assignment | Actor scans and asynchronous load | Burst joins, retained logout actors, duplicate identity |
| `ServerStatusReporter` | 30-second roster and 60-second stat reports | Synchronized reporting and backend failure behavior |
| Phoenix `ServerTracker` | Upsert, presence updates, fetch active servers, PubSub broadcast | Query count and payload as servers/subscribers grow |
| Item snapshots | Per-character HTTP post and transactional delete/insert | End-of-match disconnect bursts and ordering/durability |

These are candidates for profiling, not proof that every listed path is currently a
bottleneck. Preserve correctness while measuring; optimizing away necessary authority
checks, effects, or replication is not an acceptable shortcut.

## Required practice

- State how new work grows with players, affected targets, active actors, inventory
  size, and event frequency. Avoid introducing a per-player per-tick scan over all
  players where an event or bounded query can provide the same behavior.
- Prefer relevant recipients, bounded payloads, and event-driven updates. Use caches,
  registries, spatial queries, or batched work only where measurements justify them;
  define invalidation and actor lifetime explicitly.
- Keep blocking I/O off gameplay hot paths. Bound queued work and retries; stagger
  periodic reporting if needed, with acknowledgement/idempotency for persistent data.
- Clean up projectiles, corpses, timer handles, HTTP callbacks, effect maps, widgets,
  and disconnected actors. A single short match cannot detect slow leaks.
- Include client CPU/GPU/readability as well as server CPU/network. Many nearby spell
  effects can make a server-correct fight unplayable on clients.

## Planned load acceptance

No committed 40-player harness or established timing budget exists yet. Before
enforcing a gate, establish a reference map, hardware, build configuration, engine
revision, tick target, network conditions, test seed, and repeatable player actions.
For a selected tick target `T`, the frame budget is `1000 / T` milliseconds; do not
invent a project tick rate or claim a benchmark that has not run.

Run idle/movement, scattered combat, and concentrated combat at 2, 20, and 40 players.
For Team Battle's current 24 cap, first measure within that cap; use an explicit test
configuration or a separately validated capacity change for a 40-player scenario.
Exercise AoE, healing, walls, projectiles, looting, death/respawn, roster reporting,
reconnect storms, and match-end persistence. Include a sustained session spanning
multiple rounds and a documented latency/loss scenario.

Record median/p95/p99 server frame time, client frame time, outgoing/incoming bytes,
RPC rates/backlog, active actors/timers, CPU/memory trend, query latency/count, HTTP
errors, and persistence results. Compare runs on the same reference environment.
Set explicit absolute budgets and allowed regression thresholds after collecting a
baseline; correctness failures such as lost items or divergent scores fail regardless
of speed. Keep artifacts and missing coverage visible in CI.
