defmodule FibulaSite.AccountsFixtures do
  @moduledoc """
  This module defines test helpers for creating
  entities via the `FibulaSite.Accounts` context.
  """

  def unique_user_email, do: "user#{System.unique_integer()}@example.com"
  def valid_user_password, do: "hello world!"

  def valid_user_attributes(attrs \\ %{}) do
    Enum.into(attrs, %{
      email: unique_user_email(),
      password: valid_user_password()
    })
  end

  def user_fixture(attrs \\ %{}) do
    {:ok, user} =
      attrs
      |> valid_user_attributes()
      |> FibulaSite.Accounts.register_user()

    user
  end

  def extract_user_token(fun) do
    {:ok, captured_email} = fun.(&"[TOKEN]#{&1}[TOKEN]")
    [_, token | _] = String.split(captured_email.text_body, "[TOKEN]")
    token
  end

  def valid_character_attributes(attrs \\ %{}) do
    Enum.into(attrs, %{
      name:
        "Character#{:crypto.strong_rand_bytes(5) |> Base.encode64() |> binary_part(0, 5) |> String.replace(~r/[^a-zA-Z]/, "")}"
        |> String.capitalize(),
      vocation: "Knight",
      experience: 0,
      magic_level: 0,
      melee_skill: 10,
      distance_skill: 10,
      shielding_skill: 10
    })
  end

  def character_fixture(user \\ nil, attrs \\ %{}) do
    user = user || user_fixture()

    {:ok, character} =
      attrs
      |> valid_character_attributes()
      |> then(&FibulaSite.Accounts.create_character(user, &1))

    character
  end

  def valid_equipment_attributes do
    %{
      "helm" => %{"slot" => "helm", "item" => "Magician Hat", "count" => 1},
      "armor" => %{"slot" => "armor", "item" => "Blue Robe", "count" => 1},
      "legs" => %{"slot" => "legs", "item" => "Blue Legs", "count" => 1},
      "boots" => %{"slot" => "boots", "item" => "Sandals", "count" => 1}
    }
  end

  def valid_inventory_attributes do
    [
      %{"item" => "Mana Potion", "count" => 50},
      %{"item" => "Health Potion", "count" => 25},
      %{"item" => "Blank Rune", "count" => 10}
    ]
  end

  def valid_equipment_items do
    [
      %{"slot" => "helm", "item" => "Magician Hat", "count" => 1},
      %{"slot" => "armor", "item" => "Blue Robe", "count" => 1},
      %{"slot" => "legs", "item" => "Blue Legs", "count" => 1},
      %{"slot" => "boots", "item" => "Sandals", "count" => 1}
    ]
  end

  def valid_inventory_items do
    [
      %{"item" => "Mana Potion", "count" => 50},
      %{"item" => "Health Potion", "count" => 25},
      %{"item" => "Blank Rune", "count" => 10}
    ]
  end

  def character_items_fixture(character) do
    {:ok, _} =
      FibulaSite.Accounts.update_character_items(
        character.id,
        valid_equipment_items(),
        valid_inventory_items()
      )
  end
end
