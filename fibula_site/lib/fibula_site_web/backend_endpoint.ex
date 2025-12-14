defmodule FibulaSiteWeb.BackendEndpoint do
  use Phoenix.Endpoint, otp_app: :fibula_site

  plug Plug.RequestId
  plug Plug.Telemetry, event_prefix: [:phoenix, :endpoint]

  if code_reloading? do
    plug Phoenix.CodeReloader
    plug Phoenix.Ecto.CheckRepoStatus, otp_app: :fibula_site
  end

  plug Plug.Parsers,
    parsers: [:json],
    pass: ["*/*"],
    json_decoder: Phoenix.json_library()

  plug FibulaSiteWeb.BackendRouter
end
