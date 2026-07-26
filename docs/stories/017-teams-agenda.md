# Story 017 — Microsoft Teams agenda (and maybe chats)

## User story

As a working developer, I want to see my MS Teams calendar for the day on
the reMarkable — and possibly glance at chats — so the tablet covers my
work context without opening the laptop.

## Fit (ADR-0007): partial — agenda yes (developer's day at a glance),
chats only if nearly free.

## Honest scoping

- **Calendar (viable)**: Microsoft Graph API with the *device code flow*
  (user enters a code at microsoft.com/devicelogin from any browser — no
  keyboard-heavy OAuth on the tablet). Read-only `Calendars.Read` scope,
  render today/next-days as reKit lists. Needs an Azure app registration
  (user-provided client id in Settings). Moderate effort, low risk.
- **Chats (defer)**: `Chat.Read` requires broader consent, pagination,
  and near-real-time refresh to be useful; e-paper is a poor fit for chat
  cadence. Verdict: not now — revisit only if the agenda app proves the
  Graph plumbing and demand is real.
- Depends on story 010 phase 2 (TextField) for entering the client id, or
  ships with a config-file fallback first.

## Acceptance criteria (phase 1: agenda)

- [ ] Sign-in via device code flow, token persisted and refreshed.
- [ ] Today + upcoming events rendered with reKit components; offline shows
      the last cached agenda with its age.
- [ ] No secrets in the repository; tokens stored only on the device.
