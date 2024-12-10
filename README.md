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


## :placard: Table of Contents
- [Description](#description)
- [Running](#running)
- [Docker](#docker)
- [Documentation](#documentation)
- [Latest Release](#latest-release)
- [License](#license)
- [Contacts](#contacts)


<div id='description'/>

### :pencil: **DESCRIPTION**

Flakkari is a UDP/TCP server initially developed for the R-Type Epitech project and updated for the Video Games course at University Laval. It enables network-based gameplay, supporting multiple games and clients simultaneously through its multi-threaded architecture. The server is designed to handle high concurrency and ensure low latency, making it suitable for real-time multiplayer games. Flakkari also includes features such as game state synchronization, player authentication, and robust error handling to provide a seamless gaming experience.

Flakkari also has a [Unity client](Libraries/Flakkari4Unity/README.md) library to facilitate the development of multiplayer games. This library is available in C# and can be used to connect to the Flakkari server, send and receive messages and manage game events. It is designed to be easy to integrate into existing game projects and provides advanced features to simplify the development of multiplayer games.

> [!NOTE]
> The project is still under development and may not be stable.

#### :video_game: **GAMES REQUEST**

We provide a game request system that allows you to request a game to be added to the server. To do so, please open an [Github Issues](https://github.com/MasterLaplace/Flakkari/issues/new/choose). Select the `Game Request` template and provide the information following the template.
We will gladly review your request and add the game to the server if it meets the requirements.

The available games are listed in the [Games](Games) directory. Each game has its own configuration file. The server will load the games from this directory and make them available to clients. The main advantage of this system is that you can add new games without modifying the server code. The server will automatically update the list of available games when it starts or when a new game is added/removed/modified in the remote directory.

And if you want to add a new game, you can follow the [Game Configuration](docs/GameConfiguration.md) guide.

> [!WARNING]
> Before submitting a game request, please make sure that the game you are requesting is not already in the repository.
> Also, be aware that the game you are requesting must be open-source and must comply with the Code Of Conduct.


<div id='running'/>

### :truck: **RUNNING**

```shell
# Clone repository
$> git clone https://github.com/MasterLaplace/Flakkari.git
$> cd Flakkari

# Create a build directory
$> mkdir build
$> cd build

# Configure the project
(build)$> cmake .. && cmake --build .

# Or configure the project with Ninja
(build)$> cmake -G Ninja .. && cmake --build .

# Run the server executable
(build)$> ./flakkari <GamesDir> <ip> <port>
```

To run the server with
- the games directory: `./Games` that contains the games configurations

- the IP address: `localhost`

- the port: `8081`

expected architecture of the games list directory:

```shell
Games
├── Game_01
│   ├── assets
│   └── config.cfg
└── Game_02
    └── config.cfg
```

#### :hammer: **BUILD COMMANDS**

other build commands:
```shell
# Install the project
(build)$> sudo cmake --build . --target install

# Build project documentation
(build)$> cmake --build . --target doc

# Build project package
(build)$> cmake --build . --config Release --target package

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

<div id='docker'/>

### :whale: **DOCKER RUN**

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


<div id='documentation'/>

### :wrench: **DOCUMENTATION**

Wiki:
- [**Technical Documentation & Wiki**](https://MasterLaplace.github.io/Flakkari/)

For detailed documentation on the Flakkari protocol, see:
- [**Flakkari Protocol Documentation**](docs/RFC.txt)


## :rocket: LATEST RELEASE

> :clipboard: [Change Log](CHANGELOG.md) and [Security Policy](SECURITY.md)

- [![Latest Release](https://img.shields.io/github/v/release/MasterLaplace/Flakkari.svg?label=version)](https://github.com/MasterLaplace/Flakkari/releases/latest/)


<div id='license'/>

## :scroll: **LICENSE**

This project is licensed under the terms of the [MIT License](./LICENSE).

Copyright © 2023-2024 [Master_Laplace](https://github.com/MasterLaplace).


<div id='contacts'/>

### :mailbox_with_mail: **CONTACTS**

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
