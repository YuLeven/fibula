defmodule FibulaSite.Repo.Migrations.CreateCharacterItems do
  use Ecto.Migration

  def change do
    create table(:character_equipment) do
      add :character_id, references(:characters, on_delete: :delete_all), null: false
      # helm, armor, weapon, shield, etc.
      add :slot_type, :string, null: false
      add :item_name, :string, null: false
      add :charges, :integer, default: 0
      add :stack_count, :integer, default: 1

      timestamps()
    end

    create table(:character_inventory) do
      add :character_id, references(:characters, on_delete: :delete_all), null: false
      add :item_name, :string, null: false
      add :stack_count, :integer, default: 1
      add :charges, :integer, default: 0
      # To maintain order in inventory
      add :position, :integer, null: false

      timestamps()
    end

    create index(:character_equipment, [:character_id])
    create index(:character_inventory, [:character_id])
  end
end
