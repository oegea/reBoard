# Story 004 — Pre-launch hint modal (with real i18n)

## User story

As a reBoard user, when I open an application from the board (e.g.
Notebooks/xochitl), I want a small alert-style modal to remind me how to come
back ("press and hold the bottom edge of the screen for a couple of seconds,
or swipe up from it") before the application opens, so I never feel locked
out of the launcher — given that no reBoard UI can be drawn over a running
application (story 003 is parked to avoid adding instability).

## Requirements

- Small centered modal, e-paper friendly (white panel, black border, no
  animations), with the hint text and two actions: cancel and open.
- Confirming launches the application exactly as today (UI exits first).
- **All user-facing literals must be translatable.** Proper Qt i18n:
  `qsTr()` in QML, translation catalogs (`.ts` → `.qm`) embedded in the
  binary, loaded with `QTranslator` based on the system locale (overridable
  with `REBOARD_LOCALE`). English is the source language; Spanish ships as
  the first translation. No hardcoded per-language switches.

## Acceptance criteria

- [ ] Tapping any application shows the modal; cancel returns to the board,
      open launches the app.
- [ ] Literals come from translation catalogs; with `REBOARD_LOCALE=es` the
      modal shows in Spanish, otherwise in English.
- [ ] Adding a new language requires only a new `.ts` file, no code changes.

## Future possibilities

- Show the hint only the first N times, or per-app ("don't show again").
- Per-app custom hint text in the `.app` manifest.
- Reuse the modal component for other confirmations (see story 005).
