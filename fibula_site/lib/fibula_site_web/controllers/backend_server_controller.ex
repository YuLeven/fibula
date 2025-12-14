defmodule FibulaSiteWeb.API.BackendServerController do
  use FibulaSiteWeb, :controller
  alias FibulaSite.Servers.ServerTracker

  def update(conn, params) do
    ServerTracker.update_server_status(params)
    json(conn, %{status: "success"})
  end
end
