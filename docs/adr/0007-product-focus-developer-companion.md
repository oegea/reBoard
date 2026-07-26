# ADR-0007 — Product focus: the reMarkable as a developer's companion

Status: accepted (2026-07-26)

## Context

reBoard could grow in any direction (media, reading, games). Oriol's vision
is specific: the reMarkable, with its Type Folio keyboard, should work as a
**lightweight developer terminal** — a distraction-free machine to shell
into servers, edit, take notes and read documentation.

## Decision

Every new tool must pass this filter: *does it serve a developer using the
tablet with a keyboard?* Current verdicts:

- **Terminal** (story 013): the flagship. Keeps absorbing investment
  (quality, speed, sessions) before anything new is started.
- **File explorer**: yes, eventually — lightweight, oriented to moving and
  inspecting files, not a desktop metaphor.
- **KOReader**: package the existing community app in the store (reading
  documentation counts); we build nothing.
- **Web browser**: no for now. Huge effort, weak fit with e-paper and the
  focus; documentation reading is better served by KOReader/terminal tools.
- Anything else starts as a story and must argue its fit here first.

## Consequences

- The store catalog curates for this audience instead of chasing breadth.
- Speed and input quality (stories 010/014) take priority over new apps —
  a developer tool that lags is a developer tool that gets abandoned.
