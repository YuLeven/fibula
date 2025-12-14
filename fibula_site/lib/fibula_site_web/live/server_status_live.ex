defmodule FibulaSiteWeb.ServerStatusLive do
  use FibulaSiteWeb, :live_view
  alias FibulaSite.Servers.ServerTracker
  import FibulaSiteWeb.Layouts.Nav

  def mount(_params, _session, socket) do
    if connected?(socket) do
      Phoenix.PubSub.subscribe(FibulaSite.PubSub, "server_status")
    end

    servers = ServerTracker.get_active_servers()
    {:ok, assign(socket, servers: servers)}
  end

  def handle_info({:servers_updated, servers}, socket) do
    {:noreply, assign(socket, servers: servers)}
  end

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
                Online Players
              </h1>
            </div>
          </div>

          <div class="mt-8 flow-root">
            <div class="grid gap-6 md:grid-cols-2 lg:grid-cols-3">
              <%= for server <- @servers do %>
                <div class="overflow-hidden shadow-xl rounded-xl bg-gray-800/50 border border-gray-700 p-6">
                  <div class="flex justify-between items-center mb-4">
                    <div class="text-right">
                      <div class="text-lg font-medium text-white">
                        {server.game_mode
                        |> String.replace(~r/(?<!^)(?=[A-Z])/, " ")
                        |> String.capitalize()}
                      </div>
                    </div>
                  </div>

                  <div class="flex justify-between items-center mb-4">
                    <span class="text-sm text-gray-400">
                      {server.players_online}/{server.max_players} Players
                    </span>
                  </div>

                  <div class="divide-y divide-gray-700">
                    <%= for player <- server.online_players do %>
                      <div class="py-2 flex justify-between items-center">
                        <div>
                          <.link
                            navigate={~p"/characters/#{player.name}"}
                            class="text-white font-medium hover:text-amber-500 transition-colors duration-200"
                          >
                            {player.name}
                          </.link>
                          <span class="text-sm text-gray-400 ml-2">{player.vocation}</span>
                        </div>
                      </div>
                    <% end %>
                  </div>
                </div>
              <% end %>
            </div>
          </div>
        </div>
      </div>
    </div>
    """
  end
end
