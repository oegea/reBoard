# Story 014 — Speed, refresh and user feedback

## User story

As a reBoard user, I want the system to feel as responsive as the stock UI:
instant feedback when I touch something, fast transitions between board and
apps, and a screen refresh strategy that balances crispness and speed.

## Analysis (honest)

Where the speed difference against the stock UI comes from:

1. **The stock UI never restarts.** reBoard's launcher UI is deliberately a
   short-lived process (the e-paper framebuffer lock is exclusive — two
   owners reboot the device), so every board appearance pays Qt startup on
   ARM, and opening Notebooks pays xochitl's full cold start. This is a
   structural trade-off for stability, not an accident.
2. **No touch feedback** made everything *feel* slower than it is.
3. Waveform/refresh tuning is not exposed by the epaper Qt plugin (verified
   against libepaper.so): we cannot pick "fast/dirty vs slow/clean" per
   update from the official API today.

## Shipped in this round

- [x] Instant pressed states across the whole kit: PushButton and key caps
      invert, list rows highlight, app icons dim, back buttons fade.
- [x] Header hairlines when content scrolls (removes the "floating title"
      effect that also read as jank).
- [x] Store update flow no longer requires remove + install.

## xochitl analysis (2026-07-26, on device)

- xochitl embeds the EPFramebuffer classes directly (symbols:
  `EPFramebufferSwtcon`, `EPFramebufferCarta1000`, and notably
  `EPFramebuffer::GhostControlMode` — ghosting control EXISTS in the
  internal API). The lock file is `/tmp/epframebuffer.lock`.
- The public epaper Qt plugin exposes none of those knobs in its strings:
  our apps get whatever defaults the plugin picks. Reaching GhostControlMode
  would require deeper reverse engineering (symbol-level, linked-library
  analysis) — future work behind a safety gate; never risk the reboot loop.

## Next levers (ordered)

- [ ] Measure and log the real times: UI spawn, xochitl start, app launch.
- [ ] Verify the QML disk cache is active on device (~/.cache/*/qmlcache)
      and prime it at install time if not.
- [ ] Trim daemon reaction times (poll intervals, backoffs) guided by the
      measurements.
- [ ] Research non-official refresh control (community EPFB knowledge) —
      only behind a safety story; never risk the reboot loop.
- [ ] Consider a user-facing "display feel" setting (quality vs speed) once
      a safe lever exists.
