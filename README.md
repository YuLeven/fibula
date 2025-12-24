# Fibula
![Project banner](https://github.com/YuLeven/fibula/blob/main/game-screenshot-cropped-16-9-min.png)

<a href="https://discord.com/invite/3vsPM6qvt3" target="_blank">
 <img src="https://cdn.prod.website-files.com/6257adef93867e50d84d30e2/67d00cf7266d2c75571aebde_Example.svg" alt="Join our discord" />
</a>

Join our discord


### Gameplay footage:

<a href="http://www.youtube.com/watch?feature=player_embedded&v=GKCt1Wqv20k" target="_blank">
 <img src="http://img.youtube.com/vi/GKCt1Wqv20k/mqdefault.jpg" alt="Watch the video" width="240" height="180" border="10" />
</a>

**Disclaimer:** Fibula is a Tibia-inspired 3D arena PvP game. It is an independent project and not affiliated with or endorsed by the original Tibia creators or publishers. Any resemblance to Tibia is intentional as inspiration only — Fibula is its own game with distinct mechanics, assets, and network code.

**Overview**
- Fibula is a 3D arena-style PvP game inspired by Tibia. The project contains a server (headless) and a client (regular game client), plus a small site for community/management purposes.

**Prerequisites**
- A supported platform for building Unreal Engine from source.
- Git LFS (if needed for large assets).
- Follow Epic's official instructions to build Unreal Engine from source: https://dev.epicgames.com/documentation/en-us/unreal-engine/building-unreal-engine-from-source

Both the server and client builds in this repository depend on a locally-built Unreal Engine from source. Make sure you complete the UE source build and have the engine available before building Fibula.

**Building the Server**
- The server must be compiled as a headless (server) build. We provide a helper script to drive the build: `build_server.sh`.
- Typical steps:
  1. Ensure the Unreal Engine 5.5 source is built and available on your machine per Epic's documentation.
  2. From the repo root, run the server build helper:

```bash
./build_server.sh
```

  3. The script will call UnrealBuildTool/UE build targets for a headless server configuration (for example `-Target=FibulaServer -Platform=Win64 -Configuration=Shipping -BuildServer`).
  4. After completion, server binaries will be available under `Binaries/Win64` (or the platform-specific Binaries folder).

If you need to run a manual build with Unreal tools, make sure to use a server target and a headless build configuration (often referenced as Server or Headless in UE tooling).

**Building the Client**
- The client is built as a regular client/standalone game build.
- Steps:
  1. Confirm Unreal Engine from source is built and registered with your system.
  2. Use the normal Unreal build flow (Editor or UBT) to build the client targets (for example `FibulaClient` for Win64/Linux) or rely on your IDE/project files created by `GenerateProjectFiles`.
  3. Example using UnrealBuildTool (manual):

```bash
# from repo root, using UBT or your platform's build wrapper
./Engine/Build/BatchFiles/RunUAT.sh BuildCookRun -project="${PWD}/Fibula.uproject" -noP4 -client -platform=Win64 -build
```

Adjust the exact command to match your platform and the engine layout created when you built UE from source.

**Site / Web Deployment**

The site is used as a login server, and needs to be running for players to be able to join the game.

**Useful Scripts**
- `build_server.sh` — helper wrapper to build the server as a headless server build.
- `deploy_site.sh` — deploy or start the site component for this project.
- `docker-compose up` — runs a development version of the site on docker for ease of run

**Troubleshooting & Notes**
- Building Unreal Engine from source can take significant disk space and time. Follow Epic's docs closely for prerequisites (Visual Studio, SDKs, etc.).
- If a script fails, inspect the script for required environment variables or paths pointing to your locally-built engine.
- Logs and build artifacts are placed under `Saved/`, `Intermediate/`, and `Binaries/` — check those folders for build output and errors.

**Contributing & Contact**
- If you want to contribute, open issues or pull requests against this repository.
- For questions about building or deployment, include OS, Unreal Engine commit/branch, and any relevant log excerpts when you open an issue.





