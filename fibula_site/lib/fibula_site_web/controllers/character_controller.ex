defmodule FibulaSiteWeb.API.CharacterController do
  use FibulaSiteWeb, :controller

  alias FibulaSite.Accounts

  action_fallback FibulaSiteWeb.FallbackController

  def create(conn, %{"character" => character_params}) do
    user = conn.assigns.current_user

    case Accounts.create_character(user, character_params) do
      {:ok, character} ->
        conn
        |> put_status(:created)
        |> json(%{
          status: "success",
          character: %{
            name: character.name,
            vocation: character.vocation,
            experience: character.experience,
            magic_level: character.magic_level,
            melee_skill: character.melee_skill,
            distance_skill: character.distance_skill,
            shielding_skill: character.shielding_skill
          }
        })

      {:error, :character_limit_reached} ->
        conn
        |> put_status(:unprocessable_entity)
        |> json(%{
          status: "error",
          message: "Character limit reached. Maximum of 6 characters allowed per account."
        })

      {:error, changeset} ->
        conn
        |> put_status(:unprocessable_entity)
        |> json(%{
          status: "error",
          errors: format_changeset_errors(changeset)
        })
    end
  end

  def index(conn, _params) do
    user = conn.assigns.current_user
    characters = Accounts.list_user_characters(user)

    conn
    |> put_status(:ok)
    |> json(%{
      status: "success",
      characters:
        Enum.map(characters, fn char ->
          %{
            name: char.name,
            vocation: char.vocation,
            experience: char.experience,
            magic_level: char.magic_level,
            melee_skill: char.melee_skill,
            distance_skill: char.distance_skill,
            shielding_skill: char.shielding_skill
          }
        end)
    })
  end

  def delete(conn, %{"name" => name}) do
    user = conn.assigns.current_user

    with %{user_id: user_id} = character <- Accounts.get_character_by_name(name),
         true <- user_id == user.id,
         {:ok, _} <- Accounts.delete_character(character) do
      conn
      |> put_status(:ok)
      |> json(%{status: "success"})
    else
      nil ->
        conn
        |> put_status(:not_found)
        |> json(%{status: "error", message: "Character not found"})

      false ->
        conn
        |> put_status(:forbidden)
        |> json(%{status: "error", message: "Not authorized to delete this character"})

      {:error, _} ->
        conn
        |> put_status(:internal_server_error)
        |> json(%{status: "error", message: "Failed to delete character"})
    end
  end

  def show(conn, %{"id" => name}) do
    case Accounts.get_character_by_name(name) do
      nil ->
        conn
        |> put_status(:not_found)
        |> json(%{
          status: "error",
          message: "Character not found"
        })

      character ->
        items = Accounts.get_character_items(character.id)

        conn
        |> put_status(:ok)
        |> json(%{
          status: "success",
          character: %{
            name: character.name,
            vocation: character.vocation,
            experience: character.experience,
            magic_level: character.magic_level,
            melee_skill: character.melee_skill,
            distance_skill: character.distance_skill,
            shielding_skill: character.shielding_skill,
            equipment: items.equipment,
            inventory: items.inventory
          }
        })
    end
  end

  defp format_changeset_errors(changeset) do
    Ecto.Changeset.traverse_errors(changeset, fn {msg, opts} ->
      Regex.replace(~r"%{(\w+)}", msg, fn _, key ->
        opts |> Keyword.get(String.to_existing_atom(key), key) |> to_string()
      end)
    end)
  end
end
