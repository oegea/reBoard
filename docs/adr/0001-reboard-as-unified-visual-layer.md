# ADR-0001 — reBoard is the unified visual layer for reMarkable

Status: accepted (2026-07-26)

## Context

reBoard started as a springboard-style launcher. It has grown into the
foundation of an operating-system shell: launcher, settings, lock screen and
future system apps, plus the ambition that third-party apps feel native.

## Decision

reBoard acts as an application launcher that offers a **unified UI layer**
for the whole device:

- The resident daemon owns the session: gestures, application lifecycle and
  the guarantee that exactly one process owns the e-paper display.
- Every visual surface (board, settings, lock screen, future apps) is built
  from one shared UI kit (ADR-0005) with one i18n system (ADR-0004), so the
  device feels like a single coherent OS.
- System apps are independent binaries orchestrated by the daemon
  (ADR-0003); nothing is embedded in the launcher binary.

## Consequences

- Visual or textual conventions are decided once, in the kit/ADRs, never
  per app.
- New system features default to "new app + kit components", keeping the
  launcher small.
- Third-party apps can adopt the kit to blend in, but the daemon must never
  depend on apps behaving well (stability guards stay in the daemon).
