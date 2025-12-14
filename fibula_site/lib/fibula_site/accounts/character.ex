defmodule FibulaSite.Accounts.Character do
  use Ecto.Schema
  import Ecto.Changeset

  schema "characters" do
    field :name, :string
    field :vocation, :string
    field :experience, :integer, default: 0
    field :magic_level, :integer, default: 0
    field :melee_skill, :integer, default: 0
    field :distance_skill, :integer, default: 0
    field :shielding_skill, :integer, default: 0

    belongs_to :user, FibulaSite.Accounts.User
    belongs_to :online_in, FibulaSite.Servers.Server
    has_many :equipment, FibulaSite.Accounts.CharacterEquipment
    has_many :inventory, FibulaSite.Accounts.CharacterInventory
    has_one :highscore, FibulaSite.Accounts.Highscore

    timestamps()
  end

  @doc false
  def changeset(character, attrs) do
    character
    |> cast(attrs, [
      :name,
      :vocation,
      :experience,
      :magic_level,
      :melee_skill,
      :distance_skill,
      :shielding_skill,
      :online_in_id,
      :user_id
    ])
    |> update_change(:name, &capitalize_name/1)
    |> validate_required([:name, :vocation, :user_id])
    |> validate_length(:name, min: 2, max: 20)
    |> validate_format(:name, ~r/^[a-zA-Z\s]+$/, message: "only letters and spaces allowed")
    |> validate_inclusion(:vocation, ["Knight", "Paladin", "Sorcerer", "Druid"])
    |> validate_number(:experience, greater_than_or_equal_to: 0)
    |> validate_number(:magic_level, greater_than_or_equal_to: 0)
    |> validate_number(:melee_skill, greater_than_or_equal_to: 0)
    |> validate_number(:distance_skill, greater_than_or_equal_to: 0)
    |> validate_number(:shielding_skill, greater_than_or_equal_to: 0)
    |> unique_constraint(:name)
  end

  defp capitalize_name(name) when is_binary(name) do
    name
    |> String.split(" ")
    |> Enum.map(&String.capitalize/1)
    |> Enum.join(" ")
  end

  defp capitalize_name(name), do: name
end
