# Inventory, equipment, progression, and rewards

Source anchors: [item model](../../Source/Fibula/GameItem.h),
[serialization](../../Source/Fibula/GameItem.cpp),
[catalog](../../Source/Fibula/ItemDatabase.cpp),
[character operations](../../Source/Fibula/FibulaCharacter.cpp),
[containers](../../Source/Fibula/ItemContainer.cpp),
[reward system](../../Source/Fibula/RewardSystem.cpp).

## Current item model

`FGameItem` holds name, description, weight, rarity, stack count, icon, item/use type,
use action, and equipment attributes. `UItemDatabase` registers runes, consumables,
and equipment and indexes items by rarity. Item names are also identifiers in spell
lookups, saved action bars, and Phoenix persistence. A rename is a compatibility
change; do not silently strand stored items.

Equipment slots are helm, armor, weapon, shield, legs, boots, amulet, ring,
ammunition, and bag. Attributes cover attack/defense/armor, skill/speed/capacity
bonuses, two-handed/ranged behavior, damage reduction, charges, and loss prevention.
Review getters and equip/unequip logic together when adding an attribute.

`AddItem` checks carrying capacity and combines stackable items by name;
`AddItemDirect` skips the capacity check for paths such as restoration and match
rewards. Current capacity display includes equipped weight, while `AddItem` computes
its admission check from inventory weight. Preserve or deliberately fix this
difference with tests rather than assuming the checks already agree.

`AItemContainer` is a replicated inventory base for team stashes and corpses. Character
transfer RPCs enforce a 300-unit distance. Taking an item limits a transfer to 50
items, or 100 for ammunition, and attempts a rollback if the character add fails.
The stash class has no team-ownership field enforcing who can loot it; its name
alone does not provide access control. Validate authoritative item identity, count,
weight, container validity, and eligibility for all new transfer paths.

`FGameItem::NetSerialize` currently transmits a subset of fields, omitting rarity and
equipment attributes. Do not assume adding a `UPROPERTY` to the struct makes it
arrive through this serializer; verify the owning and observing clients.

## Creation, runtime progression, and death

Phoenix character creation transactionally creates the character, highscore, and
starter equipment/inventory from
[Elixir starting elements](../../fibula_site/lib/fibula_site/accounts/player_starting_elements.ex).
Unreal separately sets initial level/skills and replenishes combat supplies from
[C++ starting elements](../../Source/Fibula/PlayerStartingElements.cpp).

Fresh actor initialization resets level to 100 and assigns vocation starting skills;
it does not restore all the experience/skill fields returned by the character API.
Respawn calls `InitializeCharacterStats(false)`, retaining runtime level/experience
but resetting starting skills and restoring health/mana and missing starter supplies.
Knight/Paladin initialization also contains fallback weapon/ammunition logic.

Current death flow in `HandleDeath`:

1. Increment deaths. Above level 90, remove 5% of experience and compute a reward
   pool of twice that loss for tracked attackers.
2. Retain damage contributors from the last 300 seconds, distribute by damage share
   when victim/attacker level ratio is at least 0.75, then recalculate victim level.
3. Notify the game mode for stats/scoring; clear targets and auto-attack.
4. Handle item loss and spawn a corpse at the death location.
5. Pin the client's camera for five seconds and immediately call server respawn on
   the same actor. The camera delay is not a five-second server respawn timer.

A loss-prevention amulet is consumed instead of normal item loss. Otherwise all
inventory moves to the corpse, with an independent 1% loss roll per equipment slot.
The corpse expires after 180 seconds. The reward-drop branch attempts a 35% roll
with multi-attacker/retaliation conditions, but its dependence on `DamageDealers`
interacts with the earlier clearing of that map; see [known gaps](known-gaps.md).

## Reward economy

Team Battle awards two Reward Presents per connected winning-team character. Opening
a present chooses a rarity by relative weight, then an item in that rarity excluding
the present itself; it falls back to Common if the selected rarity has no candidates.
Stackable rewards use per-rarity count ranges.

Current relative weights are Common 50, Uncommon 25, Rare 15, Epic 5, Legendary 1,
Artifact 0.001. These are weights, not percentages. Test empty pools, item exclusions,
stack limits, and deterministic rarity boundaries. The current present path removes
the present before calling `AddItem` and does not use the add result as success
criteria; capacity failures need explicit handling in future changes.

Persistence saves item snapshots, while highscore reporting aggregates kill/death
statistics and record levels. Neither is a general runtime-state save system. Read
[backend contracts](backend-contracts.md) before adding persistent progression.

## Extension and validation requirements

- Add catalog data before scattering item-name conditionals. Define identity,
  stackability, use behavior, slot restrictions, serialization, UI, and persistence.
- For new slots/fields, update C++ models/serialization/equip paths, Phoenix
  schemas/validation/migrations, snapshot read/write, and client presentation.
- Treat transfers and reward redemption as conservation operations: no duplication,
  loss on failed additions, negative counts, forged attributes, or out-of-range use.
- Test two clients competing for the final stack, full capacity, two-handed swaps,
  empty/invalid slots, charges, death loss, replenishment, and reconnect round trips.
- Assess progression and rare gear for snowballing, team inequality, and farming
  incentives. Include gear variance and replenishment in combat-balance scenarios.

Existing backend tests cover character creation, basic item snapshots/slot validation,
and stats updates. Unreal inventory/death/reward automation and full cross-component
round-trip tests are still planned, not verified by those backend tests.
