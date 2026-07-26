# Story 005 — Reusable UI component library

## User story

As the reBoard developers, we want every visual element of the launcher to
be a reusable, self-contained QML component, so the system grows like an
operating-system shell: consistent visual language, one place to change each
element, and new screens assembled from existing parts.

## Rationale

reBoard is becoming the shell that governs the whole device experience.
Screens will multiply (board, modals, future settings, onboarding). Without
a component library each screen reinvents its buttons and panels and the
design drifts apart.

## Principles

- Components are **presentational and dumb**: they render data and emit
  signals. Business logic stays in the daemon / use cases; process-level
  decisions never live in QML.
- One component per file under `src/ui/qml/components/`.
- E-paper first: black/white, strong borders, no animations, minimal
  refreshes.
- All literals inside components use `qsTr()` (story 004).

## Initial catalog

| Component | Responsibility |
| --- | --- |
| `AppIcon.qml` | Rounded-square icon + label, tap signal |
| `TopBar.qml` | Thin status bar (clock for now) |
| `HomeMarker.qml` | Subtle gesture-zone pill |
| `HintDialog.qml` | Modal alert with message + cancel/confirm |

## Acceptance criteria

- [ ] `Main.qml` contains only composition/layout; every visual element
      comes from `components/`.
- [ ] Components carry no business logic (signals out, properties in).
- [ ] New screens can reuse the catalog without modification.
