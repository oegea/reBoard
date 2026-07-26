# Story 007 — Lock screen (swipe to unlock)

## User story

As a reBoard user, when I press the power button I want the classic
old-iOS-style lock screen — time, date and a "slide to unlock" control — so
waking the device feels familiar and delightful, in reBoard's e-paper visual
language.

## Status: planned (not yet implemented)

## Notes and open questions

- The power button is an evdev key (`KEY_POWER`); the resident daemon can
  observe it just like touch input. Without xochitl running, suspend is not
  handled by anyone — the daemon will need to orchestrate it (systemd
  suspend or the reMarkable power paths) before/after showing the lock
  screen. This needs on-device research: what does pressing the button do
  today while reBoard owns the session?
- The lock screen belongs to the launcher binary (board), shown by the
  daemon when waking; Settings stays a separate app (ADR-0003). Visuals come
  from the shared UI kit: it will need a `SlideToUnlock` component.
- Sleep behavior must match device expectations (e-paper keeps the last
  frame; a sleep screen image may be desirable, like the stock UI's).

## Acceptance criteria (draft)

- [ ] Pressing power while reBoard is in the foreground suspends the device
      after showing the lock screen frame.
- [ ] Pressing power again wakes into the lock screen; sliding unlocks back
      to wherever the user was (board or running app untouched).
- [ ] Slide control is a reusable UI kit component.
- [ ] All literals translatable.
