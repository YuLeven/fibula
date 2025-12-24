#!/bin/sh

set -e

echo "Waiting for PostgreSQL to be ready..."
until pg_isready -h $DB_HOST -U fibula_site -d fibula_site_dev; do
  sleep 1
done

echo "Getting dependencies..."
mix deps.get

echo "Creating database..."
mix ecto.create

echo "Running migrations..."
mix ecto.migrate

echo "Starting Phoenix server..."
exec mix phx.server
