defmodule FibulaSiteWeb.CharacterSearchLive do
  use FibulaSiteWeb, :live_view
  import FibulaSiteWeb.Layouts.Nav
  alias FibulaSite.Accounts

  @impl true
  def mount(_params, _session, socket) do
    {:ok, assign(socket, query: "", error_message: nil)}
  end

  @impl true
  def handle_event("search", %{"query" => query}, socket) do
    cond do
      String.length(query) == 0 ->
        {:noreply, assign(socket, error_message: "Please enter a character name")}

      character = Accounts.get_character_by_name(query) ->
        {:noreply, push_navigate(socket, to: ~p"/characters/#{query}")}

      true ->
        {:noreply,
         assign(socket,
           error_message: "Character \"#{query}\" not found",
           query: query
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
        <div class="relative z-10 mx-auto max-w-2xl px-6 py-12">
          <div class="sm:flex sm:items-center mb-12">
            <div class="sm:flex-auto">
              <h1 class="font-martel text-4xl tracking-tight text-amber-500 font-bold">
                Character Search
              </h1>
            </div>
          </div>
          
          <div class="mt-8">
            <div class="overflow-hidden shadow-xl rounded-xl bg-gray-800/50 border border-gray-700 p-6">
              <form phx-submit="search" class="space-y-4">
                <div>
                  <label for="query" class="block text-sm font-medium text-gray-200">
                    Character Name
                  </label>
                  
                  <div class="mt-1 flex gap-x-4">
                    <input
                      type="text"
                      name="query"
                      id="query"
                      class="block flex-1 rounded-xl border-gray-700 bg-gray-900 shadow-sm focus:border-amber-500 focus:ring-amber-500 sm:text-sm text-white placeholder-gray-500"
                      placeholder="Enter character name"
                      value={@query}
                      autocomplete="off"
                    />
                    <button
                      type="submit"
                      class="inline-flex justify-center rounded-xl bg-amber-500 px-6 py-2 text-sm font-semibold text-white shadow-sm hover:bg-amber-400 focus-visible:outline focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-amber-500 transition-colors duration-200"
                    >
                      Search
                    </button>
                  </div>
                </div>
                
                <%= if @error_message do %>
                  <p class="mt-2 text-sm font-medium text-white text-rose-500">
                    {@error_message}
                  </p>
                <% end %>
              </form>
            </div>
          </div>
        </div>
      </div>
    </div>
    """
  end
end
