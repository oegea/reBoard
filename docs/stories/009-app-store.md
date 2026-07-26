# Story 009 — App Store

## User story

As a reBoard user, I want an App Store app — UI inspired by Apple's App
Store, super usable and easy to browse — where apps are organized in the
usual store categories, so I can discover, install and uninstall
applications on my tablet without touching a terminal.

## Catalog (lives in this repository)

- `store/catalog.json` — the index: for every app its id, title, category
  and one-line summary, plus the path to its folder. Kept deliberately
  simple.
- `store/apps/<id>/app.json` — full record: long description, version,
  icon/screenshot file names (stored in the same folder), and one download
  link per supported device (`rm1`, `rm2`, `ferrari`, ...) pointing to the
  package (binary + icon + manifest + assets).
- Package format and install layout: ADR-0006.

## Behaviour

- Browse by category; every screen composed from reKit components.
- App page: icon, description, screenshots, install button.
- Install: confirm → download the package for THIS device → extract →
  the app appears in the board grid (top area) and opens normally.
- Uninstall: long-press on a board icon shows an "X" badge (iOS style).
  **Only store-installed apps ever get the X** — never the base system
  entries (Notebooks, Settings). Confirmation before removal.
- Settings integration: the store repository URL can be changed under
  Settings, with clear warnings recommending the default repository.
- No reviews/ratings for now.

## Acceptance criteria

- [ ] Store lists the catalog by category and renders app pages.
- [ ] Install downloads the right package for the running device and the
      app shows up on the board immediately after.
- [ ] Long-press uninstall works exactly and only for store apps.
- [ ] Repository URL change lives in Settings behind a warning.
- [ ] Works offline gracefully (clear error states, retry).
