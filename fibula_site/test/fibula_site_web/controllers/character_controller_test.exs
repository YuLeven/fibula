defmodule FibulaSiteWeb.CharacterControllerTest do
  use FibulaSiteWeb.ConnCase

  alias FibulaSite.Accounts
  import FibulaSite.AccountsFixtures

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

  describe "create character" do
    test "creates character when data is valid", %{conn: conn} do
      attrs = valid_character_attributes()

      conn = post(conn, ~p"/api/characters", character: attrs)
      assert %{"status" => "success", "character" => data} = json_response(conn, 201)
      assert data["name"] == attrs.name
      assert data["vocation"] == attrs.vocation
    end

    test "renders errors when data is invalid", %{conn: conn} do
      conn = post(conn, ~p"/api/characters", character: %{})
      assert %{"status" => "error", "errors" => errors} = json_response(conn, 422)
      assert errors["name"] == ["can't be blank"]
      assert errors["vocation"] == ["can't be blank"]
    end

    test "requires authentication", %{conn: conn} do
      conn =
        conn
        |> delete_req_header("authorization")
        |> post(~p"/api/characters", character: %{})

      assert json_response(conn, 401)
    end

    test "returns error when character limit is reached", %{conn: conn} do
      # Create 6 characters first
      for _i <- 1..6 do
        post(conn, ~p"/api/characters", character: valid_character_attributes())
      end

      # Try to create a 7th character
      conn = post(conn, ~p"/api/characters", character: valid_character_attributes())

      assert %{
               "status" => "error",
               "message" =>
                 "Character limit reached. Maximum of 6 characters allowed per account."
             } = json_response(conn, 422)
    end

    test "capitalizes character name correctly", %{conn: conn} do
      test_cases = [
        {"test", "Test"},
        {"test foo", "Test Foo"},
        {"tIo NaCHO", "Tio Nacho"},
        {"mary jane", "Mary Jane"},
        {"JOHN DOE", "John Doe"}
      ]

      for {input, expected} <- test_cases do
        attrs = valid_character_attributes(name: input)
        conn = post(conn, ~p"/api/characters", character: attrs)
        assert %{"status" => "success", "character" => data} = json_response(conn, 201)
        assert data["name"] == expected
      end
    end
  end

  describe "list characters" do
    test "lists all user's characters", %{conn: conn, user: user} do
      character = character_fixture(user)
      conn = get(conn, ~p"/api/characters")

      assert %{"status" => "success", "characters" => [data]} = json_response(conn, 200)
      assert data["name"] == character.name
    end

    test "returns empty list when user has no characters", %{conn: conn} do
      conn = get(conn, ~p"/api/characters")
      assert %{"status" => "success", "characters" => []} = json_response(conn, 200)
    end
  end

  describe "delete character" do
    test "deletes chosen character", %{conn: conn, user: user} do
      character = character_fixture(user)
      conn = delete(conn, ~p"/api/characters/#{character.name}")
      assert %{"status" => "success"} = json_response(conn, 200)

      conn = get(conn, ~p"/api/characters")
      assert %{"characters" => []} = json_response(conn, 200)
    end

    test "returns not found for non-existent character", %{conn: conn} do
      conn = delete(conn, ~p"/api/characters/non-existent")
      assert %{"status" => "error", "message" => "Character not found"} = json_response(conn, 404)
    end

    test "cannot delete other user's character", %{conn: conn} do
      other_user = user_fixture()
      character = character_fixture(other_user)

      conn = delete(conn, ~p"/api/characters/#{character.name}")

      assert %{"status" => "error", "message" => "Not authorized to delete this character"} =
               json_response(conn, 403)
    end
  end

  describe "get character" do
    test "gets character by name", %{conn: conn, user: user} do
      character = character_fixture(user)
      conn = get(conn, ~p"/api/characters/#{character.name}")

      assert %{"status" => "success", "character" => data} = json_response(conn, 200)
      assert data["name"] == character.name
      assert data["vocation"] == character.vocation
      assert data["experience"] == character.experience
      assert data["magic_level"] == character.magic_level
      assert data["melee_skill"] == character.melee_skill
      assert data["distance_skill"] == character.distance_skill
      assert data["shielding_skill"] == character.shielding_skill
    end

    test "returns not found for non-existent character", %{conn: conn} do
      conn = get(conn, ~p"/api/characters/non-existent")
      assert %{"status" => "error", "message" => "Character not found"} = json_response(conn, 404)
    end

    test "can view other user's character", %{conn: conn} do
      other_user = user_fixture()
      character = character_fixture(other_user)

      conn = get(conn, ~p"/api/characters/#{character.name}")
      assert %{"status" => "success", "character" => data} = json_response(conn, 200)
      assert data["name"] == character.name
    end

    test "gets character with items", %{conn: conn, user: user} do
      character = character_fixture(user)
      character_items_fixture(character)

      conn = get(conn, ~p"/api/characters/#{character.name}")
      assert %{"status" => "success", "character" => data} = json_response(conn, 200)

      assert data["equipment"]
      assert data["inventory"]
      assert is_list(data["equipment"])
      assert is_list(data["inventory"])
    end
  end
end
