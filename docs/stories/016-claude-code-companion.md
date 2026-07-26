# Story 016 — Claude Code app (remote sessions, native UI)

## User story

As a developer, I want a dedicated reBoard app to work with my **remote
Claude Code sessions** — a calm e-paper window with big touch targets to
read transcripts, answer prompts, and approve or deny actions — using the
Type Folio to type. Explicitly NOT an ssh-terminal workaround: a native
app.

## Fit (ADR-0007): this is the flagship companion use case.

## Architecture decision (design, pending build)

- **Protocol**: the app speaks Claude Code's official structured interface:
  `claude -p --input-format stream-json --output-format stream-json`
  executed on the remote host over an SSH exec channel. Every message,
  tool call and permission request arrives as a typed JSON event — a real
  API, not scraped terminal output.
- **Transport**: libssh2 or the device's ssh binary with pipes (decide by
  footprint); key-based auth; host/user configured in the app (config file
  until reKit TextField lands — story 010 phase 2 unblocks in-app setup).
- **App (reboard-claude, via the store)**:
  - Session list (resume `--resume` ids) and new-session start.
  - Transcript pane: assistant text rendered readably (lazy list, like the
    license viewer); tool calls collapsed with expand-on-tap.
  - Permission prompts as big Approve / Deny buttons — the tablet as a
    supervision surface is the killer feature.
  - Input line with OSK/Type Folio for follow-up instructions.
- **Out of scope**: running Claude Code ON the tablet; chat niceties.

## Phases

1. Skeleton app: fixed host config file, connect, one-shot prompt,
   streamed transcript rendering.
2. Permission-request handling (approve/deny), session resume list.
3. In-app setup UI once TextField exists; board notification when a
   session awaits input (daemon integration).

## Acceptance criteria (phase 1)

- [ ] With a config file pointing at a host with Claude Code installed,
      the app starts a session, streams the response live and renders it
      lazily on e-paper.
- [ ] Connection errors surface with the standard offline/error patterns.
- [ ] All UI from reKit; literals translated.
