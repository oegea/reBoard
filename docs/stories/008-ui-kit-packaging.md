# Story 008 — Shared UI kit usable by any app ("reKit")

## User story

As a developer of reBoard system apps — or of a third-party app — I want the
visual components (icons, bars, dialogs, buttons, grouped touch lists) to be
a reusable kit that any binary in (or outside) this repository can depend
on, so every app on the device shares one visual language: reBoard as the
unified visual layer of the reMarkable.

## Scope of this iteration

- Move all QML components out of the launcher into `libs/rekit/`
  (components + translation catalogs), with the iOS-style list machinery
  added: `ListGroup` and `ListRow`.
- Both `reboard-ui` and `reboard-settings` consume the kit; no app-specific
  QML lives in the kit, no kit code is duplicated in apps.
- Distribution today: the kit ships in-tree as a Qt resource that each app
  embeds at build time.

## Acceptance criteria

- [ ] No component `.qml` file lives under any app directory.
- [ ] Launcher and Settings render exclusively with kit components +
      composition.
- [ ] A new app in `apps/` can use the kit by adding one resource reference
      and one import line.

## Future possibilities

- Package the kit as a proper versioned QML module (`import ReKit 1.0`)
  with a stable API, distributable as an SDK for third-party developers
  (needs host tooling validation in the cross toolchain).
- Theming tokens (spacing, type scale) extracted as a singleton.
- A gallery/demo app documenting every component.
