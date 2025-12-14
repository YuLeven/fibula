defmodule FibulaSite.Repo.Migrations.CreateCharacters do
  use Ecto.Migration

  def change do
    create table(:characters) do
      add :name, :string, null: false
      add :vocation, :string, null: false
      add :experience, :bigint, null: false, default: 0
      add :magic_level, :integer, null: false, default: 0
      add :melee_skill, :integer, null: false, default: 0
      add :distance_skill, :integer, null: false, default: 0
      add :shielding_skill, :integer, null: false, default: 0
      add :user_id, references(:users, on_delete: :delete_all), null: false
      add :online_in_id, references(:servers)

      timestamps()
    end

    create table(:highscores) do
      add :kills, :integer, null: false, default: 0
      add :max_kills_per_game, :integer, null: false, default: 0
      add :deaths, :integer, null: false, default: 0
      add :max_deaths_per_game, :integer, null: false, default: 0
      add :max_level_per_game, :integer, null: false, default: 0
      add :character_id, references(:characters, on_delete: :delete_all), null: false

      timestamps()
    end

    create unique_index(:characters, [:name])
    create index(:characters, [:user_id])
    create index(:characters, [:online_in_id])
    create unique_index(:highscores, [:character_id])
  end
end
