# ADR-0002 — Domain-driven layering

Status: accepted (2026-07-26)

## Context

PROJECT.md mandates use cases, non-anemic domain objects and the repository
pattern. The code must stay unit-testable on any host without device
hardware or Qt.

## Decision

Three layers, in `libs/core`, shared by every binary:

- `domain` — immutable value objects and entities (validating constructors),
  pure domain services (GestureDetector, OrientationPolicy, coordinate
  transforms) and repository **interfaces**. Depends on nothing.
- `application` — use cases that depend only on domain, plus
  `UseCaseFactory`, the single place allowed to touch infrastructure to
  inject dependencies.
- `infrastructure` — implementations of the domain repositories (manifest
  files, POSIX processes + systemctl, evdev input). May depend on domain.

Hard rules: domain depends on nothing; value objects and entities are
immutable; the core is **Qt-free** so tests run on any Linux host; UI layers
contain no business logic (they call use cases and render).

## Consequences

- Broad unit coverage without hardware: fakes for repositories, pure logic
  for gestures/orientation/layout.
- Qt appears only in `apps/*/` UI code; a UI rewrite would not touch
  business rules.
- Anything device-specific (framebuffer locks, systemd quirks) is isolated
  behind repository implementations.
