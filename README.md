# reBoard

A springboard-inspired application launcher for reMarkable paper tablets.

reBoard turns your device into an extensible home screen: a paginated grid of
applications plus a fixed dock at the bottom, in the spirit of the earliest
iOS springboard. It stays resident in memory and watches the touch screen, so
a swipe up from the bottom edge (or a long press) always brings you back home,
closing whatever application is on screen — including the stock UI.

> **Status: early development.** The first target device is the reMarkable 2.
> Expect rough edges; flash at your own risk and keep a backup of your data.

## How it works

- **Nothing is embedded.** reBoard only launches third-party binaries and
  systemd units. Applications are declared with tiny manifest files.
- **One built-in entry.** "Notebooks" returns to the stock reMarkable UI by
  starting the `xochitl` systemd service.
- **Always-on home gesture.** A resident thread reads the touch screen through
  evdev even while another application owns the display. Swipe up from the
  bottom edge or press and hold for 1.5 s to come back to the launcher.
- **Official SDK.** The UI is a Qt Quick application rendered through the
  official `epaper` platform plugin, built with the official reMarkable
  cross-compilation toolchains.

## Installing applications on the board

Drop a `<id>.app` manifest in one of these directories on the device:

- `/etc/reboard/apps`
- `/opt/etc/reboard/apps`
- `~/.config/reboard/apps`

```ini
# /etc/reboard/apps/koreader.app
name=KOReader
exec=/opt/koreader/koreader.sh
# or, for systemd services:  unit=my-app.service
icon=/opt/etc/reboard/icons/koreader.png
dock=false
```

The file name (without `.app`) is the application id. Exactly one of `exec`
or `unit` is required; `icon` and `dock` are optional. See
[packaging/examples/koreader.app](packaging/examples/koreader.app).

## Installing reBoard on the device

Grab the binary for your device from the GitHub Actions artifacts (or build it
yourself, see below), then over SSH (USB cable exposes `root@10.11.99.1`):

```sh
scp reboard root@10.11.99.1:/home/root/reboard
scp packaging/reboard.service root@10.11.99.1:/etc/systemd/system/
ssh root@10.11.99.1 "systemctl daemon-reload && systemctl enable --now reboard"
```

To try it once without installing the service:

```sh
ssh root@10.11.99.1
systemctl stop xochitl
QT_QUICK_BACKEND=epaper QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS="rotate=180:invertx" ./reboard -platform epaper
```

Useful environment variables:

| Variable | Purpose |
| --- | --- |
| `REBOARD_TOUCH_DEVICE` | Force a specific `/dev/input/event*` device |
| `REBOARD_TOUCH_INVERT_X` | Invert the X axis of the touch screen |
| `REBOARD_TOUCH_NO_INVERT_Y` | Do not invert the Y axis (inverted by default for reMarkable 2) |

## Building

Everyday tasks are automated with the root `Makefile` (run `make help`):

```sh
make deps        # install host dependencies (cmake, gcovr) into ~/.local
make test        # build and run the unit tests on the host
make coverage    # unit tests + gcovr coverage report
make sdk-rm2     # download and install the official SDK (also: sdk-rm1, sdk-ferrari)
make cross-rm2   # cross-compile the launcher for reMarkable 2
make deploy      # scp the rm2 binary and service file to RM_HOST (default root@10.11.99.1)
```

The business core (domain + application + infrastructure) is plain C++17 and
builds on any Linux host; only the launcher UI needs Qt 6, which comes with
the official SDK sysroots.

## Architecture

```
src/domain/           Value objects, entities and repository interfaces.
                      Pure, immutable, no dependencies.
src/application/      Use cases (list board, launch, close foreground, ...)
                      plus the factory that wires infrastructure in.
src/infrastructure/   Repository implementations: manifest files, POSIX
                      processes + systemctl, evdev touch input.
src/ui/               Qt Quick launcher (epaper platform plugin).
```

Dependency rules: `domain` depends on nothing; `infrastructure` implements
`domain` interfaces; `application` use cases depend only on `domain`, and only
the use-case factory touches `infrastructure`.

## Contributing

reBoard does **not** accept code contributions (pull requests), but bug
reports and feature requests through GitHub issues are very welcome. See
[CONTRIBUTING.md](CONTRIBUTING.md).

## Legal

reBoard is free software licensed under the
[GNU General Public License v3.0](LICENSE).

This is an independent open-source project. It is **not affiliated with,
endorsed by, or supported by reMarkable AS** in any way. The word
"reMarkable" is used solely to refer to the devices the software runs on.
"reMarkable" is a trademark of reMarkable AS. See [NOTICE](NOTICE).
