# Goal

reBoard must be an application that acts as an application board for the
reMarkable platform, inspired by springBoard.

Its main goal is to act as a customizable application launcher. Some
applications — the minimal set — will be fixed, but it must work as an
extensible system.

No application will be embedded inside the binary itself; everything must
invoke third-party binaries.

The layout must be strongly inspired by the iOS springboard, in a simplified
form, like its earliest versions: a bottom bar with a few fixed applications
and a set of applications that appear on the board across different pages as
the board fills up.

The first basic version must be an application runnable on the reMarkable
ecosystem, and it must include a default application that returns to xochitl
(the stock reMarkable interface).

In addition, the reBoard binary must stay loaded in memory and detect a
gesture on the touch screen — for example swiping from the bottom, or pressing
and holding for X seconds. That must bring reBoard back to the main screen,
and if xochitl or another binary is on the main screen, it will close it.

# Documentation and references

Development of this project must follow what the official reMarkable SDK
determines, and it must provide compiled versions for the different existing
products: https://developer.remarkable.com/documentation/sdk

# Architecture

The business code (listing applications, executing binaries, returning to
reBoard, etc.) is organized into use cases.

- `application` → contains use cases. Only use cases, plus a factory that
  wraps the use cases with the infrastructure dependencies injected.
- `domain` → contains value objects, entities and repository definitions.
  Non-anemic VOs and entities that hold the necessary logic.
- `infrastructure` → performs the calls through the repository pattern.
  It always implements repositories defined in `domain`.

Rules:

1. Infrastructure may depend on domain.
2. Domain must not depend on anything.
3. Application only depends on domain inside the use cases; the factory that
   makes the use case callable depends on infrastructure to inject the
   dependencies.
4. Both value objects and entities are immutable objects.

# Source control

A git repository must be initialized in the main reBoard directory.
This repository will later be hosted publicly on GitHub.

It will not accept code contributions, although it will accept issues.

The license is GPL 3.

The repository must be prepared for this: license files, legally required
notices in the code if mandatory, a contributing file, and anything else
needed.

# Continuous integration

Changes pushed to the `main` branch of this repository must trigger GitHub
Actions that run the unit tests.

Unit tests must have the broadest possible coverage.

With every change, once the unit tests pass, versions are published as
artifacts within the GitHub repository.

# Manual testing

The binary that will be primarily tested is the reMarkable 2 one.

# Legal

It must be clear that the project is open source, that there is no
association with reMarkable, and that the word reMarkable is used solely to
reference the device. A non-affiliation notice is required to avoid legal
problems.

# While coding

Keep filling in CLAUDE.md as you make decisions and complete the code.

# Language

Code, comments, CLAUDE.md and any documentation must be in English. This file
must be translated into English as well.
