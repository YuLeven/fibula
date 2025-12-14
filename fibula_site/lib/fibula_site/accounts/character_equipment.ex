defmodule FibulaSite.Accounts.CharacterEquipment do
  use Ecto.Schema
  import Ecto.Changeset

  schema "character_equipment" do
    field :slot_type, :string
    field :item_name, :string
    field :stack_count, :integer, default: 1
    field :charges, :integer, default: 0
    belongs_to :character, FibulaSite.Accounts.Character

    timestamps()
  end

  def changeset(equipment, attrs) do
    equipment
    |> cast(attrs, [:slot_type, :item_name, :stack_count, :charges])
    |> validate_required([:slot_type, :item_name])
    |> validate_inclusion(:slot_type, [
      "helm",
      "armor",
      "weapon",
      "shield",
      "legs",
      "boots",
      "amulet",
      "ring",
      "ammunition",
      "bag"
    ])
    |> validate_number(:stack_count, greater_than: 0)
    |> validate_number(:charges, greater_than_or_equal_to: 0)
  end
end
