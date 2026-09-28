# Game modes, teams, and match lifecycle

Source anchors: [base mode](../../Source/Fibula/FibulaGameMode.cpp),
[FFA](../../Source/Fibula/FibulaFFAGameMode.cpp),
[Team Battle](../../Source/Fibula/FibulaTeamBattleGameMode.cpp),
[Team Battle state](../../Source/Fibula/FibulaTeamBattleGameState.cpp),
[mode enum](../../Source/Fibula/GameModeType.h).

## Current behavior

| Rule | Free-for-all | Team Battle |
| --- | --- | --- |
| Wire mode name | `FreeForAll` | `TeamBattle` |
| Config game alias | `FFA` | `TeamBattle` |
| Reported mode capacity | Base `GetMaxPlayers()`: 44 | Override: 24 |
| Team identity | All characters get team 0 | Teams 1 and 2 |
| Assignment | No grouping | Count character actors; join the smaller team; ties go to team 1 |
| Damage policy | Other non-null characters | `IsOpponentOf`, comparing team IDs |
| Heal policy | `CanHeal` returns true | `IsAllyOf`, comparing team IDs |
| Spawn selection | Random from all player starts | First half of discovered player starts for team 1; second half for team 2 |
| End condition | No custom FFA score/time finish in this subclass | 30 points or 1,500 seconds |
| Match reward | No FFA winner award in this subclass | Two Reward Presents per connected winning-team character; no draw award |

Mode capacity is used in reporting and the asynchronous character-load admission
check. Directory recommendation separately requires four free slots; these are not
one atomic admission system. Roughly 40 players is the project target, not the
current Team Battle cap or a measured limit.

The base mode's `CanDamage`/`CanHeal` return true; the `World` alias points to this
base class. Do not treat it as equivalent to all FFA subclass policies.

### Team Battle sequence

`BeginPlay -> StartTeamBattle -> WaitingToStart -> InProgress -> WaitingPostMatch`

- Start populates every discovered `ATeamStash` with the same stash supply list.
  The base `BeginPlay` also calls the population hook.
- Waiting resets scores and records `BattleStartTime`. A two-second timer moves
  the match into progress; there is no custom minimum-player readiness check here.
- Start schedules the 25-minute end timer. Each active-match death gives one point
  to the opposite team based on the victim's team ID, not killer attribution.
- At end, the higher score wins; equality produces a draw. Rewards are added,
  outcome UI is multicast, and cleanup is scheduled after 20 seconds.
- Cleanup marks possessed characters protected, requests a return to menu, and
  schedules the next battle after five seconds. Review actual disconnect/actor
  cleanup when changing this loop; requesting a client return is not a persistence
  acknowledgement.
- The replicated countdown uses server time and `BattleStartTime`, which is set
  during waiting, two seconds before the duration timer begins in progress.

Death handling and respawn live in `AFibulaCharacter`, not in the mode subclasses.
See [inventory and progression](inventory-and-progression.md) for those shared rules.

## Server bots

AFibulaGameMode reconciles bot population every few seconds. BotTargetPopulation is
the desired total active population, MaximumBotCount caps the AI population, and
BotMinimumHumanPlayers avoids running a bot-only match. Defaults are eight total
participants, twelve maximum bots, and one required human. Once the minimum loaded-human threshold is met,
the desired bot count is:

    min(MaximumBotCount, max(0, BotTargetPopulation - loaded humans), open mode slots)

With defaults, zero loaded humans creates no bots, one human gets seven, two get
six, and eight get none. Bots use AFibulaBotController, standard character stats
and starting equipment, and the mode's existing team, damage, healing, and spawn rules.

Bots are AAIController-possessed characters. They prefer nearby human opponents,
use vocation-specific attacks and available healing, patrol when no enemy is
available, and switch targets and choose a new route after a movement stall.
Navigation uses the map's NavMesh when available and direct movement as a fallback.
Bot inventory is transient and suppressed from Phoenix persistence. Bots can affect
Team Battle score through normal character deaths but do not receive player-controller
rewards. When population shrinks, removal favors the larger team; human teams remain
unchanged.

Set BotRandomSeed in game config or pass -FibulaBotSeed=<integer> for repeatable
vocation and decision selection. Keep bot population and mode capacity together:
FFA currently admits 44 players and Team Battle 24.

## Competitive balance requirements

Equalize opportunity across both teams: joining/leaving, spawn safety, routes to
combat, stash access, class composition, and reward eligibility all matter. Current
assignment counts character actors, including retained combat-logout pawns; it does
not balance skill, vocation, equipment, or parties, and has no continuous rebalancer.
Do not claim otherwise or introduce silent mid-fight team switching.

Spawn selection depends on actor enumeration order, not explicit team tags. Any map
or spawn change requires an editor check and both-team scenarios. New explicit
spawn metadata should have documented fallback behavior for existing maps.

For rule changes, state before/after behavior for even and odd populations, a tied
join, disconnect/reconnect, team wipes, late joins, and a match-end/death race.
Measure results for both teams and all vocations, not just aggregate win rate.

## Adding a mode

1. Extend `EGameModeType` and add an `AFibulaGameMode` subclass. Override policy,
   spawn, assignment, capacity, and lifecycle hooks as needed; put replicated match
   state in a game-state subclass.
2. Update config aliases/maps, login mode display/normalization, reporting wire
   names, Phoenix discovery handling, and relevant web display/tests together.
3. Define friendly fire, healing, neutral-team handling, join-in-progress, score,
   ties, rewards, and cleanup explicitly. Audit two-mode assumptions such as
   `bIsInTeamBattle`, team-ID comparisons, and FFA healing multipliers.
4. Add rule tests plus a dedicated-server lifecycle scenario that checks both
   clients' scores/outcome, cleanup, persistence, and the next match.

Native mode/mechanics coverage now includes balanced assignment through actual player
initialization, an opponent melee encounter, friendly-fire rejection, death scoring,
respawn protection, active-match score gating, and countdown boundaries. Remaining
gaps include map-backed spawn ownership, actual replicated clients, draw and reward
eligibility, match cleanup, reconnects, and repeated rounds. The bot multiplayer
smoke in [testing and CI](testing-and-ci.md) additionally exercises both modes with
real clients. Forty-actor assignment is not a 40-connection capacity test. The FFA
header's self-healing-only comment contradicts its permissive implementation; see
[known gaps](known-gaps.md).