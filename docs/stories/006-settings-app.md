# Story 006 — Settings app

## User story

As a reBoard user, I want a Settings application inspired by iOS/iPadOS
Settings — simple, uncluttered, with well-crafted touch lists — where I can
see which reBoard version and build I am running, license information,
available storage, the installed applications, and basic configuration
(language selection), so the system feels like a coherent OS.

## Requirements

- **Separate binary** (`reboard-settings`), launched from the board like any
  other application. Its source lives apart from the launcher; the only
  shared code is the reusable libraries (core + UI kit). See ADR-0003.
- iOS/iPadOS-inspired grouped lists: rounded sections, one row per item,
  hairline separators, right-aligned values, checkmark for selections.
  The list machinery must be **reusable components** (story 008), so any
  third-party app can build the same look.
- Contents (first iteration):
  - About: version, build (short git commit of the running build), license.
  - Storage: available space on the user partition.
  - Language: system / English / Español — persisted, applied by every
    reBoard app on next start (see ADR-0004).
  - Applications: the apps currently installed on the board.
- All literals translatable (ADR-0004).

## Acceptance criteria

- [ ] A "Settings" entry appears on the board and opens the app; the home
      gesture returns to the launcher.
- [ ] Version and build shown match the running binary (git short hash).
- [ ] Changing the language persists and both launcher and settings honor
      it after relaunch.
- [ ] Storage shows free/total space of the user data partition.
- [ ] The screen is assembled exclusively from shared UI kit components.

## Future possibilities

- Wi-Fi, battery details, gesture sensitivity tuning, dock management,
  manifest editing, about-device (serial, OS version).
