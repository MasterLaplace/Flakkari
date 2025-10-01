# ME : Flakkari

<p align="center">
    <img src="docs/Images/Flakkari.png" alt="Flakkari logo" style="height: 200px">
</p>

<p align="center">
    <h3 align="center">📦 Download &#xa0; : &#xa0;
    <a href="https://github.com/MasterLaplace/Flakkari/releases/latest/">
        <img src="https://img.shields.io/github/v/release/MasterLaplace/Flakkari.svg?label=Latest%20Version&style=for-the-badge" alt="Latest Version">
    </a>&#xa0; | &#xa0;
    📜 License &#xa0; : &#xa0;
    <a href="https://github.com/MasterLaplace/Flakkari/blob/main/LICENSE">
        <img src="https://img.shields.io/badge/License-MIT-brightgreen.svg?style=for-the-badge" alt="License: MIT">
    </a>
    </h3>
</p>
<p align="center">
    <h3 align="center">🌐 Supported Platforms
    &#xa0; : &#xa0;
    <img
        src="https://img.shields.io/badge/Linux-blue?style=for-the-badge"
        alt="Linux deb"
        href="https://github.com/MasterLaplace/Flakkari/releases/latest/download/flakkari-linux.deb"
    > &#xa0; | &#xa0;
    <img
        src="https://img.shields.io/badge/Windows-blue?style=for-the-badge"
        alt="Windows"
        href="https://github.com/MasterLaplace/Flakkari/releases/latest/download/flakkari-win64.exe"
    > &#xa0; | &#xa0;
    <img
        src="https://img.shields.io/badge/MacOS-blue?style=for-the-badge"
        alt="MacOS"
        href="https://github.com/MasterLaplace/Flakkari/releases/latest/download/flakkari-macos.pkg"
    >
    </h3>
</p>
<p align="center">
    <h3 align="center">📡 Supported Protocol
        &#xa0; : &#xa0;
        <img src="https://img.shields.io/badge/UDP-blue?style=for-the-badge" alt="UDP">
        &#xa0; | &#xa0;
        <img src="https://img.shields.io/badge/TCP-blue?style=for-the-badge" alt="TCP">
    </h3>
</p>

---

## :placard: Table of Contents

- [Description](#description)
  - [Games Request](#video_game-games-request)
- [Running](#running)
  - [Environment Setup](#gear-environment-setup)
  - [Build and Run](#rocket-build-and-run)
  - [Build Modes](#gear-build-modes)
  - [Build Commands](#hammer-build-commands)
  - [Package Installation](#package-installation)
  - [Environment Variables](#earth_asia-environment-variables)
- [Docker](#docker)
- [Documentation](#documentation)
- [Latest Release](#latest-release)
- [License](#license)
- [Contacts](#contacts)

---

<div id='description'/>

## :pencil: **DESCRIPTION**

Flakkari is a UDP/TCP server initially developed for the R-Type Epitech project and updated for the Video Games course at University Laval. It enables network-based gameplay, supporting multiple games and clients simultaneously through its multi-threaded architecture. The server is designed to handle high concurrency and ensure low latency, making it suitable for real-time multiplayer games. Flakkari also includes features such as game state synchronization, player authentication, and robust error handling to provide a seamless gaming experience.

**🔄 Auto-Update Feature:** Flakkari supports automatic game downloads from remote repositories, allowing the server to fetch and update games dynamically. This feature can be disabled for environments requiring pure MIT licensing.

Flakkari also has a [Unity client](Libraries/Flakkari4Unity/README.md) library to facilitate the development of multiplayer games. This library is available in C# and can be used to connect to the Flakkari server, send and receive messages and manage game events. It is designed to be easy to integrate into existing game projects and provides advanced features to simplify the development of multiplayer games.

> [!NOTE]
> The project is still under development and may not be stable.

<div id='video_game-games-request'/>

### :video_game: **GAMES REQUEST**

We provide a game request system that allows you to request a game to be added to the server. To do so, please open an [Github Issues](https://github.com/MasterLaplace/Flakkari/issues/new/choose). Select the `Game Request` template and provide the information following the template.
We will gladly review your request and add the game to the server if it meets the requirements.

The available games are listed in the [Games](Games) directory. Each game has its own configuration file. The server will load the games from this directory and make them available to clients. The main advantage of this system is that you can add new games without modifying the server code. The server will automatically update the list of available games when it starts or when a new game is added/removed/modified in the remote directory.

And if you want to add a new game, you can follow the [Game Configuration](docs/GameConfiguration.md) guide.

> [!WARNING]
> Before submitting a game request, please make sure that the game you are requesting is not already in the repository.
> Also, be aware that the game you are requesting must be open-source and must comply with the Code Of Conduct.

---

<div id='running'/>

## :truck: **RUNNING**

<div id='gear-environment-setup'/>

#### :gear: **ENVIRONMENT SETUP**

Before running the server, you need to set the game directory environment variable:

```shell
# Set the games directory environment variable
export FLAKKARI_GAME_DIR="/path/to/your/Games"

# For the current project structure:
export FLAKKARI_GAME_DIR="$(pwd)/Games"

# Or use absolute path:
export FLAKKARI_GAME_DIR="/home/user/Flakkari/Games"
```

> [!NOTE]
> **FLAKKARI_GAME_DIR:** This environment variable specifies the default directory containing game configurations. It's used as a fallback when no game directory is provided via command-line arguments.

<div id='rocket-build-and-run'/>

#### :rocket: **BUILD AND RUN**

```shell
# Clone repository
$> git clone https://github.com/MasterLaplace/Flakkari.git
$> cd Flakkari

# Set environment variable
$> export FLAKKARI_GAME_DIR="$(pwd)/Games"

# Build with XMake (recommended, cross-platform)
$> xmake

# Alternative: Build with CMake
$> mkdir build && cd build
$> cmake .. && cmake --build .

# Run the server executable
$> xmake run
# Or manually: ./build/linux/x86_64/release/flakkari

# Alternative: Run with explicit arguments (no env variable needed):
$> ./build/linux/x86_64/release/flakkari -g Games -i localhost -p 8081
# XMake: or on Windows: .\build\windows\x64\release\flakkari.exe -g Games -i localhost -p 8081
# CMake: or from build directory: ./flakkari -g Games -i localhost -p 8081
```

To run the server with

- the games directory: `./Games` that contains the games configurations

- the IP address: `localhost`

- the port: `8081`

expected architecture of the games list directory:

```shell
Games
├── Game_01.json
├── Game_01
│   └── config.cfg
├── Game_02.json
└── Game_02
    └── config.cfg
```

<div id='gear-build-modes'/>

#### :gear: **BUILD MODES**

Flakkari supports two compilation modes to accommodate different license requirements:

**🔄 Full Mode (with auto-update) - Default:**

```shell
# Enable auto-update functionality (default)
$> xmake config --with-autoupdate=true
$> xmake

# Features: Automatic game downloads from remote repositories
# Dependencies: libgit2 (GPL-2.0), libcurl, OpenSSL 3.0+
# License: MIT + GPL dependencies for auto-update feature
```

**🪶 Lightweight Mode (MIT-only):**

```shell
# Disable auto-update for pure MIT license
$> xmake config --with-autoupdate=false
$> xmake

# Features: Core server functionality only
# Dependencies: nlohmann_json, singleton (MIT)
# License: Pure MIT, no GPL dependencies
```

> [!NOTE]
> **License Considerations:** The auto-update feature uses libgit2 (GPL-2.0-only). When enabled, the resulting binary includes GPL dependencies. For commercial use or MIT-only deployments, use `--with-autoupdate=false` to build a lightweight version with pure MIT licensing.

<div id='hammer-build-commands'/>

#### :hammer: **BUILD COMMANDS**

**XMake commands:**

```shell
# Clean build files
$> xmake clean

# Build in debug mode
$> xmake f -m debug && xmake

# Build in release mode (default)
$> xmake f -m release && xmake

# Install the project
$> xmake install

# Package the project
$> xmake pack

# Run tests (if available)
$> xmake test
```

**Alternative CMake commands:**

```shell
# Create build directory
$> mkdir build && cd build

# Configure the project
(build)$> cmake .. && cmake --build .

# Or configure with Ninja
(build)$> cmake -G Ninja .. && cmake --build .

# Install the project
(build)$> sudo cmake --build . --target install

# Build documentation
(build)$> cmake --build . --target doc

# Build package
(build)$> cmake --build . --config Release --target package
```

<div id='package-installation'/>

#### 📦 **PACKAGE INSTALLATION**

```bash
# Install the package
# For windows systems
(build)$> flakkari-win64.exe

# For macos systems
(build)$> sudo installer -pkg flakkari-macos.pkg -target /

# For redhat based systems
(build)$> sudo rpm -i flakkari-linux.rpm

# For debian based systems
(build)$> sudo dpkg -i flakkari-linux.deb
```

<div id='earth_asia-environment-variables'/>

#### :earth_asia: **ENVIRONMENT VARIABLES**

| Variable | Description | Example | Required |
|----------|-------------|---------|----------|
| `FLAKKARI_GAME_DIR` | Default directory containing game configurations | `./Games` or `/path/to/games` | Yes* |

> [!NOTE]
> **\*Required when:** No game directory is specified via `-g/--games` command-line argument.

The `FLAKKARI_GAME_DIR` environment variable serves as a fallback location for game configurations when not explicitly provided through command-line arguments.

---

<div id='docker'/>

## :whale: **DOCKER RUN**

```bash
# install docker on ubuntu
$ sudo apt-get update
$ sudo apt-get install docker-ce docker-ce-cli containerd.io

# check docker version (optional)
$ docker --version
> Docker version 24.0.7, build afdd53b

# build docker image
$ docker build -t flakkari .

# run docker image
$ docker run -p 8081:8081 -it flakkari

# list docker images
$ docker ps

# stop docker image
$ docker stop <container-id>

# remove docker image
$ docker rm <container-id>
```

---

<div id='documentation'/>

## :wrench: **DOCUMENTATION**

Wiki:

- [**Technical Documentation & Wiki**](https://MasterLaplace.github.io/Flakkari/)

For detailed documentation on the Flakkari protocol, see:

- [**Flakkari Protocol Documentation**](docs/RFC.txt)

---

## :rocket: LATEST RELEASE

> :clipboard: [Change Log](CHANGELOG.md) and [Security Policy](SECURITY.md)

- [![Latest Release](https://img.shields.io/github/v/release/MasterLaplace/Flakkari.svg?label=version)](https://github.com/MasterLaplace/Flakkari/releases/latest/)

---

<div id='license'/>

## :scroll: **LICENSE**

This project is licensed under the terms of the [MIT License](./LICENSE).

**License Considerations:**

- **Core Flakkari Library:** MIT License
- **Auto-Update Feature:** When enabled (`--with-autoupdate=true`), includes libgit2 (GPL-2.0-only) dependency
- **Lightweight Build:** Use `--with-autoupdate=false` for pure MIT licensing without GPL dependencies

Copyright © 2023-2024 [Master_Laplace](https://github.com/MasterLaplace).

---

<div id='contacts'/>

## :mailbox_with_mail: **CONTACTS**

This server project has been carried out by:

<table align="center">
    <tbody>
        <tr>
            <td align="center"><a href="https://github.com/MasterLaplace/"><img src="https://avatars.githubusercontent.com/MasterLaplace?v=4?s=100" width="100px;" alt="MasterLaplace"/><br/><a href="https://github.com/MasterLaplace/"><img src="https://img.shields.io/github/followers/MasterLaplace?label=Master_Laplace&style=social" alt="Master_Laplace"/></a></td>
        </tr>
    </tbody>
</table>

---

<p align="center">ME.inc</p>
