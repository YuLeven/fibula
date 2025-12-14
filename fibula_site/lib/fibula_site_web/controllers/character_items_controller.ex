defmodule FibulaSiteWeb.API.CharacterItemsController do
  use FibulaSiteWeb, :controller

  alias FibulaSite.Accounts

  def update(conn, %{
        "name" => name,
        "items" => %{"equipment" => equipment, "inventory" => inventory}
      }) do
    with %{id: character_id} = _character <- Accounts.get_character_by_name(name),
         {:ok, _} <- Accounts.update_character_items(character_id, equipment, inventory) do
      json(conn, %{status: "success"})
    else
      nil ->
        conn
        |> put_status(:not_found)
        |> json(%{status: "error", message: "Character not found"})

      {:error, :invalid_equipment} ->
        conn
        |> put_status(:unprocessable_entity)
        |> json(%{status: "error", message: "Invalid equipment slot"})

      {:error, _} ->
        conn
        |> put_status(:unprocessable_entity)
        |> json(%{status: "error", message: "Invalid request"})
    end
  end
end
