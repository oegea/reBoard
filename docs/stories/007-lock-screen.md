# Story 007 — Lock screen (swipe to unlock)

## User story

As a reBoard user, when I press the power button I want the classic
old-iOS-style lock screen — time, date and a "slide to unlock" control — so
waking the device feels familiar and delightful, in reBoard's e-paper visual
language.

## Status: phase 1 shipped (2026-07-26); phase 2 pending research

## Device findings (validated on the rM2)

- The power button is its own evdev device (`snvs-powerkey`, `event0`,
  `KEY_POWER`), separate from touch (`event2`) and pen (`event1`).
- reMarkable OS ships `HandlePowerKey=ignore` for systemd-logind: with
  xochitl stopped, the button does nothing by default — reBoard can own it
  safely, with no risk of surprise poweroffs.
- Suspend states `freeze standby mem` exist, but xochitl performs extra
  teardown (Wi-Fi, sleep splash) before suspending — replicating that
  safely is phase 2.

## Phase 1 (shipped)

- `EvdevPowerButton` (core infrastructure) + `PowerButtonMonitor` (launcher
  UI thread): pressing power while the **board** is on screen shows the
  lock screen.
- `LockScreen` (launcher screen): big light-weight clock, localized date,
  full-screen input block; unlocking returns exactly where you were.
- `SlideToUnlock` (reKit component): recessed track, draggable knob with a
  drawn chevron, snap-back on incomplete drags. Literal "slide to unlock"
  translated via the standard catalogs.
- Overridable device via `REBOARD_POWER_DEVICE`.

## Phase 2 (pending)

- [ ] Real suspend/wake: replicate xochitl's teardown (Wi-Fi, splash) or
      find the sanctioned path; wake must land on the lock screen.
- [ ] Power press while a third-party app owns the screen (daemon-side
      handling; xochitl keeps handling its own sleep while it runs).
- [ ] Auto-lock after inactivity, sleep screen image.

## Acceptance criteria (phase 1)

- [x] Pressing power with the board visible shows the lock screen.
- [x] Sliding the knob to the end unlocks; partial drags snap back.
- [x] Clock/date localized and refreshed while visible.
- [x] Slide control is a reusable reKit component; literals translatable.
