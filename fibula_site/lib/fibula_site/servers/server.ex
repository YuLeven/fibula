defmodule FibulaSite.Servers.Server do
  use Ecto.Schema
  import Ecto.Changeset
  import Ecto.Query

  schema "servers" do
    field :address, :string
    field :port, :integer
    field :version, :string
    field :max_players, :integer
    field :players_online, :integer
    field :game_mode, :string
    field :last_update, :utc_datetime
    has_many :online_players, FibulaSite.Accounts.Character, foreign_key: :online_in_id

    timestamps()
  end

  def changeset(server, attrs) do
    server
    |> cast(attrs, [
      :address,
      :port,
      :version,
      :max_players,
      :players_online,
      :game_mode,
      :last_update
    ])
    |> validate_required([:address, :port, :last_update])
    |> unique_constraint([:address, :port], name: :servers_address_port_index)
  end

  def active(query \\ __MODULE__) do
    cutoff = DateTime.utc_now() |> DateTime.add(-2 * 60)

    from(s in query,
      where: s.last_update > ^cutoff,
      preload: [:online_players]
    )
  end

  def with_online_players(query \\ __MODULE__) do
    from(s in query,
      preload: [:online_players]
    )
  end
end
