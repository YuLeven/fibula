defmodule FibulaSiteWeb.API.CharacterStatsControllerTest do
  use FibulaSiteWeb.ConnCase

  use Phoenix.VerifiedRoutes,
    endpoint: FibulaSiteWeb.BackendEndpoint,
    router: FibulaSiteWeb.BackendRouter,
    statics: ~w()

  alias FibulaSite.Accounts
  alias FibulaSite.Repo
  alias FibulaSite.Accounts.Highscore
  import FibulaSite.AccountsFixtures

  setup %{backend_conn: conn} do
    user = user_fixture()

    {:ok, character1} =
      Accounts.create_character(user, %{name: "Test Char One", vocation: "Knight"})

    {:ok, character2} =
      Accounts.create_character(user, %{name: "Test Char Two", vocation: "Knight"})

    server_config = Application.get_env(:fibula_site, :server_auth)
    credentials = Base.encode64("#{server_config[:username]}:#{server_config[:password]}")

    conn =
      conn
      |> put_req_header("accept", "application/json")
      |> put_req_header("authorization", "Basic #{credentials}")

    %{conn: conn, character1: character1, character2: character2}
  end

  describe "update stats" do
    test "updates stats for multiple characters successfully", %{
      conn: conn,
      character1: character1,
      character2: character2
    } do
      stats = %{
        "characters" => [
          %{
            "character_name" => character1.name,
            "kills" => 5,
            "deaths" => 2,
            "level" => 10
          },
          %{
            "character_name" => character2.name,
            "kills" => 3,
            "deaths" => 1,
            "level" => 8
          }
        ]
      }

      conn = post(conn, ~p"/backend/characters/stats", stats)

      assert %{
               "status" => "success"
             } = json_response(conn, 200)

      # Verify the stats were updated
      highscore1 = Repo.get_by(Highscore, character_id: character1.id)
      assert highscore1.kills == 5
      assert highscore1.deaths == 2
      assert highscore1.max_level_per_game == 10

      highscore2 = Repo.get_by(Highscore, character_id: character2.id)
      assert highscore2.kills == 3
      assert highscore2.deaths == 1
      assert highscore2.max_level_per_game == 8
    end

    test "returns error when character not found", %{conn: conn} do
      stats = %{
        "characters" => [
          %{
            "character_name" => "NonExistentChar",
            "kills" => 5,
            "deaths" => 2,
            "level" => 10
          }
        ]
      }

      conn = post(conn, ~p"/backend/characters/stats", stats)

      assert %{
               "status" => "error",
               "message" => "Characters not found",
               "character_names" => ["NonExistentChar"]
             } = json_response(conn, 404)
    end

    test "accumulates stats correctly", %{conn: conn, character1: character1} do
      # First update
      stats = %{
        "characters" => [
          %{
            "character_name" => character1.name,
            "kills" => 5,
            "deaths" => 2,
            "level" => 10
          }
        ]
      }

      conn = post(conn, ~p"/backend/characters/stats", stats)
      assert %{"status" => "success"} = json_response(conn, 200)

      # Second update
      stats = %{
        "characters" => [
          %{
            "character_name" => character1.name,
            "kills" => 3,
            "deaths" => 1,
            "level" => 8
          }
        ]
      }

      conn = post(conn, ~p"/backend/characters/stats", stats)
      assert %{"status" => "success"} = json_response(conn, 200)

      # Verify accumulated stats
      highscore = Repo.get_by(Highscore, character_id: character1.id)
      # 5 + 3
      assert highscore.kills == 8
      # 2 + 1
      assert highscore.deaths == 3
      # keeps highest
      assert highscore.max_level_per_game == 10
    end
  end
end
