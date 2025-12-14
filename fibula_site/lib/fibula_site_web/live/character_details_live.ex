defmodule FibulaSiteWeb.CharacterDetailsLive do
  use FibulaSiteWeb, :live_view

  alias FibulaSite.Accounts
  import FibulaSiteWeb.Layouts.Nav

  defp item_image_name(name) do
    name
    |> String.downcase()
    |> String.replace(" ", "_")
    |> Kernel.<>("_icon")
  end

  @impl true
  def mount(%{"name" => name}, _session, socket) do
    case Accounts.get_character_by_name(name) |> Accounts.preload_character() do
      nil ->
        {:ok, socket |> redirect(to: ~p"/")}

      character ->
        other_characters =
          Accounts.list_user_characters(character.user) |> Enum.reject(&(&1.id == character.id))

        equipment = Accounts.list_character_equipment(character)

        {:ok,
         assign(socket,
           character: character,
           other_characters: other_characters,
           equipment: equipment
         )}
    end
  end

  @impl true
  def render(assigns) do
    ~H"""
    <div class="bg-gray-900 min-h-screen">
      <div class="relative isolate overflow-hidden">
        <div class="absolute inset-0 -z-10 bg-gradient-to-b from-gray-900 via-gray-900 to-purple-900">
        </div>
         <.navbar />
        <div class="relative z-10 mx-auto max-w-7xl px-6 py-12">
          <div class="sm:flex sm:items-center mb-12">
            <div class="sm:flex-auto">
              <h1 class="font-martel text-4xl tracking-tight text-amber-500 font-bold">
                {@character.name}
              </h1>
            </div>
          </div>
          
          <div class="mt-8 flow-root">
            <div class="overflow-x-auto">
              <div class="inline-block min-w-full align-middle">
                <div class="overflow-hidden shadow-xl rounded-xl bg-gray-800/50 border border-gray-700 p-6">
                  <dl class="grid grid-cols-1 gap-x-4 gap-y-8 sm:grid-cols-2">
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Vocation</dt>
                      
                      <dd class="mt-1 text-sm text-white">{@character.vocation}</dd>
                    </div>
                    
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Max Level in a Game</dt>
                      
                      <dd class="mt-1 text-sm text-white">
                        {@character.highscore.max_level_per_game}
                      </dd>
                    </div>
                    
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Max Kills in a Game</dt>
                      
                      <dd class="mt-1 text-sm text-white">
                        {@character.highscore.max_kills_per_game}
                      </dd>
                    </div>
                    
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Total Kills</dt>
                      
                      <dd class="mt-1 text-sm text-white">{@character.highscore.kills}</dd>
                    </div>
                    
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Max Deaths in a Game</dt>
                      
                      <dd class="mt-1 text-sm text-white">
                        {@character.highscore.max_deaths_per_game}
                      </dd>
                    </div>
                    
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Total Deaths</dt>
                      
                      <dd class="mt-1 text-sm text-white">{@character.highscore.deaths}</dd>
                    </div>
                    
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Last Seen</dt>
                      
                      <dd class="mt-1 text-sm text-white">
                        {Calendar.strftime(
                          @character.highscore.updated_at,
                          "%B %d, %Y at %I:%M %p UTC"
                        )}
                      </dd>
                    </div>
                    
                    <div class="sm:col-span-1">
                      <dt class="text-sm font-medium text-gray-400">Player since</dt>
                      
                      <dd class="mt-1 text-sm text-white">
                        {Calendar.strftime(@character.user.inserted_at, "%B %d, %Y")}
                      </dd>
                    </div>
                  </dl>
                </div>
              </div>
            </div>
          </div>
          
          <div class="mt-8">
            <h2 class="text-xl font-semibold text-amber-500 mb-4">Equipment</h2>
            
            <div class="overflow-hidden shadow-xl rounded-xl bg-gray-800/50 border border-gray-700 p-6">
              <div class="w-[200px] mx-auto">
                <div class="grid grid-cols-3 gap-1">
                  <!-- Row 1: Amulet, Helmet, Backpack -->
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if amulet = Enum.find(@equipment, & &1.slot_type == "amulet") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(amulet.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={amulet.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if helm = Enum.find(@equipment, & &1.slot_type == "helm") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(helm.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={helm.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if bag = Enum.find(@equipment, & &1.slot_type == "bag") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(bag.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={bag.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
    <!-- Row 2: Weapon, Armor, Shield -->
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if weapon = Enum.find(@equipment, & &1.slot_type == "weapon") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(weapon.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={weapon.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if armor = Enum.find(@equipment, & &1.slot_type == "armor") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(armor.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={armor.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if shield = Enum.find(@equipment, & &1.slot_type == "shield") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(shield.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={shield.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
    <!-- Row 3: Ring, Legs, Ammunition -->
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if ring = Enum.find(@equipment, & &1.slot_type == "ring") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(ring.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={ring.item_name}
                        />
                        <%= if ring.charges > 0 do %>
                          <div class="absolute bottom-0 right-0 text-xs text-white bg-gray-900/80 px-1">
                            {ring.charges}
                          </div>
                        <% end %>
                      <% end %>
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if legs = Enum.find(@equipment, & &1.slot_type == "legs") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(legs.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={legs.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if ammo = Enum.find(@equipment, & &1.slot_type == "ammunition") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(ammo.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={ammo.item_name}
                        />
                        <%= if ammo.stack_count > 1 do %>
                          <div class="absolute bottom-0 right-0 text-xs text-white bg-gray-900/80 px-1">
                            {ammo.stack_count}
                          </div>
                        <% end %>
                      <% end %>
                    </div>
                  </div>
                  
    <!-- Row 4: Empty, Boots, Empty -->
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                      <%= if boots = Enum.find(@equipment, & &1.slot_type == "boots") do %>
                        <img
                          src={~p"/images/items/#{item_image_name(boots.item_name)}" <> ".png"}
                          class="w-[50px] h-[50px]"
                          alt={boots.item_name}
                        />
                      <% end %>
                    </div>
                  </div>
                  
                  <div>
                    <div class="relative w-[50px] h-[50px] border border-gray-700 bg-gray-900/50">
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
          
          <%= if length(@other_characters) > 0 do %>
            <div class="mt-8">
              <h2 class="text-xl font-semibold text-amber-500 mb-4">Other Characters</h2>
              
              <div class="grid grid-cols-1 gap-4 sm:grid-cols-2 lg:grid-cols-3">
                <%= for character <- @other_characters do %>
                  <div class="relative flex items-center space-x-3 rounded-lg border border-gray-700 bg-gray-800/50 px-6 py-5 shadow-sm hover:border-amber-500">
                    <div class="min-w-0 flex-1">
                      <.link navigate={~p"/characters/#{character.name}"} class="focus:outline-none">
                        <span class="absolute inset-0" aria-hidden="true" />
                        <p class="text-sm font-medium text-white">{character.name}</p>
                        
                        <p class="truncate text-sm text-gray-400">{character.vocation}</p>
                      </.link>
                    </div>
                  </div>
                <% end %>
              </div>
            </div>
          <% end %>
        </div>
      </div>
    </div>
    """
  end
end
