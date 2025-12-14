defmodule FibulaSite.Accounts.CharacterTest do
  use FibulaSite.DataCase

  alias FibulaSite.Accounts
  alias FibulaSite.Accounts.Character
  alias FibulaSite.Accounts.Highscore

  import FibulaSite.AccountsFixtures

  setup do
    %{user: user_fixture()}
  end

  describe "create_character/2" do
    test "creates character with valid attributes", %{user: user} do
      attrs = valid_character_attributes()
      assert {:ok, %Character{} = character} = Accounts.create_character(user, attrs)
      assert character.name == attrs.name
      assert character.vocation == attrs.vocation
      assert character.user_id == user.id

      # Verify highscore was created
      highscore = Repo.get_by(Highscore, character_id: character.id)
      assert highscore != nil
      assert highscore.kills == 0
      assert highscore.max_kills_per_game == 0
      assert highscore.deaths == 0
      assert highscore.max_deaths_per_game == 0
      assert highscore.max_level_per_game == 0
    end

    test "returns error with invalid attributes", %{user: user} do
      assert {:error, %Ecto.Changeset{}} = Accounts.create_character(user, %{})
    end

    test "enforces unique character names", %{user: user} do
      attrs = valid_character_attributes()
      assert {:ok, %Character{}} = Accounts.create_character(user, attrs)
      assert {:error, changeset} = Accounts.create_character(user, attrs)
      assert "has already been taken" in errors_on(changeset).name
    end

    test "validates character name format", %{user: user} do
      attrs = valid_character_attributes(name: "Invalid123")
      assert {:error, changeset} = Accounts.create_character(user, attrs)
      assert "only letters and spaces allowed" in errors_on(changeset).name
    end

    test "validates vocation", %{user: user} do
      attrs = valid_character_attributes(vocation: "InvalidVocation")
      assert {:error, changeset} = Accounts.create_character(user, attrs)
      assert "is invalid" in errors_on(changeset).vocation
    end

    test "enforces character limit per account", %{user: user} do
      # Create 6 characters
      for _i <- 1..6 do
        {:ok, _character} = Accounts.create_character(user, valid_character_attributes())
      end

      # Try to create a 7th character
      assert {:error, :character_limit_reached} =
               Accounts.create_character(user, valid_character_attributes())
    end

    test "does not create new highscore when character is updated", %{user: user} do
      attrs = valid_character_attributes()
      {:ok, character} = Accounts.create_character(user, attrs)

      # Get the initial highscore
      initial_highscore = Repo.get_by(Highscore, character_id: character.id)
      assert initial_highscore != nil

      # Update the character
      {:ok, _updated_character} =
        character
        |> Character.changeset(%{experience: 1000})
        |> Repo.update()

      # Verify no new highscore was created
      highscores = Repo.all(from h in Highscore, where: h.character_id == ^character.id)
      assert length(highscores) == 1
      assert Enum.at(highscores, 0).id == initial_highscore.id
    end
  end

  describe "list_user_characters/1" do
    test "returns all user characters", %{user: user} do
      character = character_fixture(user)
      assert [returned_character] = Accounts.list_user_characters(user)
      assert returned_character.id == character.id
    end

    test "doesn't return other users' characters", %{user: user} do
      other_user = user_fixture()
      _other_character = character_fixture(other_user)
      assert [] = Accounts.list_user_characters(user)
    end
  end

  describe "get_character_by_name/1" do
    test "returns character with matching name", %{user: user} do
      character = character_fixture(user)
      assert %Character{} = returned_character = Accounts.get_character_by_name(character.name)
      assert returned_character.id == character.id
    end

    test "returns nil with non-existent name" do
      assert is_nil(Accounts.get_character_by_name("NonExistentCharacter"))
    end
  end

  describe "delete_character/1" do
    test "deletes the character", %{user: user} do
      character = character_fixture(user)
      assert {:ok, %Character{}} = Accounts.delete_character(character)
      assert is_nil(Accounts.get_character_by_name(character.name))
    end

    test "deletes associated highscore", %{user: user} do
      character = character_fixture(user)
      highscore = Repo.get_by(Highscore, character_id: character.id)
      assert highscore != nil

      assert {:ok, _} = Accounts.delete_character(character)
      assert is_nil(Repo.get(Highscore, highscore.id))
    end
  end

  describe "update_character_items/3" do
    test "updates character items successfully", %{user: user} do
      character = character_fixture(user)
      equipment = Map.values(valid_equipment_attributes())
      inventory = valid_inventory_attributes()

      assert {:ok, _} = Accounts.update_character_items(character.id, equipment, inventory)

      items = Accounts.get_character_items(character.id)
      assert length(items.equipment) == length(equipment)
      assert length(items.inventory) == length(inventory)
    end

    test "returns error for non-existent character" do
      equipment = Map.values(valid_equipment_attributes())
      inventory = valid_inventory_attributes()

      assert_raise Postgrex.Error, fn ->
        Accounts.update_character_items(-1, equipment, inventory)
      end
    end
  end
end
