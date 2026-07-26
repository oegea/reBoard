# ADR-0006 — App Store catalog and package format

Status: accepted (2026-07-26)

## Context

Story 009: reBoard needs an app store whose catalog lives in this git
repository, with per-device downloadable packages, safe install/uninstall,
and a hard guarantee that base system apps can never be removed.

## Decision

### Catalog (in-repo, served raw over HTTPS)

- `store/catalog.json`: `{ "catalogVersion": 1, "apps": [ { "id", "name",
  "category", "summary", "path" } ] }`. Categories use a fixed slug set
  (`productivity`, `reading`, `games`, `utilities`, `development`, ...).
- `store/apps/<id>/app.json`: long `description`, `version`, `icon`,
  `screenshots[]` (files in the same folder) and `downloads`: a map of
  device slug (`rm1`, `rm2`, `ferrari`, ...) → package URL. Absence of the
  running device's slug means "not available for this device".
- The store app fetches the catalog from a base URL; the default points at
  this repository's raw content. The URL is configurable from Settings
  behind a warning (story 009).

### Package format

- A `.tar.gz` (BusyBox tar can extract it on-device) containing at least:
  `manifest.app` (standard reBoard manifest, story on manifests), the
  binary/assets it needs, and `icon.png`.

### Install layout and uninstall safety

- Packages extract to `/home/root/.local/share/reboard/apps/<id>/`.
- The manifest is copied to `~/.config/reboard/apps-store/<id>.app` — a
  directory reserved for store-managed manifests. The launcher scans it in
  addition to the standard manifest directories.
- **Only apps whose manifest lives in `apps-store/` are uninstallable**
  (long-press X). Built-ins and hand-installed manifests never show the X.
  Uninstall = delete the manifest + the package directory.

## Consequences

- Publishing an app = a PR adding a folder + catalog entry; releases can
  host the package binaries.
- Device slug detection is needed on-device (from the SDK image metadata;
  fallback `rm2`).
- The store app needs Qt Network (present in the sysroot) and tar via the
  existing process infrastructure.
