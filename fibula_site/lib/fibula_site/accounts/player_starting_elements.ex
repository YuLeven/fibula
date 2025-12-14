defmodule FibulaSite.Accounts.PlayerStartingElements do
  def get_starting_equipment(vocation) do
    case vocation do
      "Paladin" ->
        [
          %{slot: "weapon", item: "Elvish Bow", count: 1},
          %{slot: "ammunition", item: "Arrow", count: 800},
          %{slot: "helm", item: "Steel Helmet", count: 1},
          %{slot: "armor", item: "Plate Armor", count: 1},
          %{slot: "legs", item: "Plate Legs", count: 1},
          %{slot: "boots", item: "Sandals", count: 1},
          %{slot: "amulet", item: "Scarf", count: 1},
          %{slot: "bag", item: "Backpack", count: 1}
        ]

      "Druid" ->
        [
          %{slot: "shield", item: "Wooden Spellbook", count: 1},
          %{slot: "helm", item: "Fur Cap", count: 1},
          %{slot: "boots", item: "Sandals", count: 1},
          %{slot: "legs", item: "Mammoth Fur Shorts", count: 1},
          %{slot: "armor", item: "Mammoth Fur Cape", count: 1},
          %{slot: "amulet", item: "Scarf", count: 1},
          %{slot: "bag", item: "Backpack", count: 1},
          %{slot: "weapon", item: "Terra Rod", count: 1}
        ]

      "Sorcerer" ->
        [
          %{slot: "helm", item: "Magician Hat", count: 1},
          %{slot: "armor", item: "Magicians Robe", count: 1},
          %{slot: "legs", item: "Leather Pants", count: 1},
          %{slot: "boots", item: "Sandals", count: 1},
          %{slot: "shield", item: "Wooden Spellbook", count: 1},
          %{slot: "amulet", item: "Scarf", count: 1},
          %{slot: "bag", item: "Backpack", count: 1},
          %{slot: "weapon", item: "The Scorcher", count: 1}
        ]

      "Knight" ->
        [
          %{slot: "weapon", item: "Bright Sword", count: 1},
          %{slot: "armor", item: "Plate Armor", count: 1},
          %{slot: "legs", item: "Plate Legs", count: 1},
          %{slot: "boots", item: "Sandals", count: 1},
          %{slot: "shield", item: "Guardian Shield", count: 1},
          %{slot: "helm", item: "Steel Helmet", count: 1},
          %{slot: "amulet", item: "Scarf", count: 1},
          %{slot: "bag", item: "Backpack", count: 1}
        ]
    end
  end

  def get_starting_inventory(vocation) do
    case vocation do
      "Knight" ->
        [
          %{item: "War Hammer", count: 1}
        ]

      _ ->
        []
    end
  end
end
