defmodule FibulaSite.Servers.ServerCleanupTest do
  use FibulaSite.DataCase
  alias FibulaSite.Servers.{Server, ServerCleanup}
  alias FibulaSite.Accounts.{User, Character}
  alias FibulaSite.Repo

  describe "cleanup" do
    test "removes stale servers" do
      # Create a stale server
      {:ok, stale_server} =
        %Server{}
        |> Server.changeset(%{
          address: "127.0.0.1",
          port: 7171,
          version: "1.0.0",
          max_players: 1000,
          players_online: 0,
          game_mode: "PvP",
          last_update: DateTime.add(DateTime.utc_now(), -6 * 60)
        })
        |> Repo.insert()

      # Create an active server
      {:ok, active_server} =
        %Server{}
        |> Server.changeset(%{
          address: "127.0.0.2",
          port: 7172,
          version: "1.0.0",
          max_players: 1000,
          players_online: 0,
          game_mode: "PvP",
          last_update: DateTime.utc_now()
        })
        |> Repo.insert()

      # Trigger cleanup
      ServerCleanup.cleanup_stale_servers()

      # Verify stale server was removed
      assert is_nil(Repo.get(Server, stale_server.id))
      # Verify active server remains
      assert Repo.get(Server, active_server.id)
    end

    test "clears online status for characters in stale servers" do
      # Create a stale server
      {:ok, stale_server} =
        %Server{}
        |> Server.changeset(%{
          address: "127.0.0.1",
          port: 7171,
          version: "1.0.0",
          max_players: 1000,
          players_online: 0,
          game_mode: "PvP",
          last_update: DateTime.add(DateTime.utc_now(), -6 * 60)
        })
        |> Repo.insert()

      # Create a user and character
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
          user_id: user.id,
          online_in_id: stale_server.id
        })
        |> Repo.insert()

      # Trigger cleanup
      ServerCleanup.cleanup_stale_servers()

      # Verify character's online status was cleared
      updated_character = Repo.get(Character, character.id)
      assert is_nil(updated_character.online_in_id)
    end

    test "keeps online status for characters in active servers" do
      # Create an active server
      {:ok, active_server} =
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

      # Create a user and character
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
          user_id: user.id,
          online_in_id: active_server.id
        })
        |> Repo.insert()

      # Trigger cleanup
      ServerCleanup.cleanup_stale_servers()

      # Verify character's online status was not cleared
      updated_character = Repo.get(Character, character.id)
      assert updated_character.online_in_id == active_server.id
    end
  end
end
