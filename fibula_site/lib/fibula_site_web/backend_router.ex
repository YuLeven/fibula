defmodule FibulaSiteWeb.BackendRouter do
  use Phoenix.Router, helpers: true
  import Plug.Conn
  import Phoenix.Controller
  import FibulaSiteWeb.UserAuth

  pipeline :backend do
    plug :accepts, ["json"]
  end

  scope "/backend", FibulaSiteWeb.API do
    pipe_through [:backend, :require_server_authentication]

    post "/server/status", BackendServerController, :update
    post "/characters/:name/items", CharacterItemsController, :update
    post "/characters/stats", CharacterStatsController, :update
  end
end
