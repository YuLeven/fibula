defmodule FibulaSiteWeb.Layouts.Nav do
  use Phoenix.Component
  import FibulaSiteWeb.CoreComponents

  def navbar(assigns) do
    ~H"""
    <nav class="relative z-20 border-b border-gray-700/50">
      <div class="mx-auto max-w-7xl px-6">
        <div class="flex h-16 items-center justify-between">
          <div class="flex items-center gap-8">
            <.link
              navigate="/"
              class="text-2xl font-martel font-bold text-amber-500 hover:text-amber-400 transition-colors duration-200"
            >
              Home
            </.link>
            
            <.link
              navigate="/highscores"
              class="text-2xl font-martel font-bold text-amber-500 hover:text-amber-400 transition-colors duration-200"
            >
              Highscores
            </.link>
            
            <.link
              navigate="/characters"
              class="text-2xl font-martel font-bold text-amber-500 hover:text-amber-400 transition-colors duration-200"
            >
              Characters
            </.link>
            
            <.link
              navigate="/online"
              class="text-2xl font-martel font-bold text-amber-500 hover:text-amber-400 transition-colors duration-200"
            >
              Online players
            </.link>
          </div>
        </div>
      </div>
    </nav>
    """
  end
end
