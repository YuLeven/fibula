defmodule FibulaSiteWeb.CharacterItemsControllerTest do
  use FibulaSiteWeb.ConnCase

  use Phoenix.VerifiedRoutes,
    endpoint: FibulaSiteWeb.BackendEndpoint,
    router: FibulaSiteWeb.BackendRouter,
    statics: ~w()

  import FibulaSite.AccountsFixtures
  alias FibulaSite.Accounts

  setup %{backend_conn: backend_conn} do
    server_config = Application.get_env(:fibula_site, :server_auth)
    credentials = Base.encode64("#{server_config[:username]}:#{server_config[:password]}")

    backend_conn =
      backend_conn
      |> put_req_header("accept", "application/json")
      |> put_req_header("authorization", "Basic #{credentials}")

    %{backend_conn: backend_conn}
  end

  describe "update character items" do
    test "updates items successfully", %{backend_conn: conn} do
      user = user_fixture()
      character = character_fixture(user)

      items = %{
        "equipment" => [
          %{"slot" => "helm", "item" => "Magician Hat", "count" => 1},
          %{"slot" => "armor", "item" => "Blue Robe", "count" => 1}
        ],
        "inventory" => [
          %{"item" => "Mana Potion", "count" => 50},
          %{"item" => "Health Potion", "count" => 25}
        ]
      }

      conn = post(conn, ~p"/backend/characters/#{character.name}/items", items: items)
      assert json_response(conn, 200) == %{"status" => "success"}

      updated_items = Accounts.get_character_items(character.id)
      assert length(updated_items.equipment) == 2
      assert length(updated_items.inventory) == 2
    end

    test "returns not found for non-existent character", %{backend_conn: conn} do
      items = %{
        "equipment" => [],
        "inventory" => []
      }

      conn =
        post(conn, ~p"/backend/characters/non-existent/items",
          items: %{"equipment" => [], "inventory" => []}
        )

      assert %{"message" => "Character not found", "status" => "error"} = json_response(conn, 404)
    end

    test "validates equipment slot types", %{backend_conn: conn} do
      user = user_fixture()
      character = character_fixture(user)

      items = %{
        "equipment" => [
          %{"slot" => "invalid_slot", "item" => "Some Item", "count" => 1}
        ],
        "inventory" => []
      }

      conn = post(conn, ~p"/backend/characters/#{character.name}/items", items: items)
      assert %{"status" => "error"} = json_response(conn, 422)
    end
  end
end
