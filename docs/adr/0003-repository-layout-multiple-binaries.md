# ADR-0003 — Repository layout for multiple binaries

Status: accepted (2026-07-26)

## Context

The repository now produces several binaries (launcher daemon, launcher UI,
settings; lock screen and more to come). Their sources must not mix beyond
deliberately reusable pieces.

## Decision

```
libs/
  core/       Shared business core (ADR-0002): domain, application,
              infrastructure. Qt-free. Target: reboard_core.
  rekit/      Shared UI kit (ADR-0005): QML components + i18n catalogs.
apps/
  launcher/   daemon/ (binary: reboard) and ui/ (binary: reboard-ui).
  settings/   binary: reboard-settings.
tests/        Unit tests for libs/core.
```

Rules:

1. An app may depend on `libs/*`, never on another app's sources.
2. Anything two apps need moves down into a lib; no copy-paste between apps.
3. One directory per binary; each app dir owns its main.cpp, view models and
   screen QML (composition only — reusable visuals belong to rekit).
4. The daemon stays Qt-free and is the only resident process.

## Consequences

- Adding an app = new `apps/<name>/` + a CMake target; CI packages every
  binary per device.
- Include paths stay stable (`domain/...`, `infrastructure/...`) with
  `libs/core` as the include root.
- The kit and core can later be published for third-party use without
  dragging app code along.
