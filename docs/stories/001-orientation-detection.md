# Story 001 — Orientation detection

## User story

As a reBoard user, I want the launcher to render in portrait or landscape
according to how I am using the tablet — and always in landscape when the
physical keyboard (Type Folio) is attached and in use — so that the board,
the top bar, the home marker and the "return home" gesture zone are always
where I expect them to be.

## Context and constraints

- The reMarkable 2 has **no accelerometer**: the stock UI (xochitl) does not
  auto-rotate either; orientation there is a manual setting. Automatic
  "follow the physical orientation" is therefore not possible on rM2.
- The Type Folio keyboard attaches through the pogo pins and shows up as a
  standard evdev keyboard device. Its presence is detectable by scanning
  `/dev/input/event*` for a device advertising letter/enter keys and no
  touch axes.
- Orientation affects **three** things, which must stay consistent:
  1. How the UI is rendered (board, dock, top bar).
  2. Where the subtle home marker is drawn (always at the *visual* bottom).
  3. How raw touch coordinates are interpreted by the resident gesture
     detector (the "swipe up from the bottom edge" zone must follow the
     visual bottom, both while the board is shown and while another
     application is on screen).

## Decision table (first iteration)

| Condition | Orientation |
| --- | --- |
| `REBOARD_ORIENTATION=portrait` or `landscape` set | Forced to that value |
| Physical keyboard detected | Landscape |
| Otherwise | Portrait |

The landscape rotation direction defaults to 90° clockwise (visual bottom on
the physical left edge, where the Type Folio holds the device) and can be
flipped with `REBOARD_LANDSCAPE_ROTATION=270` until validated on hardware.

## Acceptance criteria

- [ ] With no keyboard attached, the board renders in portrait.
- [ ] With the keyboard attached, the board renders in landscape and the
      dock/top bar/home marker sit on the visual bottom/top.
- [ ] Attaching/detaching the keyboard while an application is running is
      picked up: the next time the board is shown it uses the new
      orientation, and the gesture zone follows immediately.
- [ ] The home gesture (swipe from the visual bottom edge / long press)
      works identically in both orientations, including while xochitl is on
      screen.
- [ ] `REBOARD_ORIENTATION` overrides everything (useful for devices or
      setups we have not tested).
- [ ] Orientation logic is pure domain code covered by unit tests
      (policy + touch coordinate transform).

## Future possibilities (out of scope here)

- Reading the user's orientation preference from xochitl's own settings so
  both UIs agree when there is no keyboard.
- Real auto-rotation on devices that do ship an accelerometer (Paper Pro
  family), preferably behind the same orientation policy.
- Per-application orientation preferences in the `.app` manifest.
- Overlaying a home marker while another application owns the display; the
  e-paper framebuffer lock is exclusive on rM2, so this needs dedicated
  research (see story 002 notes) and is not attempted in this iteration.
