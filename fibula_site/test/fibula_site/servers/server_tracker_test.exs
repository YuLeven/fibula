defmodule FibulaSite.Servers.ServerTrackerTest do
  use FibulaSite.DataCase
  alias FibulaSite.Servers.{Server, ServerTracker}
  alias FibulaSite.Accounts.{User, Character}
  alias Phoenix.PubSub

  describe "update_server_status/1" do
    test "creates a new server when it doesn't exist" do
      status = %{
        "address" => "127.0.0.1",
        "port" => 7171,
        "version" => "1.0.0",
        "max_players" => 1000,
        "players_online" => 0,
        "game_mode" => "PvP",
        "players" => []
      }

      assert :ok = ServerTracker.update_server_status(status)
      assert [server] = Repo.all(Server)
      assert server.address == "127.0.0.1"
      assert server.port == 7171
    end

    test "updates existing server" do
      {:ok, server} =
        %Server{}
        |> Server.changeset(%{
          address: "127.0.0.1",
          port: 7171,
          version: "1.0.0",
          max_players: 1000,
          players_online: 0,
          game_mode: "PvP",
          last_update: DateTime.utc_now()
        })
        |> Repo.insert()

      status = %{
        "address" => "127.0.0.1",
        "port" => 7171,
        "version" => "2.0.0",
        "max_players" => 2000,
        "players_online" => 1,
        "game_mode" => "PvE",
        "players" => []
      }

      assert :ok = ServerTracker.update_server_status(status)
      updated_server = Repo.get(Server, server.id)
      assert updated_server.version == "2.0.0"
      assert updated_server.max_players == 2000
      assert updated_server.game_mode == "PvE"
    end

    test "updates online characters" do
      {:ok, server} =
        %Server{}
        |> Server.changeset(%{
          address: "127.0.0.1",
          port: 7171,
          version: "1.0.0",
          max_players: 1000,
          players_online: 0,
          game_mode: "PvP",
          last_update: DateTime.utc_now()
        })
        |> Repo.insert()

      {:ok, user} =
        %User{}
        |> User.registration_changeset(%{
          email: "test@example.com",
          password: "password123",
          password_confirmation: "password123"
        })
        |> Repo.insert()

      {:ok, character} =
        %Character{}
        |> Character.changeset(%{
          name: "Test Character",
          vocation: "Knight",
          user_id: user.id
        })
        |> Repo.insert()

      status = %{
        "address" => "127.0.0.1",
        "port" => 7171,
        "version" => "1.0.0",
        "max_players" => 1000,
        "players_online" => 1,
        "game_mode" => "PvP",
        "players" => ["Test Character"]
      }

      assert :ok = ServerTracker.update_server_status(status)
      updated_character = Repo.get(Character, character.id)
      assert updated_character.online_in_id == server.id

      # Test character goes offline
      status = %{status | "players" => [], "players_online" => 0}
      assert :ok = ServerTracker.update_server_status(status)
      offline_character = Repo.get(Character, character.id)
      assert is_nil(offline_character.online_in_id)
    end

    test "broadcasts server updates" do
      PubSub.subscribe(FibulaSite.PubSub, "server_status")

      status = %{
        "address" => "127.0.0.1",
        "port" => 7171,
        "version" => "1.0.0",
        "max_players" => 1000,
        "players_online" => 0,
        "game_mode" => "PvP",
        "players" => []
      }

      assert :ok = ServerTracker.update_server_status(status)
      assert_receive {:servers_updated, [server]}
      assert server.address == "127.0.0.1"
      assert server.port == 7171
    end
  end

  describe "get_active_servers/0" do
    test "returns only active servers" do
      recent =
        insert_server(%{
          address: "127.0.0.1",
          port: 7171,
          last_update: DateTime.utc_now()
        })

      _old =
        insert_server(%{
          address: "127.0.0.2",
          port: 7172,
          last_update: DateTime.add(DateTime.utc_now(), -3 * 60)
        })

      servers = ServerTracker.get_active_servers()
      assert length(servers) == 1
      assert hd(servers).id == recent.id
    end
  end

  describe "get_active_servers/1" do
    test "filters servers by game mode" do
      insert_server(%{game_mode: "Standard", last_update: DateTime.utc_now()})

      team_battle =
        insert_server(%{
          address: "127.0.0.2",
          port: 7172,
          game_mode: "TeamBattle",
          last_update: DateTime.utc_now()
        })

      servers = ServerTracker.get_active_servers(game_mode: "TeamBattle")
      assert length(servers) == 1
      assert hd(servers).id == team_battle.id
    end

    test "returns all servers when no game mode specified" do
      server1 = insert_server(%{game_mode: "Standard", last_update: DateTime.utc_now()})

      server2 =
        insert_server(%{
          address: "127.0.0.2",
          port: 7172,
          game_mode: "TeamBattle",
          last_update: DateTime.utc_now()
        })

      servers = ServerTracker.get_active_servers()
      assert length(servers) == 2
      server_ids = Enum.map(servers, & &1.id)
      assert server1.id in server_ids
      assert server2.id in server_ids
    end
  end

  defp insert_server(attrs) do
    {:ok, server} =
      %Server{}
      |> Server.changeset(%{
        address: Map.get(attrs, :address, "127.0.0.1"),
        port: Map.get(attrs, :port, 7171),
        version: "1.0.0",
        max_players: 1000,
        players_online: 0,
        game_mode: Map.get(attrs, :game_mode, "PvP"),
        last_update: Map.get(attrs, :last_update, DateTime.utc_now())
      })
      |> Repo.insert()

    server
  end
end
