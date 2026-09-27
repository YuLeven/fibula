# Backend regression tests

Read `../../AGENTS.md`, `../AGENTS.md`, and the
[test/CI guide](../../docs/agents/testing-and-ci.md). Commands run from `fibula_site/`.

- Reuse `support/data_case.ex`, `support/conn_case.ex`, and account fixtures. Tests
  use ExUnit and Ecto SQL Sandbox with the test database; never real player data.
- Public/browser and backend endpoints differ. `ConnCase` provides `conn` and
  `backend_conn`; use the correct router/endpoint and credentials for the request.
  Browser session login does not replace the Bearer header for player API tests.
  Backend Basic credentials come from test config, not deployment values.
- Test context behavior and HTTP boundaries at the appropriate level. For gameplay
  contracts, assert JSON field shapes consumed by Unreal as well as stored effects.
- Character detail reads intentionally allow other authenticated users' profiles;
  do not confuse this with permission to possess a character. New play-authorization
  coverage must exercise the actual join boundary.
- Inventory/stat changes need malformed-input, rollback, repeated-request, ordering,
  and conservation cases as applicable. Discovery needs exact free-slot boundaries,
  version/mode mismatch, stale servers, and join-race coverage beyond basic selection.
- Use timestamps or controllable clocks for freshness boundaries rather than minute
  sleeps. PubSub/process tests must account for SQL Sandbox ownership and global
  workers; do not enable `async: true` where shared state makes it unsafe.
- Run the focused file first, then the full suite for shared changes. Keep fixtures
  isolated and deterministic. Assert the failure mode before fixing a regression.
- `mix test` creates/migrates the test database. Its hostname is `localhost` in the
  checked-in test config; development's `DB_HOST` is not honored there. Do not reset
  databases or delete Docker volumes as a test prerequisite.
- No CI workflow is currently checked in. Adding tests is necessary but does not
  automatically wire them into CI; keep the documented coverage inventory accurate.
