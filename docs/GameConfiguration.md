# Game Configuration

The server allow two ways to add new games to the server:
- Using JSON configuration
- Using folder structure

### Using JSON Configuration

The server uses a configuration file to load the games. The configuration file is a JSON file that contains multiple required fields:
- `name`: the name of the game
- `version`: the version of the game
- `url`: the URL of the game repository
- `description`: a brief description of the game
- `license`: the license of the game (e.g., MIT, GPL, ...)
- `authors`: a list of authors that contributed to the game

Exemple of a game configuration file:
```json
{
    "title": "Game_01",
    "version": "1.0.0",
    "url": "https://github.com/Author/Game_01",
    "description": "A brief description of the game",
    "license": "MIT",
    "authors": [
        {
            "name": "Author"
        }
    ]
}
```

### Using Folder Structure

To add a new game to the server, you need to create a new directory in the `Games` directory that contains the game's configuration file and scripts if needed.

The expected architecture of the games list directory:
```shell
Games
├── Game_01
│   ├── assets # not used by the server
│   ├── scripts # if needed
│   ├── other_folder # used by the config file
│   └── config.cfg
└── Game_02
    └── config.cfg
```
