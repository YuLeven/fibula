defmodule FibulaSite.Accounts.Highscore do
  use Ecto.Schema
  import Ecto.Changeset

  schema "highscores" do
    field :kills, :integer, default: 0
    field :max_kills_per_game, :integer, default: 0
    field :deaths, :integer, default: 0
    field :max_deaths_per_game, :integer, default: 0
    field :max_level_per_game, :integer, default: 0
    belongs_to :character, FibulaSite.Accounts.Character

    timestamps()
  end

  def changeset(highscore, attrs) do
    highscore
    |> cast(attrs, [
      :kills,
      :max_kills_per_game,
      :deaths,
      :max_deaths_per_game,
      :max_level_per_game,
      :character_id
    ])
    |> validate_required([
      :kills,
      :max_kills_per_game,
      :deaths,
      :max_deaths_per_game,
      :max_level_per_game,
      :character_id
    ])
    |> validate_number(:kills, greater_than_or_equal_to: 0)
    |> validate_number(:max_kills_per_game, greater_than_or_equal_to: 0)
    |> validate_number(:deaths, greater_than_or_equal_to: 0)
    |> validate_number(:max_deaths_per_game, greater_than_or_equal_to: 0)
    |> validate_number(:max_level_per_game, greater_than_or_equal_to: 0)
    |> foreign_key_constraint(:character_id)
    |> unique_constraint(:character_id)
  end

  def update_stats_changeset(highscore, %{kills: kills, deaths: deaths, level: level}) do
    highscore
    |> cast(
      %{
        kills: highscore.kills + kills,
        deaths: highscore.deaths + deaths,
        max_kills_per_game: max(highscore.max_kills_per_game, kills),
        max_deaths_per_game: max(highscore.max_deaths_per_game, deaths),
        max_level_per_game: max(highscore.max_level_per_game, level)
      },
      [:kills, :deaths, :max_kills_per_game, :max_deaths_per_game, :max_level_per_game]
    )
    |> validate_required([
      :kills,
      :deaths,
      :max_kills_per_game,
      :max_deaths_per_game,
      :max_level_per_game
    ])
    |> validate_number(:kills, greater_than_or_equal_to: 0)
    |> validate_number(:deaths, greater_than_or_equal_to: 0)
    |> validate_number(:max_kills_per_game, greater_than_or_equal_to: 0)
    |> validate_number(:max_deaths_per_game, greater_than_or_equal_to: 0)
    |> validate_number(:max_level_per_game, greater_than_or_equal_to: 0)
  end
end
