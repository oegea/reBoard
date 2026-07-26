# Story 016 — Claude Code companion (remote sessions from the tablet)

## User story

As a developer, I want to connect to my Claude Code sessions from the
reMarkable — the tablet as a calm, e-paper window into my agent runs — so I
can read output, answer prompts and steer work with the Type Folio.

## Fit (ADR-0007): excellent — this IS the developer-companion vision.

## Phased approach

1. **Phase 0 (works today, document it)**: the Terminal + `ssh` into any
   machine and run `claude` there. Zero new code; validate the experience
   (rendering of the TUI, latency) and fix terminal gaps it reveals.
2. **Phase 1 (research)**: evaluate the official remote/teleport options —
   Claude Code web/cloud sessions and the CLI's remote features — and
   whether a thin client makes sense versus plain ssh+TUI. Decide with real
   usage data from phase 0.
3. **Phase 2 (only if phase 1 justifies it)**: a dedicated reBoard app —
   e-paper-first reading pane for transcripts, big touch targets for
   approve/deny prompts, notifications on the board when a session needs
   attention.

## Acceptance criteria (phase 0)

- [ ] Running `claude` over ssh from the tablet's Terminal is usable:
      readable output, workable keyboard interaction, no emulator glitches.
- [ ] Gaps found (colors, spinners, alt-screen behaviors) are filed against
      story 013.
