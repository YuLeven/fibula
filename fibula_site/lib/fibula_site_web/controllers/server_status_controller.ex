defmodule FibulaSiteWeb.API.ServerStatusController do
  use FibulaSiteWeb, :controller
  alias FibulaSite.Servers.ServerTracker

  @error_codes %{
    version_mismatch: "VERSION_MISMATCH",
    servers_full: "SERVERS_FULL",
    server_busy: "SERVER_BUSY",
    maintenance: "MAINTENANCE"
  }

  def recommend(conn, params) do
    client_version = params["version"]
    game_mode = params["game_mode"]

    # First, get all active servers and check version match
    all_servers = ServerTracker.get_active_servers()

    version_matching_servers =
      Enum.filter(all_servers, fn server -> server.version == client_version end)

    cond do
      # Check version mismatch first
      Enum.empty?(version_matching_servers) ->
        conn
        |> put_status(:upgrade_required)
        |> json(%{
          status: "error",
          code: @error_codes.version_mismatch,
          message:
            "You are using an outdated version of the game. Please update to the latest version."
        })

      true ->
        # Then filter by game mode and check availability
        available_servers =
          version_matching_servers
          |> Enum.filter(fn server ->
            (game_mode == nil || server.game_mode == game_mode) &&
              server.max_players - server.players_online >= 4
          end)

        if Enum.empty?(available_servers) do
          conn
          |> put_status(:service_unavailable)
          |> json(%{
            status: "error",
            code: @error_codes.servers_full,
            message:
              "All servers are currently full. You are in the queue and will be logged in shortly."
          })
        else
          recommended_server =
            Enum.max_by(available_servers, fn server -> server.players_online end)
            |> Map.take([:address, :port, :players_online, :max_players, :game_mode])

          json(conn, %{
            status: "success",
            server: recommended_server
          })
        end
    end
  end
end
