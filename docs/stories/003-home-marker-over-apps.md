# Story 003 — Home marker visible over running applications (research)

## User story

As a reBoard user, I want the subtle home marker to stay visible while
another application (e.g. xochitl) is on screen, so I always know where the
"return to launcher" gesture zone is — without interfering with note-taking.

## Status: blocked on research — do not implement naively

## Why this is hard

- Touch input is a shared kernel interface (`/dev/input/event*`): any number
  of processes can read it concurrently. That is why the resident daemon can
  always detect the home gesture.
- The e-paper display is the opposite: there is no hardware compositor and
  no layering. On the reMarkable 2 each application drives the panel with
  its own software TCON (SWTCON) behind an **exclusive per-process lock**
  (EPFramebuffer). A second process trying to draw makes xochitl crash-loop,
  and the OS failure policy then **reboots the device** (observed twice on
  2026-07-26 during development).
- Conclusion: with the official SDK there is currently no sanctioned way to
  draw over a running application.

## Options to research (in rough order of preference)

1. **LD_PRELOAD injection into xochitl** — the community rm2fb project
   demonstrates hooking Qt/SWTCON internals inside the xochitl process; an
   injected shim could composite a small marker through xochitl's own
   refresh engine. Powerful, but fragile across OS updates and must be
   opt-in, version-gated and fail-safe (never risk the reboot loop).
2. **Ask reMarkable** — the developer program is new; an official overlay or
   status-area API may be worth a feature request.
3. **Do nothing visual, improve discoverability** — the gesture works
   everywhere already; first-run onboarding on the board itself ("swipe up
   from the bottom edge anytime to come back") may be enough in practice.
4. Rejected: drawing directly to the framebuffer or a second SWTCON —
   proven to reboot the device.

## Acceptance criteria (for whichever option wins)

- [ ] Never degrades stability of xochitl or any third-party app; a failure
      of the overlay must leave the running app untouched.
- [ ] Marker is subtle, at the visual bottom (orientation-aware, see story
      001) and never intercepts touch input meant for the app.
- [ ] Can be disabled via configuration.
- [ ] Survives (or cleanly disables itself on) OS updates it has not been
      validated against.
