# Story 010 — Keyboard and text input system

## User story

As a user of any reBoard app, I want to type text — with the physical Type
Folio keyboard or with an on-screen touch keyboard that overlays the current
app like in iOS — so features like store search or settings fields are fully
usable. As a developer, I want input fields and the keyboard to be reKit
components any app can use.

## Requirements

- reKit gains text input components (`TextField` at minimum) with the
  e-paper visual language.
- On-screen keyboard as a reKit overlay component: slides over the current
  screen, dismissable, works on any app that embeds it; stable and
  reusable, no hacks.
- Physical keyboard input must reach the focused field (Qt evdev keyboard
  support: needs `QT_QPA_GENERIC_PLUGINS=evdevkeyboard` or equivalent on
  the epaper platform — to be validated on device).
- Keyboard layout/localization (e.g. ES, US) selectable from Settings and
  honored by both the on-screen keyboard and the physical keyboard mapping.
- About reusing the stock reMarkable keyboard: to be checked briefly, but
  it is expected to live inside xochitl (not reusable); if so, build our
  own without hesitation.

## Acceptance criteria

- [ ] A store search field is typeable with both the Type Folio and the
      on-screen keyboard.
- [ ] The on-screen keyboard overlays and dismisses without disturbing the
      underlying screen state.
- [ ] Layout selection in Settings affects both input paths.
- [ ] All components live in reKit and carry no app-specific logic.
