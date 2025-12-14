defmodule FibulaSite.Repo.Migrations.CreateServers do
  use Ecto.Migration

  def change do
    create table(:servers) do
      add :address, :string, null: false
      add :port, :integer, null: false
      add :version, :string
      add :max_players, :integer
      add :players_online, :integer
      add :game_mode, :string
      add :last_update, :utc_datetime, null: false

      timestamps()
    end

    create unique_index(:servers, [:address, :port])
    create index(:servers, [:last_update])
    create index(:servers, [:game_mode])
  end
end
