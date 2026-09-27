# Login, discovery, and persistence contracts

Source anchors: [public router](../../fibula_site/lib/fibula_site_web/router.ex),
[backend router](../../fibula_site/lib/fibula_site_web/backend_router.ex),
[authentication](../../fibula_site/lib/fibula_site_web/user_auth.ex),
[accounts context](../../fibula_site/lib/fibula_site/accounts.ex),
[login client](../../Source/Fibula/LoginWidget.cpp),
[game mode](../../Source/Fibula/FibulaGameMode.cpp),
[reporter](../../Source/Fibula/ServerStatusReporter.cpp).

## Endpoint contract

Development runs the public endpoint on 4000 and server backend on 4001. Browser
routes use Phoenix sessions/CSRF. Player API requests use `Authorization: Bearer`
with a Base64-encoded stored session token, not a JWT. Backend requests use Basic
authentication from environment/runtime configuration. Keep these boundaries distinct.

| Route | Auth | Request / response responsibilities |
| --- | --- | --- |
| `POST /api/register`, `POST /api/login` | Credentials | `email`, `password`; success returns `status` and `data.email`, `data.token` |
| `GET /api/characters` | Bearer | Lists authenticated user's `characters` |
| `POST /api/characters` | Bearer | Nested `character` attributes; creates a character and starting records; current account limit is six |
| `GET /api/characters/:name` | Bearer | Router calls parameter `id`; controller treats it as a name; returns `character` including equipment/inventory |
| `DELETE /api/characters/:name` | Bearer + ownership check | Deletes an owned character; tests cover denial for another user's character |
| `GET /api/server/recommend` | Bearer | `version`, optional `game_mode`; client also sends `character`, which recommendation does not use |
| `POST /backend/server/status` | Basic | Address/port/version/mode/capacity/player list snapshot |
| `POST /backend/characters/stats` | Basic | `characters` array of named kill/death deltas and match record candidates |
| `POST /backend/characters/:name/items` | Basic | `items.equipment` and `items.inventory` full replacement snapshot |

Character names are title-cased on creation, allow letters/spaces, and are 2-20
characters. Retrieval uses exact database name matching. The character detail API
intentionally allows authenticated users to view other users' characters, as a test
asserts. Reading a character record therefore does **not** prove permission to possess
it in the game. Do not accidentally treat profile visibility as gameplay authorization.

## Discovery and joining

The client normalizes display labels to `FreeForAll` / `TeamBattle` and asks for a
server matching `ServerConfig::GAME_VERSION`. Recommendation uses active servers,
filters exact version first, then mode and at least four remaining slots, choosing
the most populated eligible server. It returns address, port, population, capacity,
and mode. No matching version (including no active servers) yields HTTP 426 with
`VERSION_MISMATCH`; no eligible server after mode/capacity filtering yields 503 with
`SERVERS_FULL`.

The client retries transient errors and transport failures at five-second intervals.
`SERVER_BUSY` and `MAINTENANCE` are also recognized transient codes, but the current
recommendation action emits only the mismatch/full cases above. This UI is a polling
loop, not a durable FIFO queue, reservation, matchmaking rating, or party allocation.

Travel passes character/token options to the dedicated server. `LoadCharacterData`
normally requests a record using that token; an existing same-name actor instead
gets repossessed without that HTTP request. `OnCharacterDataLoaded` does a separate
capacity check before spawn. These paths need ownership, disconnect-during-request,
duplicate-login, stale-response, and capacity-race tests; see [known gaps](known-gaps.md).

## Reporting, online presence, and stats

- `UServerStatusReporter` sends status every 30 seconds and stats every 60 seconds,
  both with an immediate initial timer firing.
- Status fields: `address`, `port`, `max_players`, `players_online`, `version`,
  `game_mode`, and `players` entries containing `name`, `vocation`, and `level`.
- `ServerTracker` upserts by address/port, sets `last_update`, updates character
  `online_in_id` from names, and broadcasts `{:servers_updated, servers}` on
  `server_status`. The online page subscribes to that topic.
- `Server.active` uses a two-minute freshness window. `ServerCleanup` runs every
  minute, clears presence and removes records older than five minutes. These are
  different thresholds; presence and active-directory counts can temporarily differ.
- Stats entries contain `character_name`, additive `kills`/`deaths`, `match_kills`,
  `match_deaths`, and `level`. Bulk updates use an Ecto transaction and reject a
  batch with missing names. Totals add deltas; record fields take maxima. This does
  not save runtime experience or replace the character's skill fields.

The reporter clears accumulated stats after dispatch without waiting for an HTTP
acknowledgement. Repeating a stats batch adds the deltas again. Any future retry
mechanism needs acknowledgement and deduplication/idempotency design, not blind retry.
`BackendServerController.update` currently responds success without propagating a
tracker error; HTTP 200 alone is not proof the status was persisted.

## Item snapshots

The payload is `items: {equipment: [...], inventory: [...]}`. Equipment entries use
`slot`, `item`, `count`, and optional `charges`; inventory entries omit `slot` and
use array order as position. Unreal saves all ten named equipment slots when filled.
Phoenix defaults omitted charges to zero and reads inventory back in position order.

`Accounts.update_character_items` validates equipment then transactionally deletes
and reinserts the snapshot. Inventory uses bulk insertion without applying its
changeset first. A transaction protects replacement atomicity, but does not supply
request ordering, deduplication, full input validation, or protection from stale
snapshots overwriting newer ones.

`AFibulaCharacter::Destroyed` triggers an item post. A 300-second persistence timer
helper exists in the game mode but has no C++ call site starting it. The post has no
completion handler. Do not document guaranteed periodic saves or crash durability.

## Safe extension and tests

When changing a contract, trace the C++ sender/parser, Phoenix route, auth plug,
controller, context, schema/migration, and tests. Preserve names/enums for stored and
older clients, or specify a migration/version rollout. Adding a mode or item should
not require independent contradictory mappings in each layer.

Test malformed/missing fields, ownership and credentials, boundary capacity, stale
presence, failed transactions, out-of-order/duplicate delivery, and reconnect item
round trips. Use local fixtures and test endpoints. Avoid real server registration,
production data, or tokens in logs. Add controllable HTTP seams for Unreal failure
tests rather than coupling them to a live remote service.
