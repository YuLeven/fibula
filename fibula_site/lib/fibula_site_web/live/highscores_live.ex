defmodule FibulaSiteWeb.HighscoresLive do
  use FibulaSiteWeb, :live_view

  alias FibulaSite.Accounts
  import FibulaSiteWeb.Layouts.Nav

  @impl true
  def mount(_params, _session, socket) do
    {:ok, socket}
  end

  @impl true
  def handle_params(params, _url, socket) do
    page = String.to_integer(params["page"] || "1")
    sort_by = (params["sort_by"] || "max_level_per_game") |> String.to_atom()
    sort_order = (params["sort_order"] || "desc") |> String.to_atom()

    paginate_options = %{page: page, per_page: 50}
    sort_options = %{sort_by: sort_by, sort_order: sort_order}

    highscores = Accounts.list_highscores(paginate_options, sort_options)

    socket =
      assign(socket,
        highscores: highscores.entries,
        page: page,
        sort_by: sort_by,
        sort_order: sort_order,
        total_pages: highscores.total_pages
      )

    {:noreply, socket}
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
              <h1 class="font-martel text-4xl tracking-tight text-amber-500 font-bold">Highscores</h1>
            </div>
          </div>
          
          <div class="mt-8 flow-root">
            <div class="overflow-x-auto">
              <div class="inline-block min-w-full align-middle">
                <div class="overflow-hidden shadow-xl rounded-xl bg-gray-800/50 border border-gray-700">
                  <table class="min-w-full divide-y divide-gray-700/50">
                    <thead>
                      <tr>
                        <th
                          scope="col"
                          class="py-4 pl-6 pr-3 text-left text-sm font-semibold text-white"
                        >
                          Character
                        </th>
                        
                        <th scope="col" class="px-3 py-4 text-left text-sm font-semibold text-white">
                          Vocation
                        </th>
                        
                        <th scope="col" class="px-3 py-4 text-left text-sm font-semibold text-white">
                          <.sort_link
                            sort_by={:max_level_per_game}
                            sort_order={@sort_order}
                            current_sort={@sort_by}
                          >
                            Max Level
                          </.sort_link>
                        </th>
                        
                        <th scope="col" class="px-3 py-4 text-left text-sm font-semibold text-white">
                          <.sort_link
                            sort_by={:max_kills_per_game}
                            sort_order={@sort_order}
                            current_sort={@sort_by}
                          >
                            Single game kills
                          </.sort_link>
                        </th>
                        
                        <th scope="col" class="px-3 py-4 text-left text-sm font-semibold text-white">
                          <.sort_link
                            sort_by={:kills}
                            sort_order={@sort_order}
                            current_sort={@sort_by}
                          >
                            Total Kills
                          </.sort_link>
                        </th>
                        
                        <th scope="col" class="px-3 py-4 text-left text-sm font-semibold text-white">
                          <.sort_link
                            sort_by={:max_deaths_per_game}
                            sort_order={@sort_order}
                            current_sort={@sort_by}
                          >
                            Single game deaths
                          </.sort_link>
                        </th>
                        
                        <th scope="col" class="px-3 py-4 text-left text-sm font-semibold text-white">
                          <.sort_link
                            sort_by={:deaths}
                            sort_order={@sort_order}
                            current_sort={@sort_by}
                          >
                            Total Deaths
                          </.sort_link>
                        </th>
                      </tr>
                    </thead>
                    
                    <tbody class="divide-y divide-gray-700/50">
                      <%= for highscore <- @highscores do %>
                        <tr class="hover:bg-purple-900/20 transition-colors duration-200">
                          <td class="whitespace-nowrap py-4 pl-6 pr-3 text-sm font-medium text-amber-500">
                            <.link
                              navigate={~p"/characters/#{highscore.character.name}"}
                              class="hover:text-amber-400 transition-colors duration-200"
                            >
                              {highscore.character.name}
                            </.link>
                          </td>
                          
                          <td class="whitespace-nowrap px-3 py-4 text-sm text-gray-200">
                            {highscore.character.vocation}
                          </td>
                          
                          <td class="whitespace-nowrap px-3 py-4 text-sm text-gray-200">
                            {highscore.max_level_per_game}
                          </td>
                          
                          <td class="whitespace-nowrap px-3 py-4 text-sm text-gray-200">
                            {highscore.max_kills_per_game}
                          </td>
                          
                          <td class="whitespace-nowrap px-3 py-4 text-sm text-gray-200">
                            {highscore.kills}
                          </td>
                          
                          <td class="whitespace-nowrap px-3 py-4 text-sm text-gray-200">
                            {highscore.max_deaths_per_game}
                          </td>
                          
                          <td class="whitespace-nowrap px-3 py-4 text-sm text-gray-200">
                            {highscore.deaths}
                          </td>
                        </tr>
                      <% end %>
                    </tbody>
                  </table>
                </div>
              </div>
            </div>
          </div>
          
          <div class="mt-8 flex items-center justify-between">
            <div class="flex flex-1 justify-between sm:hidden">
              <.link
                :if={@page > 1}
                navigate={
                  ~p"/highscores?page=#{@page - 1}&sort_by=#{@sort_by}&sort_order=#{@sort_order}"
                }
                class="relative inline-flex items-center rounded-xl bg-purple-600 px-4 py-2 text-sm font-semibold text-white shadow-sm hover:bg-purple-500 transition-colors duration-200"
              >
                Previous
              </.link>
              
              <.link
                :if={@page < @total_pages}
                navigate={
                  ~p"/highscores?page=#{@page + 1}&sort_by=#{@sort_by}&sort_order=#{@sort_order}"
                }
                class="relative ml-3 inline-flex items-center rounded-xl bg-purple-600 px-4 py-2 text-sm font-semibold text-white shadow-sm hover:bg-purple-500 transition-colors duration-200"
              >
                Next
              </.link>
            </div>
            
            <div class="hidden sm:flex sm:flex-1 sm:items-center sm:justify-between">
              <div>
                <p class="text-sm text-gray-200">
                  Showing page <span class="font-medium text-amber-500">{@page}</span>
                  of <span class="font-medium text-amber-500">{@total_pages}</span>
                </p>
              </div>
              
              <div>
                <nav
                  class="isolate inline-flex -space-x-px rounded-xl shadow-sm"
                  aria-label="Pagination"
                >
                  <.link
                    :if={@page > 1}
                    navigate={
                      ~p"/highscores?page=#{@page - 1}&sort_by=#{@sort_by}&sort_order=#{@sort_order}"
                    }
                    class="relative inline-flex items-center rounded-l-xl bg-purple-600 px-4 py-2 text-sm font-semibold text-white shadow-sm hover:bg-purple-500 transition-colors duration-200"
                  >
                    <span class="sr-only">Previous</span>
                    <.icon name="hero-chevron-left-solid" class="h-5 w-5" />
                  </.link>
                  
                  <.link
                    :if={@page < @total_pages}
                    navigate={
                      ~p"/highscores?page=#{@page + 1}&sort_by=#{@sort_by}&sort_order=#{@sort_order}"
                    }
                    class="relative inline-flex items-center rounded-r-xl bg-purple-600 px-4 py-2 text-sm font-semibold text-white shadow-sm hover:bg-purple-500 transition-colors duration-200"
                  >
                    <span class="sr-only">Next</span>
                    <.icon name="hero-chevron-right-solid" class="h-5 w-5" />
                  </.link>
                </nav>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
    """
  end

  defp sort_link(assigns) do
    ~H"""
    <.link
      navigate={
        ~p"/highscores?sort_by=#{@sort_by}&sort_order=#{next_sort_order(@sort_by, @sort_order, @current_sort)}"
      }
      class="group inline-flex text-white hover:text-amber-500 transition-colors duration-200"
    >
      {render_slot(@inner_block)}
      <span class="ml-2 flex-none rounded text-gray-400 group-hover:text-amber-500">
        <%= if @sort_by == @current_sort do %>
          <%= if @sort_order == :asc do %>
            <.icon name="hero-arrow-up-solid" class="h-5 w-5" />
          <% else %>
            <.icon name="hero-arrow-down-solid" class="h-5 w-5" />
          <% end %>
        <% else %>
          <.icon name="hero-arrows-up-down-solid" class="h-5 w-5" />
        <% end %>
      </span>
    </.link>
    """
  end

  defp next_sort_order(sort_by, sort_order, current_sort) when sort_by == current_sort do
    case sort_order do
      :asc -> :desc
      :desc -> :asc
    end
  end

  defp next_sort_order(_sort_by, _sort_order, _current_sort), do: :desc
end
