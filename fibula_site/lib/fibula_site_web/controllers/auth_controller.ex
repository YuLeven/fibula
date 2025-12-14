defmodule FibulaSiteWeb.AuthController do
  use FibulaSiteWeb, :controller

  alias FibulaSite.Accounts
  alias FibulaSite.Accounts.User

  def register(conn, %{"email" => email, "password" => password}) do
    case Accounts.register_user(%{email: email, password: password}) do
      {:ok, user} ->
        token = Accounts.generate_user_session_token(user)
        token_string = Base.encode64(token)

        conn
        |> put_status(:created)
        |> json(%{
          status: "success",
          data: %{
            email: user.email,
            token: token_string
          }
        })

      {:error, %Ecto.Changeset{} = changeset} ->
        errors = format_changeset_errors(changeset)

        conn
        |> put_status(:unprocessable_entity)
        |> json(%{
          status: "error",
          errors: errors
        })
    end
  end

  def register(conn, _params) do
    conn
    |> put_status(:bad_request)
    |> json(%{
      status: "error",
      message: "Email and password are required"
    })
  end

  def login(conn, %{"email" => email, "password" => password}) do
    case Accounts.get_user_by_email_and_password(email, password) do
      %User{} = user ->
        token = Accounts.generate_user_session_token(user)
        token_string = Base.encode64(token)

        conn
        |> put_status(:ok)
        |> json(%{
          status: "success",
          data: %{
            email: user.email,
            token: token_string
          }
        })

      nil ->
        conn
        |> put_status(:unauthorized)
        |> json(%{
          status: "error",
          message: "Invalid email or password"
        })
    end
  end

  defp format_changeset_errors(changeset) do
    Ecto.Changeset.traverse_errors(changeset, fn {msg, opts} ->
      Regex.replace(~r"%{(\w+)}", msg, fn _, key ->
        opts |> Keyword.get(String.to_existing_atom(key), key) |> to_string()
      end)
    end)
  end
end
