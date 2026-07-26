# Story 002 — Top bar

## User story

As a reBoard user, I want a thin, iOS-inspired status bar at the top of the
board showing the time and the essentials, so that the launcher feels like a
proper home screen — while staying completely out of the way when another
application (e.g. xochitl) is on screen.

## Scope

- A slim bar rendered at the very top of the board, in both orientations.
- Contents for this iteration: the current time (HH:MM), centered, plus a
  hairline separator under the bar. Nothing else yet.
- E-paper friendly: no animations; the clock refreshes at most once per
  minute to avoid unnecessary panel refreshes.
- **Only visible while the board is shown.** The launcher UI is a
  short-lived process that dies before any application starts, so by design
  nothing of reBoard is drawn over other applications. Note-taking in
  xochitl must never be obstructed.

## Acceptance criteria

- [ ] The board shows a thin top bar with the current time, correctly
      positioned in portrait and landscape.
- [ ] The clock stays correct while the board is open (minute-level
      refresh).
- [ ] No reBoard UI element is drawn while another application owns the
      screen.
- [ ] The bar does not steal touch input from the board grid.

## Future possibilities (out of scope here)

- Battery percentage and charging indicator (from
  `/sys/class/power_supply/`).
- Wi-Fi status.
- Date, and a long-press on the clock to open a minimal settings panel.
- A subtle *persistent* home marker overlaid on top of running applications.
  This conflicts with the exclusive EPFramebuffer lock on rM2 (drawing from
  a second process is what used to reboot the device), so it requires
  dedicated research — e.g. cooperative drawing before handing the display
  over, or device-specific low-level techniques. Tracked as its own future
  story; must never risk device stability.
