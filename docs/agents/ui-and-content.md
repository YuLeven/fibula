# Client UI, maps, and community pages

## Client feature map

All C++ paths below are relative to `Source/Fibula/`.

| Feature | Entry points | Behavior to preserve |
| --- | --- | --- |
| Login/character selection/queue | `FibulaLoginGameMode.*`, `LoginWidget.*`, `AuthTokenManager.*` | Login/register, character management, mode selection, cancellation and retry feedback |
| Movement, camera, targeting | `FibulaCharacter::SetupPlayerInputComponent`, targeting and camera methods | Enhanced Input bindings, mouse targeting, distinct heal target, zoom, dual-mouse movement, death camera |
| Main HUD and ground-item targeting | `FibulaHUD.*`, `StatusWidget.*` | Owning-player widgets; health/mana/status, inventory/equipment, chat, action bars |
| Slots, drag/drop, tooltips | `GameSlot.*`, `ItemDragDropOperation.h`, `TooltipWidget.*` | Inventory/equipment/action-bar semantics, item/spell tooltips, quantities |
| Spell browser | `SpellListWidget.*`, `SpellRowWidget.*`, `SpellDatabase.*` | Vocation-filtered catalog and usable action-bar entries |
| Loot | `LootWidget.*`, `ItemContainer.*` | Display replicated container contents and request server transfers |
| Scores | `ScoreboardWidget.*`, `ScoreboardRowWidget.*`, team game state | Kill sorting, team outcome screens, replicated match countdown |
| World feedback | `PlayerNameTagWidget.*`, `NameTagWidgetComponent.*`, floating widgets and display utilities | Readable targets, damage, healing, names, and status |
| Settings/audio | `SettingsWidget.*`, `GraphicsSettingsWidget.*`, `SoundSettingsWidget.*`, `SoundSettingsManager.*`, `BackgroundMusicManager.*` | Graphics settings via Unreal settings; local sound settings and music |

[StatusWidget.cpp](../../Source/Fibula/StatusWidget.cpp) saves action-bar layouts to
`ActionBar_<Vocation>` SaveGame slots and checks vocation on load. Layouts identify
content by name, so renamed spells/items require compatibility handling. Auth tokens
use the separate `AuthData` slot; neither file is the backend character save system.

[ScoreboardWidget](../../Source/Fibula/ScoreboardWidget.cpp) scans character actors,
sorts by kills, and reconstructs rows every five seconds. Rows read name, level, team,
kills, and deaths from the character, so AI-controlled characters without a PlayerState
remain visible. Match outcome UI is driven by Team Battle state. Test the underlying
counts, not just whether the widget renders.

Chat messages and incantations involve character server handling and the replicated
`AChatSystem`. New UI actions must request validated server operations, not mutate
gameplay state locally. Distinguish local input focus, owner RPC feedback, and
replicated observer feedback. Test that typing/chat/UI interactions do not trigger
unintended combat or movement actions.

## Maps, Blueprints, and visual balance

The configured login map is `LoginScreen`; the default arena is `Ankrahmun`; a
`TravelMap` and other map assets are tracked under `Content/Maps/`. This inventory
does not prove the other maps are configured or playable. Blueprint internals and
per-map overrides need editor inspection.

C++ resolves named Blueprint assets such as `/Game/Characters/BP_ThirdPersonCharacter`
and `/Game/HUDs/Widgets/WBP_LoginWidget`. `BindWidget` names and reflected interfaces
are contracts with those binary assets. Renames require references/redirects and an
editor compile/load check; source compilation alone will not catch every broken binding.

For map changes, assess both teams' spawn access, cover, choke points, line of sight,
protection boundaries, stash access, and traversability with magic walls. For effects,
preserve readable targeting and spell areas during a crowded 40-player fight. Client
frame time, audio overload, and occluded warnings affect competitive play as well as
server simulation. Avoid granting gameplay authority to particles, animation, or HUD.

Follow [Content/AGENTS.md](../../Content/AGENTS.md) for binary asset work and
[Config/AGENTS.md](../../Config/AGENTS.md) for maps, input, collision, and packaging.

## Community and account pages

Phoenix paths are under `fibula_site/lib/fibula_site_web/`:

- `live/server_status_live.ex`: active-server/player view fed by `server_status`
  PubSub broadcasts. Distinguish active-server counts from stored presence.
- `live/highscores_live.ex`: paginated/sorted highscore views backed by the Accounts
  context; lifetime totals and per-game maxima have different meanings.
- `live/character_search_live.ex` and `character_details_live.ex`: name search,
  profile and equipment display. Search currently uses exact stored-name lookup.
- `live/user_*` and `controllers/user_session_controller.ex`: browser registration,
  confirmation, login, reset, and settings flows. These differ from the game JSON API.
- `controllers/page_html/`, layouts/components, and `fibula_site/assets/`: landing,
  privacy, navigation, styles, and browser behavior.

Use existing HEEx/LiveView patterns, labels, error messages, and auth mounts. Keep
user input bounded and queries paginated. When extending public profile information,
separate public read visibility from permission to edit or play that character.

## Validation

Use relevant LiveView/controller tests for web changes and `mix assets.build` for
CSS/JS pipeline changes. Inspect altered UI at practical resolutions, keyboard focus,
loading/error/empty states, and reconnects. For Unreal, check widget bindings and
saved layouts in the editor/client, then the owning and observing player where
networked behavior changes. Visual checks supplement behavior tests; screenshots
alone do not establish correct targeting, scoring, or authority.
