# Combat, spells, and balance

Source anchors: [character](../../Source/Fibula/FibulaCharacter.cpp),
[spell execution](../../Source/Fibula/SpellSystem.cpp),
[spell definitions](../../Source/Fibula/SpellDatabase.cpp),
[formulas](../../Source/Fibula/GameFormulas.cpp),
[starting values](../../Source/Fibula/PlayerStartingElements.cpp).

## Current combat model

The vocations are Knight, Paladin, Sorcerer, and Druid. Characters start a fresh
runtime session at level 100. Base starting skills are:

| Vocation | Magic | Melee | Distance | Shielding | Automatic attacks |
| --- | --- | --- | --- | --- | --- |
| Knight | 8 | 65 | 20 | 80 | Melee, 210 Unreal units |
| Paladin | 21 | 10 | 65 | 40 | Ranged, 2,500 units; ranged weapon and ammunition required |
| Sorcerer | 65 | 10 | 10 | 20 | Not started by the current auto-attack path |
| Druid | 65 | 10 | 10 | 20 | Not started by the current auto-attack path |

Equipment and temporary effects modify effective values. Auto-attacks use a
two-second timer; current target distance checks run every 0.5 seconds with a
10,000-unit maximum. A range constant for mages does not enable mage auto-attacks.

Input reaches `ServerCastSpell`, `ServerUseItem`, or chat-spell handling on the
character, then `ASpellSystem::ServerTryExecuteSpell`. The server looks up the
definition, checks vocation, mana, exhaustion, target requirements, and protection
state, executes the selected spell type, then charges costs on reported success.
Runes connect spell names to item names; potions have their own consumption branch.
Revalidate input on the server even if the UI has already rejected it.

### Definitions and effects

`FSpellDefinition` in `SpellSystem.h` describes names/words, type, element, allowed
vocations, mana, range, area grid, coefficients, status effects, and cosmetic assets.
`USpellDatabase` keys names case-insensitively; incantation lookup compares `Words`.
Existing categories include targeted/area spells and runes, self support, potions,
targeted/area healing, magic walls, and moving projectile spells.

The catalog includes waves and beams, Sudden Death/Great Fireball/Explosion runes,
Berserk variants, haste, Magic Shield, Heal Friend, Mass Healing, Sharpshooter,
Paralyse, and Magic Wall. Read the actual definition for coefficients and allowed
vocations instead of importing rules from Tibia or inferring them from names.

- Area effects use overlap queries followed by a rotated grid test with 100-unit
  cells and tolerance. Grid marker `3` supplies the origin and positive cells can
  apply effects. Range, direction, cover, and grid boundaries need test scenarios.
- `ExecuteSpellEffects` checks line of sight for non-self spells. Individual
  execution paths have additional target/range checks; do not assume they are all
  equivalent, especially for damage targets versus healing targets.
- Magic walls require valid ground and clear space, replicate as actors, and have
  a 20-second destruction timer. Projectile spells spawn `ASpellProjectile`; it
  replicates movement, checks proximity on authority, and executes its effect on
  destruction. Its configured lifespan is four seconds.
- `ASpellProjectileAnimationActor` is a separate cosmetic travel-effect path used
  by `SpellEffectDisplay`. Do not infer damage timing or collision from the visual
  projectile, or move authoritative hit resolution into it.
- Timed support effects include speed, skills, magic shield, light, and healing
  blocking. Timer ownership and restoration vary across branches. Define refresh,
  stacking, replacement, death, and disconnect behavior for every new effect.

### Damage, healing, and cadence

`GameFormulas` derives health/mana/capacity from vocation and level. Spell effect
bounds use level and effective skill multiplied by definition coefficients; physical
damage also subtracts randomized defense/armor attenuation. Skill-based Knight and
Paladin spells combine weapon attack with melee/distance skill. Preserve rounding,
integer division, and random bounds in regression tests when refactoring formulas.

FFA support-healing multipliers are 0.5 for Knight/Paladin and 0.3 for mages;
Team Battle uses 1.0. These are mode rules, not display-only values. General exhaust
lasts one second, offensive exhaust two seconds. Validation uses general exhaust
for non-offensive spells and offensive exhaust for offensive spells; they are not
a single universal cooldown.

`ServerModifyHealth` applies protection-zone and mode policy checks, clamps healing,
then handles incoming-damage modifiers, amulet reduction/charges, mana absorption
through Magic Shield, health damage, attribution, and death. The magic-shield early
return means health-damage attribution is not identical to mana damage attribution.
Check that damage formulas cannot unintentionally turn an attack into healing when
attenuation exceeds a rolled value.

Protection-zone entry checks `IsInCombat`; the combat delay constant is 120 seconds.
The current health method rejects both damage and healing while protected, and
casting in a zone permits only `SupportSpell` through its initial gate. Do not
summarize this as merely blocking offensive casts. Zone overlap handling and
overlapping volumes need authority and state-transition tests.

## Required balance evidence

For tuning, document the intended problem and before/after values using the same
level, loadout, and scenario. Include all affected vocations and both modes:

- Burst and sustained damage/healing, mana and consumable cost, cooldown interaction,
  effective survivability, movement/control uptime, and counterplay.
- Representative duels, coordinated focus fire, support-heavy groups, and a crowded
  roughly 20-versus-20 fight; keep gear and class composition explicit.
- Spawn/protection boundaries, line of sight, latency, no-mana/no-item cases, and
  the interaction with death loss, replenishment, and match rewards.

Use deterministic seeds or controllable random sources for reproducible formula
tests. Test meaningful bounds/invariants and intended outcomes; do not make a test
that simply copies the implementation formula. Simulations support balance decisions
but do not replace playtesting for engagement and readability.

## Adding a spell or combat mechanic

Extend definitions and the existing execution category when possible. Add a new
category only for behavior that needs it, and update `IsOffensiveSpell`, `GetIsRune`,
validation, dispatch, cost handling, target selection, display, and tests together.
For an item-backed spell, align `ItemDatabase`, starting supplies, action bars, and
Phoenix item names where applicable. New actor/timer effects must clean up safely.

For a new vocation, update `EVocation`, `StringToVocation`, formula/skill switches,
starting supplies and Phoenix starting equipment, Phoenix vocation validation,
login choices, spell eligibility, saved layouts, and any Blueprint/mesh assumptions.
Unknown vocation strings currently fall back to Sorcerer in the game mode; that is
not a complete compatibility strategy for an added vocation.

Planned regression coverage: legal/illegal casts, no-cost failures, exactly-once
costs/effects, cooldown boundaries, friendly fire/healing by mode, grid/LOS boundaries,
effect refresh and expiry, shield overflow, death attribution, and replicated observer
results. See [testing](testing-and-ci.md) and [known gaps](known-gaps.md) before claiming
these invariants are already guaranteed.
