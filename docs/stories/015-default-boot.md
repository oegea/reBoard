# Story 015 — reBoard as default boot experience (non-intrusive)

## User story

As a reBoard user, I want the tablet to boot into reBoard by default —
without breaking the stock system, replacing original binaries or touching
the OS partition — and with a trivial way back.

## Answer: yes, and the pieces already exist

- `packaging/reboard.service` is a plain systemd unit in
  `/etc/systemd/system` (user space, no OS files touched). Enabling it
  (`systemctl enable reboard`) makes reBoard start at boot; it stops
  xochitl at startup and the built-in Notebooks entry brings it back.
- Nothing is replaced: xochitl's own unit and binary stay untouched;
  disabling reboard (`systemctl disable reboard`) restores stock behavior
  entirely. Recovery paths: SSH, or a reboot after disabling.

## Work remaining before recommending it

- [ ] Survive OS updates gracefully (updates may reset /etc — detect and
      offer to re-enable rather than failing silently).
- [ ] Boot-order validation on device: Wi-Fi, time sync and storage must be
      up before the board shows.
- [ ] Power management while reBoard is the session owner (story 007
      phase 2) — sleep must work before boot-default is honest.
- [ ] A Settings toggle ("Start reBoard at boot") wrapping enable/disable
      with clear explanations.
