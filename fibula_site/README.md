# FibulaSite

A Phoenix web application for the Fibula game project.

## Prerequisites

- **Elixir 1.14+** and **Erlang/OTP**
- **PostgreSQL 15+** (if running without Docker)
- **Docker and Docker Compose** (if running with Docker)

## Running with Docker Compose (Recommended)

The easiest way to get started is using Docker Compose, which automatically sets up PostgreSQL and the Phoenix application.

### Steps:

1. **Start the application**:
   ```bash
   docker-compose up
   ```

   This will:
   - Start a PostgreSQL 15 container
   - Build and start the Phoenix application
   - Set up the database and run migrations
   - Start the server on port 4000

2. **Access the application**:
   - Visit [http://localhost:4000](http://localhost:4000) from your browser

3. **Stop the application**:
   ```bash
   docker-compose down
   ```

4. **To rebuild after code changes**:
   ```bash
   docker-compose up --build
   ```

## Running without Docker

If you prefer to run the application directly on your system:

### 1. Install Dependencies

Ensure you have Elixir, Erlang, and PostgreSQL installed on your system.

### 2. Configure Database

Make sure PostgreSQL is running and create a database user:

```bash
# Connect to PostgreSQL
psql -U postgres

# Create user and database (in psql)
CREATE USER fibula_site WITH PASSWORD 'fibula_site';
CREATE DATABASE fibula_site_dev OWNER fibula_site;
\q
```

### 3. Install Application Dependencies

```bash
# Install Hex and Rebar
mix local.hex --force
mix local.rebar --force

# Install dependencies
mix deps.get
```

### 4. Setup Database

```bash
# Create database and run migrations
mix ecto.setup
```

Alternatively, you can run these commands separately:
```bash
mix ecto.create      # Create the database
mix ecto.migrate     # Run migrations
```

### 5. Start the Phoenix Server

```bash
# Start the server
mix phx.server

# Or start with an interactive Elixir shell
iex -S mix phx.server
```

### 6. Access the Application

Visit [http://localhost:4000](http://localhost:4000) from your browser.

## Development

### Database Management

```bash
# Create the database
mix ecto.create

# Run migrations
mix ecto.migrate

# Rollback last migration
mix ecto.rollback

# Reset database (drop, create, migrate)
mix ecto.reset

# Drop database
mix ecto.drop
```

### Running Tests

```bash
mix test
```

### Code Formatting

```bash
# Check formatting
mix format --check-formatted

# Format code
mix format
```

## Production Deployment

Please [check our deployment guides](https://hexdocs.pm/phoenix/deployment.html).

