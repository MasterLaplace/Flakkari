# Change Log

All notable changes to the "Flakkari" project will be documented in this file.

Check [Keep a Changelog](http://keepachangelog.com/) for recommendations on how to structure this file.

## [Unreleased]

### Added

- Game Request System and XMake build system #61 - by @MasterLaplace in [#61](https://github.com/MasterLaplace/Flakkari/pull/61)
- Define default parameters - by @MasterLaplace in [#67](https://github.com/MasterLaplace/Flakkari/pull/67)

### Fixed

### Changed

### Removed


## [0.5.0] - 2024-11-29

### Added

- Implement 3d components - by @MasterLaplace in [#49](https://github.com/MasterLaplace/Flakkari/pull/49)
- Implement flakkari protocol v1 - by @MasterLaplace in [#52](https://github.com/MasterLaplace/Flakkari/pull/52)
- Implement unity library - by @MasterLaplace in [#54](https://github.com/MasterLaplace/Flakkari/pull/54)
- Handle Camera's direction and Disconnect Request - by @MasterLaplace in [#56](https://github.com/MasterLaplace/Flakkari/pull/56)

### Fixed

- Build package and handle event - by @MasterLaplace in [#50](https://github.com/MasterLaplace/Flakkari/pull/50)

### Changed

- Update Flakkari4Unity Library to synchronize correctly with Protocol v1 - by @MasterLaplace in [#58](https://github.com/MasterLaplace/Flakkari/pull/58)

## [0.4.0] - 2024-11-08

### Added

- Include Singleton library - by @MasterLaplace in [#39](https://github.com/MasterLaplace/Flakkari/pull/39)
- Enhance GitHub workflows with build provenance and linter permissions - by @MasterLaplace in ([7026c57](https://github.com/MasterLaplace/Flakkari/commit/7026c5730a5adb2171a7e955a8aa43dfb538f056))
- Add Dependabot configuration - by @MasterLaplace in ([7026c57](https://github.com/MasterLaplace/Flakkari/commit/7026c5730a5adb2171a7e955a8aa43dfb538f056))

### Fixed

- Fix Create Release and Deploy Page - by @MasterLaplace in [#41](https://github.com/MasterLaplace/Flakkari/pull/41)
- Fix bug failed to connect - by @MasterLaplace in [#43](https://github.com/MasterLaplace/Flakkari/pull/43)
- Fix to get a constant connection using the protocol - by @MasterLaplace in [#45](https://github.com/MasterLaplace/Flakkari/pull/45)


## [0.3.0] - 2024-11-07

### Added

- Implement a complete payload for flakkari protocol - by @MasterLaplace in [#26](https://github.com/MasterLaplace/Flakkari/pull/26)
- Feature version bumper and release - by @MasterLaplace in [#33](https://github.com/MasterLaplace/Flakkari/pull/33)
- Feature deploy page on release - by @MasterLaplace in [#37](https://github.com/MasterLaplace/Flakkari/pull/37)
- Feature clang format linter - by @MasterLaplace in [#38](https://github.com/MasterLaplace/Flakkari/pull/38)

### Fixed

- Bug macos build error caused by logger class - by @MasterLaplace in [#25](https://github.com/MasterLaplace/Flakkari/pull/25)
- Windows CI Build Errors - by @MasterLaplace in [#28](https://github.com/MasterLaplace/Flakkari/pull/28)
- Bug bumper script and ci tag out of sync - by @MasterLaplace in [#36](https://github.com/MasterLaplace/Flakkari/pull/36)

### Changed

- Set up Git with GitHub Token - by @MasterLaplace in [#34](https://github.com/MasterLaplace/Flakkari/pull/34)


## [0.2.0] - 2024-01-9

### Added

- Implement a basic client manager - by @MasterLaplace in [#11](https://github.com/MasterLaplace/Flakkari/pull/11)
- Implement header packet for flakkari protocol - by @MasterLaplace in [#12](https://github.com/MasterLaplace/Flakkari/pull/12)
- Implemen a complete udp server - by @MasterLaplace in [#13](https://github.com/MasterLaplace/Flakkari/pull/13)
- Update the flakkari logger - by @MasterLaplace in [#15](https://github.com/MasterLaplace/Flakkari/pull/14)
- Update the flakkari network library - by @MasterLaplace in [#16](https://github.com/MasterLaplace/Flakkari/pull/15)
- Implement a command manager for admin user - by @MasterLaplace in [#17](https://github.com/MasterLaplace/Flakkari/pull/16)
- Implement ecs in flakkari server - by @MasterLaplace in [#18](https://github.com/MasterLaplace/Flakkari/pull/17)
- Implement a math library in flakkari engine - by @MasterLaplace in [#19](https://github.com/MasterLaplace/Flakkari/pull/18)
- Implement a game class in flakkari server - by @MasterLaplace in [#20](https://github.com/MasterLaplace/Flakkari/pull/19)
- Implement the game manager class - by @MasterLaplace in [#21](https://github.com/MasterLaplace/Flakkari/pull/20)
- Implement a complete game manager - by @MasterLaplace in [#22](https://github.com/MasterLaplace/Flakkari/pull/21)

### Changed

- Dev 0.2.0 by @MasterLaplace in [#23](https://github.com/MasterLaplace/Flakkari/pull/22)


## [0.1.0] - 2023-12-24

### Added

* Apply personal GitHub template - by @MasterLaplace in [#2](https://github.com/MasterLaplace/Flakkari/pull/2)
* Implement a logger class for the flakkary server - by @MasterLaplace in [#3](https://github.com/MasterLaplace/Flakkari/pull/3)
* Implement a address class to handle ip and port - by @MasterLaplace in [#4](https://github.com/MasterLaplace/Flakkari/pull/4)
* Implement a socket wrapper - by @MasterLaplace in [#5](https://github.com/MasterLaplace/Flakkari/pull/5)
* Implement a buffer class to handle binary data - by @MasterLaplace in [#6](https://github.com/MasterLaplace/Flakkari/pull/6)
* Implement a poll wrapper - by @MasterLaplace in [#7](https://github.com/MasterLaplace/Flakkari/pull/7)
* Implement a complete network lib - by @MasterLaplace in [#8](https://github.com/MasterLaplace/Flakkari/pull/8)
* Implement a simple udp server and client - by @MasterLaplace in [#9](https://github.com/MasterLaplace/Flakkari/pull/9)

### Changed

* Dev 0.1.0 by @MasterLaplace in [#10](https://github.com/MasterLaplace/Flakkari/pull/10)
