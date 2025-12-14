defmodule FibulaSite.Servers.ServerCleanup do
  use GenServer
  alias FibulaSite.Repo
  alias FibulaSite.Servers.Server
  alias FibulaSite.Accounts.Character
  import Ecto.Query

  def start_link(_opts) do
    GenServer.start_link(__MODULE__, %{}, name: __MODULE__)
  end

  def cleanup_stale_servers do
    cutoff = DateTime.utc_now() |> DateTime.add(-5 * 60)

    # First, clear online_in_id for characters in stale servers
    from(c in Character,
      join: s in Server,
      on: c.online_in_id == s.id,
      where: s.last_update < ^cutoff
    )
    |> Repo.update_all(set: [online_in_id: nil])

    # Then delete the stale servers
    from(s in Server, where: s.last_update < ^cutoff)
    |> Repo.delete_all()
  end

  @impl true
  def init(state) do
    schedule_cleanup()
    {:ok, state}
  end

  @impl true
  def handle_info(:cleanup, state) do
    cleanup_stale_servers()
    schedule_cleanup()
    {:noreply, state}
  end

  defp schedule_cleanup do
    Process.send_after(self(), :cleanup, :timer.minutes(1))
  end
end
