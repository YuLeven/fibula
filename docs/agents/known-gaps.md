# Known implementation gaps and assumptions to check

These are source observations from the initial `861db51` review, not runtime
reproductions or a complete defect/security audit. They identify where agents must
not mistake current code for an intended guarantee. Fixes require focused work and
regression tests; this documentation does not change gameplay or infrastructure.

| Area | Observation and source anchor | Required follow-up when touching it |
| --- | --- | --- |
| Game possession auth | `FibulaGameMode::LoadCharacterData` reuses a same-name actor before token validation. Its normal detail fetch calls an API that intentionally allows viewing other users' characters (`CharacterController.show` and its tests). `InitNewPlayer` also logs travel options containing the token. | Establish server-side permission to play the character for both new/reconnect paths; preserve intentional public profile reads; redact tokens; test foreign/invalid credentials and duplicate sessions. |
| Persistence delivery | `StartItemPersistenceTimer` has no C++ caller; destruction posts items without a completion handler. `ReportCharacterStats` clears its buffer immediately after dispatch; backend stats add deltas. | Do not claim periodic/durable/exactly-once saves. Design acknowledgements, ordering and duplicate handling before adding retries; test outage and shutdown behavior. |
| Item replication | `FGameItem::NetSerialize` omits `Rarity` and `EquipmentAttributes`. | Verify round-trip/client behavior for all fields that clients need; adding a reflected field alone is insufficient. |
| Item validation | Inventory snapshots bypass `CharacterInventory.changeset` in `Accounts.update_character_items`. Several character RPCs accept item structs and use client-provided attributes; transfer-to-container dereferences its argument without the validity check used by transfer-from-container. | Validate authoritative item data, quantities, actor validity, range, and ownership at boundaries; test malformed requests and conservation. |
| Reward conservation | `ProcessRewardPresent` removes the present and ignores the result of `AddItem`. | Test full-capacity and failure paths so redemption cannot silently destroy value. |
| Combat entry points | Health/auto-attack methods are client-callable server RPCs. Normal callers perform some checks outside the RPC entry; `HasAuthority` alone does not validate a client request. Potion execution attempts removal after applying mana without the rune count gate. | Test direct invalid requests, attack rate/range/ownership, and missing consumables at the authoritative boundary. |
| FFA/healing targeting | FFA header says self-healing only but `CanHeal` returns true. `FindNextTarget(true)` adds non-allies in Team Battle although healing policy allows allies. | Treat implementation and comment as inconsistent; establish intended targeting/policy and test the matrix before changing it. |
| Kill and death records | `IncrementKills` has no C++ call site found; game-state kill accumulation reads the character kill count. Above level 90, death clears `DamageDealers` before item-loss reward eligibility reads it. | Verify score/highscore/UI attribution and drop eligibility end to end; consider possible Blueprint calls before asserting a runtime outcome. |
| Team lifecycle | Assignment counts character actors, tie-breaks to team 1, and spawn grouping depends on discovered actor order. Battle start time is set before the two-second wait ends. | Test retained logout pawns, joins/leaves, stable team spawns, countdown versus end timer, and repeated matches. |
| Discovery/admission | Mode caps are 44/24, recommendation requires four free slots, reports are periodic, and final admission occurs after async loading. There is no slot reservation. | Distinguish capacity targets from current behavior; test boundary and simultaneous joins, stale status, and mode-specific capacity. |
| Status acknowledgement | `BackendServerController.update` returns success regardless of `ServerTracker.update_server_status` result. | Test invalid status and propagate failure meaningfully before clients rely on acknowledgements. |
| Validation coverage | A local bot multiplayer smoke runner exists, but it has not yet been run on this checkout; tracked CI workflows, general Unreal gameplay automation, cross-service smoke, and load harnesses remain absent. | Run scripts/test_bots.ps1 with the UE 5.5 source engine, then continue the staged [CI roadmap](testing-and-ci.md). Do not describe unrun checks as passing. |

Source navigation: [character](../../Source/Fibula/FibulaCharacter.cpp),
[game mode](../../Source/Fibula/FibulaGameMode.cpp),
[spell system](../../Source/Fibula/SpellSystem.cpp),
[serializer](../../Source/Fibula/GameItem.cpp),
[rewards](../../Source/Fibula/RewardSystem.cpp),
[reporter](../../Source/Fibula/ServerStatusReporter.cpp),
[accounts](../../fibula_site/lib/fibula_site/accounts.ex),
[character detail tests](../../fibula_site/test/fibula_site_web/controllers/character_controller_test.exs),
[status controller](../../fibula_site/lib/fibula_site_web/controllers/backend_server_controller.ex).

When resolving an entry, link its regression test or reproducible validation and
update the affected feature guide. Do not preserve a known accidental behavior as a
balance rule merely because it is described here.
