defmodule FibulaSite.Repo do
  use Ecto.Repo,
    otp_app: :fibula_site,
    adapter: Ecto.Adapters.Postgres

  use Scrivener, page_size: 50
end
