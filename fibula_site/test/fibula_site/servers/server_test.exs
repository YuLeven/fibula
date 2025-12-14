defmodule FibulaSite.Servers.ServerTest do
  use FibulaSite.DataCase

  alias FibulaSite.Servers.Server
  alias FibulaSite.Accounts.{User, Character}

  describe "changeset/2" do
    test "valid attributes" do
      attrs = %{
        address: "127.0.0.1",
        port: 7171,
        version: "1.0.0",
        max_players: 1000,
        players_online: 0,
        game_mode: "PvP",
        last_update: DateTime.utc_now()
      }

      changeset = Server.changeset(%Server{}, attrs)
      assert changeset.valid?
    end

    test "invalid without required fields" do
      changeset = Server.changeset(%Server{}, %{})
      refute changeset.valid?
      assert "can't be blank" in errors_on(changeset).address
      assert "can't be blank" in errors_on(changeset).port
      assert "can't be blank" in errors_on(changeset).last_update
    end

    test "enforces unique address and port combination" do
      attrs = %{
        address: "127.0.0.1",
        port: 7171,
        version: "1.0.0",
        max_players: 1000,
        players_online: 0,
        game_mode: "PvP",
        last_update: DateTime.utc_now()
      }

      {:ok, _server} = %Server{} |> Server.changeset(attrs) |> Repo.insert()
      {:error, changeset} = %Server{} |> Server.changeset(attrs) |> Repo.insert()

      assert "has already been taken" in errors_on(changeset).address
    end
  end

  describe "active/1" do
    test "returns only servers updated in the last 2 minutes" do
      recent = insert_server(%{last_update: DateTime.utc_now()})

      _old =
        insert_server(%{
          address: "127.0.0.2",
          port: 7172,
          last_update: DateTime.add(DateTime.utc_now(), -3 * 60)
        })

      servers = Server.active() |> Repo.all()
      assert length(servers) == 1
      assert hd(servers).id == recent.id
    end

    test "preloads online players" do
      server = insert_server()
      character = insert_character(%{online_in: server})

      [server_with_players] = Server.active() |> Repo.all()
      assert [loaded_character] = server_with_players.online_players
      assert loaded_character.id == character.id
    end
  end

  defp insert_server(attrs \\ %{}) do
    default_attrs = %{
      address: Map.get(attrs, :address, "127.0.0.1"),
      port: Map.get(attrs, :port, 7171),
      version: "1.0.0",
      max_players: 1000,
      players_online: 0,
      game_mode: "PvP",
      last_update: Map.get(attrs, :last_update, DateTime.utc_now())
    }

    {:ok, server} =
      %Server{}
      |> Server.changeset(default_attrs)
      |> Repo.insert()

    server
  end

  defp insert_character(attrs) do
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
        online_in_id: attrs.online_in.id,
        user_id: user.id
      })
      |> Repo.insert()

    character
  end
end
