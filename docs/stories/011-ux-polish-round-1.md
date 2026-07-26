# Story 011 — UX polish, round 1

## User story

As a reBoard user, I want the details to feel deliberate — real app icons,
the return-hint only where it helps, proper selection marks, a readable
license — because the UI/UX of this system must be cared for down to the
millimeter.

## Scope

1. **Return hint only for Notebooks**: the pre-launch modal (story 004)
   only makes sense when handing the screen to the stock UI; other apps
   open directly. Modeled as a `hint` flag on the application (manifest key
   `hint=true`; built-in Notebooks sets it), not hardcoded ids in QML.
2. **Real icons**: Notebooks and Settings ship monochrome icons rendered on
   the board instead of initial letters. Store apps bring their own icon in
   the package; the letter tile remains only as a fallback.
3. **Drawn checkmark**: the "✓" glyph is missing from the device font and
   rendered as "[]"; selection marks become a reKit-drawn `CheckMark`
   component (no font dependence).
4. **License viewer**: tapping the License row in Settings opens the full
   GPL-3.0 text in a scrollable page with a back button.

## Acceptance criteria

- [ ] Opening Settings (or any non-hint app) shows no modal; Notebooks
      still shows it.
- [ ] Board shows real icons for the base apps; manifest apps without icon
      fall back to the letter tile.
- [ ] Language selection shows a crisp drawn checkmark.
- [ ] License page scrolls the entire GPL text and returns with Back.
