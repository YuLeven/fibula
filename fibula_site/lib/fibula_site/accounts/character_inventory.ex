defmodule FibulaSite.Accounts.CharacterInventory do
  use Ecto.Schema
  import Ecto.Changeset

  schema "character_inventory" do
    field :item_name, :string
    field :stack_count, :integer, default: 1
    field :charges, :integer, default: 0
    field :position, :integer
    belongs_to :character, FibulaSite.Accounts.Character

    timestamps()
  end

  def changeset(inventory, attrs) do
    inventory
    |> cast(attrs, [:item_name, :stack_count, :charges, :position])
    |> validate_required([:item_name, :position])
    |> validate_number(:stack_count, greater_than: 0)
    |> validate_number(:charges, greater_than_or_equal_to: 0)
    |> validate_number(:position, greater_than_or_equal_to: 0)
  end
end
