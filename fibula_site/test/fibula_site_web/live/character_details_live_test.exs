defmodule FibulaSiteWeb.CharacterDetailsLiveTest do
  use FibulaSiteWeb.ConnCase

  import Phoenix.LiveViewTest
  import FibulaSite.AccountsFixtures

  describe "Character details" do
    test "displays character information", %{conn: conn} do
      user = user_fixture()
      {:ok, character} = create_character(user)
      {:ok, _view, html} = live(conn, ~p"/characters/#{character.name}")

      assert html =~ character.name
      assert html =~ character.vocation
      assert html =~ "Max Level in a Game"
      assert html =~ "Max Kills in a Game"
      assert html =~ "Total Kills"
      assert html =~ "Max Deaths in a Game"
      assert html =~ "Total Deaths"
      assert html =~ "Last Seen"
      assert html =~ "Player since"
    end

    test "displays other characters from the same account", %{conn: conn} do
      user = user_fixture()
      {:ok, character1} = create_character(user, %{name: "Tester One", vocation: "Knight"})
      {:ok, character2} = create_character(user, %{name: "Tester Two", vocation: "Paladin"})
      {:ok, _view, html} = live(conn, ~p"/characters/#{character1.name}")

      assert html =~ "Other Characters"
      assert html =~ character2.name
      assert html =~ character2.vocation
    end

    test "doesn't show other characters section when user has only one character", %{conn: conn} do
      user = user_fixture()
      {:ok, character} = create_character(user)
      {:ok, _view, html} = live(conn, ~p"/characters/#{character.name}")

      refute html =~ "Other Characters"
    end

    test "shows navigation links", %{conn: conn} do
      user = user_fixture()
      {:ok, character} = create_character(user)
      {:ok, _view, html} = live(conn, ~p"/characters/#{character.name}")

      assert html =~ "Home"
      assert html =~ "Highscores"
    end

    test "handles non-existent character", %{conn: conn} do
      assert {:error, {:redirect, %{to: "/"}}} = live(conn, ~p"/characters/NonExistentChar")
    end
  end

  defp create_character(user, attrs \\ %{}) do
    attrs =
      Enum.into(attrs, %{
        name: unique_character_name(),
        vocation: "Knight"
      })

    FibulaSite.Accounts.create_character(user, attrs)
  end

  defp unique_character_name do
    "Tester #{number_to_word(System.unique_integer([:positive]) |> rem(10))}"
  end

  defp number_to_word(0), do: "Zero"
  defp number_to_word(1), do: "One"
  defp number_to_word(2), do: "Two"
  defp number_to_word(3), do: "Three"
  defp number_to_word(4), do: "Four"
  defp number_to_word(5), do: "Five"
  defp number_to_word(6), do: "Six"
  defp number_to_word(7), do: "Seven"
  defp number_to_word(8), do: "Eight"
  defp number_to_word(9), do: "Nine"
end
