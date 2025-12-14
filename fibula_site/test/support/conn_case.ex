defmodule FibulaSiteWeb.ConnCase do
  @moduledoc """
  This module defines the test case to be used by
  tests that require setting up a connection.

  Such tests rely on `Phoenix.ConnTest` and also
  import other functionality to make it easier
  to build common data structures and query the data layer.

  Finally, if the test case interacts with the database,
  we enable the SQL sandbox, so changes done to the database
  are reverted at the end of every test. If you are using
  PostgreSQL, you can even run database tests asynchronously
  by setting `use FibulaSiteWeb.ConnCase, async: true`, although
  this option is not recommended for other databases.
  """

  use ExUnit.CaseTemplate

  using do
    quote do
      # The default endpoint for testing
      @endpoint FibulaSiteWeb.Endpoint
      @backend_endpoint FibulaSiteWeb.BackendEndpoint

      use FibulaSiteWeb, :verified_routes
      alias FibulaSiteWeb.BackendRouter.Helpers, as: BackendRoutes

      # Import conveniences for testing with connections
      import Plug.Conn
      import Phoenix.ConnTest
      import FibulaSiteWeb.ConnCase

      alias FibulaSiteWeb.Router.Helpers, as: Routes
    end
  end

  def setup_backend_conn(conn) do
    conn
    |> Map.put(:secret_key_base, FibulaSiteWeb.BackendEndpoint.config(:secret_key_base))
    |> Map.put(:private, %{phoenix_endpoint: FibulaSiteWeb.BackendEndpoint})
  end

  setup tags do
    FibulaSite.DataCase.setup_sandbox(tags)

    conn = Phoenix.ConnTest.build_conn()

    backend_conn =
      Phoenix.ConnTest.build_conn()
      |> setup_backend_conn()

    {:ok, conn: conn, backend_conn: backend_conn}
  end

  @doc """
  Setup helper that registers and logs in users.

      setup :register_and_log_in_user

  It stores an updated connection and a registered user in the
  test context.
  """
  def register_and_log_in_user(%{conn: conn, backend_conn: backend_conn}) do
    user = FibulaSite.AccountsFixtures.user_fixture()
    %{conn: log_in_user(conn, user), backend_conn: log_in_user(backend_conn, user), user: user}
  end

  @doc """
  Logs the given `user` into the `conn`.

  It returns an updated `conn`.
  """
  def log_in_user(conn, user) do
    token = FibulaSite.Accounts.generate_user_session_token(user)

    conn
    |> Phoenix.ConnTest.init_test_session(%{})
    |> Plug.Conn.put_session(:user_token, token)
  end
end
