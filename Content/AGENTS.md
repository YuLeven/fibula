# Unreal maps, Blueprints, and assets

Read the root and `Source/AGENTS.md`, plus
[UI/content](../docs/agents/ui-and-content.md) and the affected gameplay guide.

- `.uasset` and `.umap` are Git LFS binary assets. Inspect/edit them with Unreal
  Editor or supported engine tooling; never rewrite them with text tools. Check
  actual asset availability before a load/cook and preserve LFS tracking.
- Preserve C++ asset paths, Blueprint inheritance, reflected names, and `BindWidget`
  contracts. Update references/redirects for intentional moves or renames; do not
  mass-resave unrelated content.
- Arena edits affect competitive balance: verify both teams' spawn locations,
  cover, protection boundaries, stash access, movement routes, and spell sightlines.
  Team Battle currently splits enumerated player starts into halves rather than
  resolving explicit team tags; inspect the actual loaded map behavior.
- Test intended effects and UI under crowded combat. Excess particles/audio,
  misleading spell footprints, or unreadable team/target markers can harm fair play
  even when server rules are unchanged.
- Compile changed Blueprints, inspect affected maps/widgets in the editor/client,
  and perform the relevant multiplayer or cook/load check. Record unavailable editor
  checks explicitly. Include a concise description of binary changes and useful
  screenshots for review; source compilation alone cannot validate them.
