defmodule FibulaSite.Accounts do
  @moduledoc """
  The Accounts context.
  """

  import Ecto.Query, warn: false
  alias FibulaSite.Repo

  alias FibulaSite.Accounts.{
    User,
    UserToken,
    UserNotifier,
    Character,
    CharacterEquipment,
    CharacterInventory,
    PlayerStartingElements,
    Highscore
  }

  ## Database getters

  @doc """
  Gets a user by email.

  ## Examples

      iex> get_user_by_email("foo@example.com")
      %User{}

      iex> get_user_by_email("unknown@example.com")
      nil

  """
  def get_user_by_email(email) when is_binary(email) do
    Repo.get_by(User, email: email)
  end

  @doc """
  Gets a user by email and password.

  ## Examples

      iex> get_user_by_email_and_password("foo@example.com", "correct_password")
      %User{}

      iex> get_user_by_email_and_password("foo@example.com", "invalid_password")
      nil

  """
  def get_user_by_email_and_password(email, password)
      when is_binary(email) and is_binary(password) do
    user = Repo.get_by(User, email: email)
    if User.valid_password?(user, password), do: user
  end

  @doc """
  Gets a single user.

  Raises `Ecto.NoResultsError` if the User does not exist.

  ## Examples

      iex> get_user!(123)
      %User{}

      iex> get_user!(456)
      ** (Ecto.NoResultsError)

  """
  def get_user!(id), do: Repo.get!(User, id)

  ## User registration

  @doc """
  Registers a user.

  ## Examples

      iex> register_user(%{field: value})
      {:ok, %User{}}

      iex> register_user(%{field: bad_value})
      {:error, %Ecto.Changeset{}}

  """
  def register_user(attrs) do
    %User{}
    |> User.registration_changeset(attrs)
    |> Repo.insert()
  end

  @doc """
  Returns an `%Ecto.Changeset{}` for tracking user changes.

  ## Examples

      iex> change_user_registration(user)
      %Ecto.Changeset{data: %User{}}

  """
  def change_user_registration(%User{} = user, attrs \\ %{}) do
    User.registration_changeset(user, attrs, hash_password: false, validate_email: false)
  end

  ## Settings

  @doc """
  Returns an `%Ecto.Changeset{}` for changing the user email.

  ## Examples

      iex> change_user_email(user)
      %Ecto.Changeset{data: %User{}}

  """
  def change_user_email(user, attrs \\ %{}) do
    User.email_changeset(user, attrs, validate_email: false)
  end

  @doc """
  Emulates that the email will change without actually changing
  it in the database.

  ## Examples

      iex> apply_user_email(user, "valid password", %{email: ...})
      {:ok, %User{}}

      iex> apply_user_email(user, "invalid password", %{email: ...})
      {:error, %Ecto.Changeset{}}

  """
  def apply_user_email(user, password, attrs) do
    user
    |> User.email_changeset(attrs)
    |> User.validate_current_password(password)
    |> Ecto.Changeset.apply_action(:update)
  end

  @doc """
  Updates the user email using the given token.

  If the token matches, the user email is updated and the token is deleted.
  The confirmed_at date is also updated to the current time.
  """
  def update_user_email(user, token) do
    context = "change:#{user.email}"

    with {:ok, query} <- UserToken.verify_change_email_token_query(token, context),
         %UserToken{sent_to: email} <- Repo.one(query),
         {:ok, _} <- Repo.transaction(user_email_multi(user, email, context)) do
      :ok
    else
      _ -> :error
    end
  end

  defp user_email_multi(user, email, context) do
    changeset =
      user
      |> User.email_changeset(%{email: email})
      |> User.confirm_changeset()

    Ecto.Multi.new()
    |> Ecto.Multi.update(:user, changeset)
    |> Ecto.Multi.delete_all(:tokens, UserToken.by_user_and_contexts_query(user, [context]))
  end

  @doc ~S"""
  Delivers the update email instructions to the given user.

  ## Examples

      iex> deliver_user_update_email_instructions(user, current_email, &url(~p"/users/settings/confirm_email/#{&1}"))
      {:ok, %{to: ..., body: ...}}

  """
  def deliver_user_update_email_instructions(%User{} = user, current_email, update_email_url_fun)
      when is_function(update_email_url_fun, 1) do
    {encoded_token, user_token} = UserToken.build_email_token(user, "change:#{current_email}")

    Repo.insert!(user_token)
    UserNotifier.deliver_update_email_instructions(user, update_email_url_fun.(encoded_token))
  end

  @doc """
  Returns an `%Ecto.Changeset{}` for changing the user password.

  ## Examples

      iex> change_user_password(user)
      %Ecto.Changeset{data: %User{}}

  """
  def change_user_password(user, attrs \\ %{}) do
    User.password_changeset(user, attrs, hash_password: false)
  end

  @doc """
  Updates the user password.

  ## Examples

      iex> update_user_password(user, "valid password", %{password: ...})
      {:ok, %User{}}

      iex> update_user_password(user, "invalid password", %{password: ...})
      {:error, %Ecto.Changeset{}}

  """
  def update_user_password(user, password, attrs) do
    changeset =
      user
      |> User.password_changeset(attrs)
      |> User.validate_current_password(password)

    Ecto.Multi.new()
    |> Ecto.Multi.update(:user, changeset)
    |> Ecto.Multi.delete_all(:tokens, UserToken.by_user_and_contexts_query(user, :all))
    |> Repo.transaction()
    |> case do
      {:ok, %{user: user}} -> {:ok, user}
      {:error, :user, changeset, _} -> {:error, changeset}
    end
  end

  ## Session

  @doc """
  Generates a session token.
  """
  def generate_user_session_token(user) do
    {token, user_token} = UserToken.build_session_token(user)
    Repo.insert!(user_token)
    token
  end

  @doc """
  Gets the user with the given signed token.
  """
  def get_user_by_session_token(token) do
    {:ok, query} = UserToken.verify_session_token_query(token)
    Repo.one(query)
  end

  @doc """
  Deletes the signed token with the given context.
  """
  def delete_user_session_token(token) do
    Repo.delete_all(UserToken.by_token_and_context_query(token, "session"))
    :ok
  end

  ## Confirmation

  @doc ~S"""
  Delivers the confirmation email instructions to the given user.

  ## Examples

      iex> deliver_user_confirmation_instructions(user, &url(~p"/users/confirm/#{&1}"))
      {:ok, %{to: ..., body: ...}}

      iex> deliver_user_confirmation_instructions(confirmed_user, &url(~p"/users/confirm/#{&1}"))
      {:error, :already_confirmed}

  """
  def deliver_user_confirmation_instructions(%User{} = user, confirmation_url_fun)
      when is_function(confirmation_url_fun, 1) do
    if user.confirmed_at do
      {:error, :already_confirmed}
    else
      {encoded_token, user_token} = UserToken.build_email_token(user, "confirm")
      Repo.insert!(user_token)
      UserNotifier.deliver_confirmation_instructions(user, confirmation_url_fun.(encoded_token))
    end
  end

  @doc """
  Confirms a user by the given token.

  If the token matches, the user account is marked as confirmed
  and the token is deleted.
  """
  def confirm_user(token) do
    with {:ok, query} <- UserToken.verify_email_token_query(token, "confirm"),
         %User{} = user <- Repo.one(query),
         {:ok, %{user: user}} <- Repo.transaction(confirm_user_multi(user)) do
      {:ok, user}
    else
      _ -> :error
    end
  end

  defp confirm_user_multi(user) do
    Ecto.Multi.new()
    |> Ecto.Multi.update(:user, User.confirm_changeset(user))
    |> Ecto.Multi.delete_all(:tokens, UserToken.by_user_and_contexts_query(user, ["confirm"]))
  end

  ## Reset password

  @doc ~S"""
  Delivers the reset password email to the given user.

  ## Examples

      iex> deliver_user_reset_password_instructions(user, &url(~p"/users/reset_password/#{&1}"))
      {:ok, %{to: ..., body: ...}}

  """
  def deliver_user_reset_password_instructions(%User{} = user, reset_password_url_fun)
      when is_function(reset_password_url_fun, 1) do
    {encoded_token, user_token} = UserToken.build_email_token(user, "reset_password")
    Repo.insert!(user_token)
    UserNotifier.deliver_reset_password_instructions(user, reset_password_url_fun.(encoded_token))
  end

  @doc """
  Gets the user by reset password token.

  ## Examples

      iex> get_user_by_reset_password_token("validtoken")
      %User{}

      iex> get_user_by_reset_password_token("invalidtoken")
      nil

  """
  def get_user_by_reset_password_token(token) do
    with {:ok, query} <- UserToken.verify_email_token_query(token, "reset_password"),
         %User{} = user <- Repo.one(query) do
      user
    else
      _ -> nil
    end
  end

  @doc """
  Resets the user password.

  ## Examples

      iex> reset_user_password(user, %{password: "new long password", password_confirmation: "new long password"})
      {:ok, %User{}}

      iex> reset_user_password(user, %{password: "valid", password_confirmation: "not the same"})
      {:error, %Ecto.Changeset{}}

  """
  def reset_user_password(user, attrs) do
    Ecto.Multi.new()
    |> Ecto.Multi.update(:user, User.password_changeset(user, attrs))
    |> Ecto.Multi.delete_all(:tokens, UserToken.by_user_and_contexts_query(user, :all))
    |> Repo.transaction()
    |> case do
      {:ok, %{user: user}} -> {:ok, user}
      {:error, :user, changeset, _} -> {:error, changeset}
    end
  end

  def create_character(user, attrs \\ %{}) do
    # First check the character count
    character_count =
      Repo.one(from c in Character, where: c.user_id == ^user.id, select: count(c.id))

    if character_count >= 6 do
      {:error, :character_limit_reached}
    else
      Ecto.Multi.new()
      |> Ecto.Multi.insert(:character, fn _ ->
        user
        |> Ecto.build_assoc(:characters)
        |> Character.changeset(attrs)
      end)
      |> Ecto.Multi.insert(:highscore, fn %{character: character} ->
        Highscore.changeset(%Highscore{}, %{
          kills: 0,
          max_kills_per_game: 0,
          deaths: 0,
          max_deaths_per_game: 0,
          max_level_per_game: 0,
          character_id: character.id
        })
      end)
      |> Ecto.Multi.insert_all(:equipment, CharacterEquipment, fn %{character: character} ->
        vocation = attrs["vocation"] || attrs[:vocation]
        equipment = PlayerStartingElements.get_starting_equipment(vocation)

        Enum.map(equipment, fn %{slot: slot, item: item, count: count} ->
          %{
            character_id: character.id,
            slot_type: slot,
            item_name: item,
            stack_count: count,
            inserted_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second),
            updated_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second)
          }
        end)
      end)
      |> Ecto.Multi.insert_all(:inventory, CharacterInventory, fn %{character: character} ->
        vocation = attrs["vocation"] || attrs[:vocation]
        inventory = PlayerStartingElements.get_starting_inventory(vocation)

        Enum.with_index(inventory, fn %{item: item, count: count}, idx ->
          %{
            character_id: character.id,
            item_name: item,
            stack_count: count,
            position: idx,
            inserted_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second),
            updated_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second)
          }
        end)
      end)
      |> Repo.transaction()
      |> case do
        {:ok, %{character: character}} -> {:ok, character}
        {:error, :character, changeset, _} -> {:error, changeset}
        {:error, _, error, _} -> {:error, error}
      end
    end
  end

  @doc """
  Gets a character by name.

  Returns nil if the character does not exist.
  """
  def get_character_by_name(name) when is_binary(name) do
    Repo.get_by(Character, name: name)
  end

  @doc """
  Preloads character associations.
  """
  def preload_character(nil), do: nil

  def preload_character(character) do
    Repo.preload(character, [:user, :highscore])
  end

  @doc """
  Returns the list of characters for a given user.
  """
  def list_user_characters(user) do
    Character
    |> where(user_id: ^user.id)
    |> Repo.all()
  end

  @doc """
  Deletes a character.
  """
  def delete_character(%Character{} = character) do
    Repo.delete(character)
  end

  def update_character_items(character_id, equipment, inventory) do
    # First validate all equipment items using the model's changeset
    equipment_valid? =
      Enum.all?(equipment, fn item ->
        %CharacterEquipment{}
        |> CharacterEquipment.changeset(%{
          slot_type: item["slot"],
          item_name: item["item"],
          stack_count: item["count"],
          charges: item["charges"] || 0
        })
        |> Ecto.Changeset.apply_action(:validate)
        |> case do
          {:ok, _} -> true
          {:error, _} -> false
        end
      end)

    if equipment_valid? do
      Repo.transaction(fn ->
        Repo.delete_all(from e in CharacterEquipment, where: e.character_id == ^character_id)
        Repo.delete_all(from i in CharacterInventory, where: i.character_id == ^character_id)

        # Insert new equipment
        equipment_params =
          Enum.map(equipment, fn item ->
            %{
              character_id: character_id,
              slot_type: item["slot"],
              item_name: item["item"],
              stack_count: item["count"],
              charges: item["charges"] || 0,
              inserted_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second),
              updated_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second)
            }
          end)

        # Insert new inventory
        inventory_params =
          Enum.with_index(inventory, fn item, idx ->
            %{
              character_id: character_id,
              item_name: item["item"],
              stack_count: item["count"],
              charges: item["charges"] || 0,
              position: idx,
              inserted_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second),
              updated_at: NaiveDateTime.utc_now() |> NaiveDateTime.truncate(:second)
            }
          end)

        Repo.insert_all(CharacterEquipment, equipment_params)
        Repo.insert_all(CharacterInventory, inventory_params)
      end)
    else
      {:error, :invalid_equipment}
    end
  end

  def get_character_items(character_id) do
    equipment =
      CharacterEquipment
      |> where(character_id: ^character_id)
      |> Repo.all()
      |> Enum.map(
        &%{
          slot: &1.slot_type,
          item: &1.item_name,
          count: &1.stack_count,
          charges: &1.charges
        }
      )

    inventory =
      CharacterInventory
      |> where(character_id: ^character_id)
      |> order_by(:position)
      |> Repo.all()
      |> Enum.map(
        &%{
          item: &1.item_name,
          count: &1.stack_count,
          charges: &1.charges
        }
      )

    %{equipment: equipment, inventory: inventory}
  end

  @doc """
  Updates character statistics and highscores.
  """
  def update_character_stats(character_id, stats) when is_map(stats) do
    character = Repo.get(Character, character_id) |> Repo.preload(:highscore)

    if character do
      character.highscore
      |> Highscore.update_stats_changeset(stats)
      |> Repo.update()
    else
      {:error, :not_found}
    end
  end

  @doc """
  Updates multiple character statistics and highscores in a single transaction.
  """
  def update_character_stats_bulk(stats_list) do
    character_names = Enum.map(stats_list, & &1["character_name"])

    characters =
      Repo.all(from c in Character, where: c.name in ^character_names, preload: :highscore)

    found_names = MapSet.new(characters, & &1.name)
    requested_names = MapSet.new(character_names)
    missing_names = MapSet.difference(requested_names, found_names)

    if MapSet.size(missing_names) > 0 do
      {:error, :not_found, MapSet.to_list(missing_names)}
    else
      stats_by_name = Map.new(stats_list, &{&1["character_name"], &1})

      multi =
        Enum.reduce(characters, Ecto.Multi.new(), fn character, multi ->
          stats = stats_by_name[character.name]

          current_highscore =
            character.highscore ||
              %Highscore{
                character_id: character.id,
                kills: 0,
                deaths: 0,
                max_kills_per_game: 0,
                max_deaths_per_game: 0,
                max_level_per_game: 0
              }

          # Calculate new values
          new_kills = current_highscore.kills + (stats["kills"] || 0)
          new_deaths = current_highscore.deaths + (stats["deaths"] || 0)
          new_max_kills = max(current_highscore.max_kills_per_game, stats["match_kills"] || 0)
          new_max_deaths = max(current_highscore.max_deaths_per_game, stats["match_deaths"] || 0)
          new_max_level = max(current_highscore.max_level_per_game, stats["level"] || 0)

          changeset =
            Highscore.changeset(current_highscore, %{
              kills: new_kills,
              deaths: new_deaths,
              max_kills_per_game: new_max_kills,
              max_deaths_per_game: new_max_deaths,
              max_level_per_game: new_max_level
            })

          Ecto.Multi.insert_or_update(multi, {:highscore, character.id}, changeset)
        end)

      case Repo.transaction(multi) do
        {:ok, results} ->
          {:ok, Enum.map(results, fn {{:highscore, _}, highscore} -> highscore end)}

        {:error, _, changeset, _} ->
          {:error, changeset}
      end
    end
  end

  @doc """
  Lists highscores with pagination and sorting.
  """
  def list_highscores(paginate_options, sort_options) do
    Highscore
    |> join(:inner, [h], c in assoc(h, :character))
    |> preload([h, c], character: c)
    |> order_by([h, _], [{^sort_options.sort_order, ^sort_options.sort_by}])
    |> Repo.paginate(paginate_options)
  end

  def list_top_highscores do
    Highscore
    |> preload(:character)
    |> order_by([h], desc: h.max_level_per_game)
    |> limit(5)
    |> Repo.all()
  end

  @doc """
  Returns a paginated list of highscores.
  """

  def list_character_equipment(character) do
    Repo.all(from e in CharacterEquipment, where: e.character_id == ^character.id)
  end
end
