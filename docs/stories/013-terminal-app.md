# Story 013 — Terminal app (distributed through the App Store)

## User story

As a reBoard power user, I want a super fresh terminal app to use a shell
directly on the reMarkable — crisp monospace rendering on e-paper, usable
with the Type Folio or the on-screen keyboard — installed from our own App
Store like any third-party app would be.

## Notes

- First app distributed through the store instead of being built into the
  base system: it validates the whole packaging/catalog pipeline with a
  real product (retiring hello-demo).
- Architecture: PTY (forkpty) + terminal state machine in a lib, rendering
  and input as a Qt Quick app built from reKit components. E-paper-aware
  refresh (batch updates, no cursor blinking).
- **Depends on story 010 (keyboard & text input)**: physical Type Folio
  input and the reKit on-screen keyboard overlay must exist first; the
  keyboard layout comes from Settings.
- Package it per device in CI and publish it in `store/`.

## Acceptance criteria (draft)

- [ ] Installable/uninstallable from the App Store on the rM2.
- [ ] Runs a login shell; typing works with Type Folio and the on-screen
      keyboard; output renders crisply with minimal ghosting.
- [ ] Home gesture always returns to the board (shell keeps running or is
      terminated — decide and document).
- [ ] UI assembled from reKit; literals translated.

## Status: planned — next after the keyboard system (story 010)
