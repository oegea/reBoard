# Live testing against a real reMarkable 2

How we test reBoard interactively against the development device. Device
credentials live in `docs/DEVICE-TESTING.local.md` (gitignored — never
commit it; copy the template below).

## Setup

- Connect the tablet over USB: it exposes SSH at `root@10.11.99.1`.
- The root password is shown on the device under
  Settings → Help → Copyrights and licenses (and stored locally in the
  `.local.md` file for automation).
- Password auth is automated with an `expect` wrapper kept OUTSIDE the
  repository (session scratchpad), reading the password from the `RMPASS`
  environment variable.

## Test loop (agreed workflow)

1. Build: `make cross-rm2`.
2. Deploy: `scp build-rm2/apps/launcher/reboard build-rm2/apps/launcher/reboard-ui build-rm2/apps/settings/reboard-settings root@10.11.99.1:/home/root/`.
3. Launch **persistently, with no timer** — the tester tries it at their own
   pace and reports back:

   ```sh
   # BusyBox pkill -f silently kills nothing — use pidof + kill instead.
   kill $(pidof reboard) $(pidof reboard-ui) 2>/dev/null
   systemctl reset-failed xochitl
   systemctl stop xochitl
   nohup env QT_QPA_PLATFORM=epaper QT_QUICK_BACKEND=epaper \
     QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS='rotate=180:invertx' \
     /home/root/reboard >/home/root/reboard.log 2>&1 &
   ```

4. When done: `kill $(pidof reboard) $(pidof reboard-ui) 2>/dev/null; systemctl start xochitl`.
5. Logs: `/home/root/reboard.log` (grep -v 'bin file' to drop benign noise)
   and `journalctl -b [-1]` for system-level events.

## Device facts learned the hard way

- The e-paper framebuffer lock is exclusive per process; a second owner
  makes xochitl crash-loop and `remarkable-fail.sh` **reboots the device**.
- Rapid xochitl stop/start cycles exhaust its systemd start rate limit with
  the same reboot outcome — always `systemctl reset-failed xochitl` before
  starting it.
- A tablet reboot is always a safe recovery: the stock UI boots untouched.
- The remote shell is BusyBox: no `timeout`, limited flags.

## Template for `docs/DEVICE-TESTING.local.md`

```markdown
# Local device credentials (DO NOT COMMIT)
- Device: reMarkable 2, USB at root@10.11.99.1
- Root password: <password from the device settings>
```
