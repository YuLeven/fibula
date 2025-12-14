defmodule FibulaSiteWeb.API.ServerStatusControllerTest do
  use FibulaSiteWeb.ConnCase
  alias FibulaSite.Servers.ServerTracker
  import FibulaSite.AccountsFixtures
  alias FibulaSite.Accounts

  setup %{conn: conn} do
    user = user_fixture()
    token = Accounts.generate_user_session_token(user)
    encoded_token = Base.encode64(token)

    conn =
      conn
      |> put_req_header("accept", "application/json")
      |> put_req_header("authorization", "Bearer #{encoded_token}")

    %{conn: conn, user: user}
  end

  setup do
    reset_server_tracker()
    :ok
  end

  describe "recommend/2" do
    test "recommends server with most players and sufficient slots", %{conn: conn} do
      # Set up multiple servers
      # 20 slots free
      update_server(7171, 100, 80)
      # 10 slots free
      update_server(7172, 100, 90)
      # 30 slots free, most players with >= 4 slots
      update_server(7173, 100, 70)

      conn = get(conn, ~p"/api/server/recommend", version: "1.0.0")

      assert %{
               "server" => %{
                 "port" => 7172,
                 "players_online" => 90,
                 "max_players" => 100
               }
             } = json_response(conn, 200)
    end

    test "returns error when all servers are full", %{conn: conn} do
      # Set up full servers
      update_server(7171, 100, 100)
      update_server(7172, 100, 100)

      conn = get(conn, ~p"/api/server/recommend", version: "1.0.0")

      assert json_response(conn, 503) == %{
               "status" => "error",
               "code" => "SERVERS_FULL",
               "message" =>
                 "All servers are currently full. You are in the queue and will be logged in shortly."
             }
    end

    test "recommends server with specific game mode", %{conn: conn} do
      # Set up multiple servers with different game modes
      update_server(7171, 100, 80, "Standard")
      update_server(7172, 100, 90, "TeamBattle")
      update_server(7173, 100, 95, "Standard")

      conn = get(conn, ~p"/api/server/recommend", version: "1.0.0", game_mode: "TeamBattle")

      assert %{
               "status" => "success",
               "server" => %{
                 "port" => 7172,
                 "players_online" => 90,
                 "max_players" => 100,
                 "game_mode" => "TeamBattle"
               }
             } = json_response(conn, 200)
    end

    test "returns servers full when no servers match game mode", %{conn: conn} do
      # Set up servers with different game mode
      update_server(7171, 100, 80, "Standard")
      update_server(7172, 100, 90, "Standard")

      conn = get(conn, ~p"/api/server/recommend", version: "1.0.0", game_mode: "TeamBattle")

      assert json_response(conn, 503) == %{
               "status" => "error",
               "code" => "SERVERS_FULL",
               "message" =>
                 "All servers are currently full. You are in the queue and will be logged in shortly."
             }
    end

    test "returns version mismatch when client version doesn't match any server", %{conn: conn} do
      # Set up servers with a different version
      update_server(7171, 100, 80, "Standard", "2.0.0")
      update_server(7172, 100, 90, "TeamBattle", "2.0.0")

      conn = get(conn, ~p"/api/server/recommend", version: "1.0.0", game_mode: "TeamBattle")

      assert json_response(conn, 426) == %{
               "status" => "error",
               "code" => "VERSION_MISMATCH",
               "message" =>
                 "You are using an outdated version of the game. Please update to the latest version."
             }
    end
  end

  defp update_server(port, max_players, players_online, game_mode \\ "PvP", version \\ "1.0.0") do
    status = %{
      "address" => "127.0.0.1",
      "port" => port,
      "max_players" => max_players,
      "players_online" => players_online,
      "version" => version,
      "game_mode" => game_mode,
      "players" => []
    }

    ServerTracker.update_server_status(status)
  end

  defp reset_server_tracker do
    ServerTracker.reset_state()
  end
end
