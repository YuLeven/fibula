defmodule FibulaSiteWeb.PageController do
  use FibulaSiteWeb, :controller

  alias FibulaSite.Accounts
  alias FibulaSite.Servers.ServerTracker

  def home(conn, _params) do
    top_highscores = Accounts.list_top_highscores()
    render(conn, :home, top_highscores: top_highscores)
  end

  def privacy(conn, _params) do
    render(conn, :privacy)
  end
end
