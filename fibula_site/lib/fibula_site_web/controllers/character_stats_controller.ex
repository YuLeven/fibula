defmodule FibulaSiteWeb.API.CharacterStatsController do
  use FibulaSiteWeb, :controller

  alias FibulaSite.Accounts

  def update(conn, %{"characters" => characters}) do
    case Accounts.update_character_stats_bulk(characters) do
      {:ok, _highscores} ->
        json(conn, %{status: "success"})

      {:error, :not_found, character_names} ->
        conn
        |> put_status(:not_found)
        |> json(%{
          status: "error",
          message: "Characters not found",
          character_names: character_names
        })
    end
  end
end
