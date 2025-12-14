defmodule FibulaSite.Servers.ServerTracker do
  alias FibulaSite.Repo
  alias FibulaSite.Servers.Server
  alias FibulaSite.Accounts.Character
  alias Phoenix.PubSub
  import Ecto.Query

  def update_server_status(status) do
    server_params = %{
      address: status["address"],
      port: status["port"],
      version: status["version"],
      max_players: status["max_players"],
      players_online: status["players_online"],
      game_mode: status["game_mode"],
      last_update: DateTime.utc_now()
    }

    result =
      Repo.get_by(Server, address: status["address"], port: status["port"])
      |> case do
        nil -> %Server{}
        existing -> existing
      end
      |> Server.changeset(server_params)
      |> Repo.insert_or_update()

    case result do
      {:ok, server} ->
        # Extract player names from the player objects
        player_names =
          Enum.map(status["players"] || [], fn
            %{"name" => name} -> name
            name when is_binary(name) -> name
          end)

        # Clear online status for characters no longer in this server
        from(c in Character,
          where: c.online_in_id == ^server.id and c.name not in ^player_names
        )
        |> Repo.update_all(set: [online_in_id: nil])

        # Update online status for current players
        from(c in Character, where: c.name in ^player_names)
        |> Repo.update_all(set: [online_in_id: server.id])

        servers = get_active_servers()
        PubSub.broadcast(FibulaSite.PubSub, "server_status", {:servers_updated, servers})
        :ok

      {:error, _changeset} ->
        {:error, :invalid_data}
    end
  end

  def get_active_servers(opts \\ []) do
    query = Server.active()

    query =
      case Keyword.get(opts, :game_mode) do
        nil -> query
        mode -> from(s in query, where: s.game_mode == ^mode)
      end

    Repo.all(query)
  end

  def online_player_count do
    from(c in Character, where: not is_nil(c.online_in_id))
    |> Repo.aggregate(:count, :id)
  end

  def reset_state do
    Repo.delete_all(Server)
  end
end
