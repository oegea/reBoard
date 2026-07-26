# ADR-0005 — reKit: the shared UI component kit

Status: accepted (2026-07-26)

## Context

Stories 005/006/008: every screen in the system — and ideally third-party
apps — must share one visual language, iOS/iPadOS-inspired, tailored to
e-paper.

## Decision

- All reusable visual elements live in `libs/rekit/components/`, one
  component per file: `AppIcon`, `TopBar`, `HomeMarker`, `PushButton`,
  `HintDialog`, `ListGroup`, `ListRow`, and whatever comes next
  (`SlideToUnlock`, ...).
- Components are **presentational and dumb**: properties in, signals out;
  no business logic, no process control, no persistence.
- E-paper design rules: black/white with strong borders, no animations,
  minimal refreshes (e.g. clock updates at most twice a minute), generous
  touch targets (rows ≥ 80px), rounded 12–28px radii.
- Literals inside components use `qsTr()` (ADR-0004).
- Distribution today: apps embed `libs/rekit/rekit.qrc` and import
  `qrc:/rekit/components`. Goal (story 008): a proper versioned QML module
  (`import ReKit 1.0`) once host tooling is validated in the cross SDK, so
  third-party apps can require it like a system framework.

## Consequences

- Screens are composition-only; visual changes happen once, in the kit.
- The kit is the contract with future third-party developers — component
  APIs should evolve additively (new properties with defaults) rather than
  breaking.
