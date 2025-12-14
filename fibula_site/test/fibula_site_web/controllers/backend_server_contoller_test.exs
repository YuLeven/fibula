defmodule FibulaSiteWeb.API.BackendServerControllerTest do
  use FibulaSiteWeb.ConnCase

  use Phoenix.VerifiedRoutes,
    endpoint: FibulaSiteWeb.BackendEndpoint,
    router: FibulaSiteWeb.BackendRouter,
    statics: ~w()

  alias FibulaSite.Servers.ServerTracker
  alias FibulaSite.Accounts.{User, Character}
  alias FibulaSite.Repo
  import FibulaSite.AccountsFixtures

  setup %{backend_conn: conn} do
    server_config = Application.get_env(:fibula_site, :server_auth)
    credentials = Base.encode64("#{server_config[:username]}:#{server_config[:password]}")

    conn =
      conn
      |> put_req_header("authorization", "Basic #{credentials}")

    %{backend_conn: conn}
  end

  describe "update/2" do
    test "updates server status successfully", %{backend_conn: conn} do
      # Create test user and characters
      user = user_fixture()

      {:ok, _character1} =
        %Character{}
        |> Character.changeset(%{
          name: "First Player",
          vocation: "Knight",
          user_id: user.id
        })
        |> Repo.insert()

      {:ok, _character2} =
        %Character{}
        |> Character.changeset(%{
          name: "Second Player",
          vocation: "Sorcerer",
          user_id: user.id
        })
        |> Repo.insert()

      status = %{
        "address" => "127.0.0.1",
        "port" => 7171,
        "version" => "1.0.0",
        "max_players" => 100,
        "players_online" => 50,
        "game_mode" => "PvP",
        "players" => [
          %{"name" => "First Player", "level" => 100, "vocation" => "Knight"},
          %{"name" => "Second Player", "level" => 80, "vocation" => "Sorcerer"}
        ]
      }

      conn = post(conn, ~p"/backend/server/status", status)
      assert json_response(conn, 200) == %{"status" => "success"}

      # Verify the server was updated in the tracker
      servers = ServerTracker.get_active_servers()

      [server] =
        Enum.filter(servers, fn server -> server.address == "127.0.0.1" && server.port == 7171 end)

      assert server.max_players == 100
      assert server.players_online == 50
      assert length(server.online_players) == 2
    end

    test "requires authentication", %{backend_conn: conn} do
      conn =
        conn
        |> delete_req_header("authorization")
        |> post(~p"/backend/server/status", %{})

      assert json_response(conn, 401) == %{
               "status" => "error",
               "message" => "Unauthorized"
             }
    end
  end

  defp update_server(port, max_players, players_online) do
    status = %{
      "address" => "127.0.0.1",
      "port" => port,
      "max_players" => max_players,
      "players_online" => players_online,
      "version" => "1.0.0",
      "game_mode" => "PvP",
      "players" => []
    }

    ServerTracker.update_server_status(status)
  end
end
