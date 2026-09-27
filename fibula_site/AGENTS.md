# Phoenix login service and website

Read the root `AGENTS.md` first. Run the commands below from `fibula_site/`.

Read [backend contracts](../docs/agents/backend-contracts.md) for API, authentication,
discovery, and persistence work; [inventory and progression](../docs/agents/inventory-and-progression.md)
for item/character changes; and [UI and content](../docs/agents/ui-and-content.md)
for community pages. Follow [testing and CI](../docs/agents/testing-and-ci.md) and
`test/AGENTS.md` when adding tests. Check [known gaps](../docs/agents/known-gaps.md)
before relying on a current implementation as a guarantee.

This service is part of the PvP runtime. Preserve competitive integrity in ownership,
rewards, and persistence; evaluate reporting and discovery changes for servers with
roughly 40 players. Keep API contracts explicit so future modes and items can extend
them without scattered string mappings or silently incompatible clients.

## Architecture and conventions

- The app uses Elixir (`~> 1.14` in `mix.exs`), Phoenix 1.7, LiveView 1.0,
  Ecto/PostgreSQL, and Mix-managed Tailwind/esbuild. Use `mix.lock` and the existing
  Mix aliases; do not introduce an npm workflow for the current asset pipeline.
- `lib/fibula_site/` owns accounts, character persistence, and server tracking.
  `lib/fibula_site_web/` owns controllers, routers, LiveViews, and components.
  Put domain logic in contexts and keep transport/UI code focused on its boundary.
- Development uses the public/login endpoint on port 4000 and the backend endpoint
  on port 4001. Inspect both `router.ex` and `backend_router.ex` for API work.
  Preserve browser, player API, and server authentication boundaries.
- Match Unreal request/response shapes for character stats, items, login, and
  server status. Inspect `../Source/Fibula/` callers and relevant controller tests
  when altering a contract; test invalid input and unauthorized access as needed.
- Follow existing changeset, Ecto transaction, SQL Sandbox, fixture, and LiveView
  patterns. Add schema changes through new migrations in `priv/repo/migrations/`;
  preserve existing player data and do not rewrite deployed migration history.
- Make frontend edits in `assets/` and HEEx components/templates. Preserve existing
  interaction and accessibility conventions and check affected pages visually.

## Local setup and validation

Read `README.md`, `mix.exs`, and the relevant `config/*.exs` before setup. With
Elixir/Erlang and local PostgreSQL available, use `mix setup` for initial dependency,
database, and asset setup, then `mix phx.server` to run the app. Initial setup creates
and migrates the development database and runs `priv/repo/seeds.exs`.

The alternative `docker compose up --build` starts the development app and PostgreSQL
using `docker-compose.yml`; its entrypoint fetches dependencies and creates/migrates
the development database. Keep persistent database volumes unless resetting them
is explicitly intended.

Useful checks, chosen for the files changed:

```sh
mix format --check-formatted path/to/changed_file.ex
mix compile
mix test test/fibula_site_web/controllers/character_items_controller_test.exs
mix test
mix assets.build
```

Substitute the actual changed files and relevant test paths. Run focused tests first;
run the full suite for shared account, authentication, persistence, or routing work.
Use `mix assets.build` for asset changes. Format only touched files, keeping unrelated
formatting changes out of the diff.

`mix test` runs aliases that create and migrate the test database. The checked-in
test configuration uses PostgreSQL on `localhost`, database
`fibula_site_test` (with an optional `MIX_TEST_PARTITION` suffix), and SQL Sandbox.
Unlike development, it does not read `DB_HOST`; a test run inside the app container
needs an appropriate test database hostname configuration. Do not assume Compose
startup alone makes containerized tests work. Never point tests at production data.

Use existing `test/support/` fixtures and connection/data cases, including backend
connections where appropriate. Avoid real email delivery and external service calls
in tests. Do not use `mix ecto.reset`, drop databases, or remove Docker volumes as
routine validation. Report unavailable prerequisites or pre-existing failures
separately from failures caused by the change.
