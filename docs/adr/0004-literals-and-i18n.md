# ADR-0004 — User-facing literals and internationalization

Status: accepted (2026-07-26)

## Context

reBoard is the visual layer of the device (ADR-0001); every user-facing
string must be translatable across the whole system, with no per-language
hacks in code.

## Decision

- Source language is **English**. Every user-facing literal goes through
  Qt's i18n: `qsTr()` in QML, `tr()` in C++ when ever needed.
- Translations live in catalogs under `libs/rekit/i18n/` (`reboard_<lang>.ts`,
  hand-maintained XML), compiled to `.qm` at build time and embedded under
  `:/i18n` in every app. Adding a language = adding one `.ts` file.
- Locale resolution, identical in every app:
  1. `REBOARD_LOCALE` environment variable (testing/override);
  2. persisted user choice (`language` key in the shared reBoard config,
     `~/.config/reboard/reboard.conf`, INI via QSettings) — "system" means
     no preference;
  3. `QLocale::system()`.
- Missing translations fall back to the English source string; a toolchain
  without Qt LinguistTools builds without catalogs (warning, English only).

## Consequences

- The Settings language picker only writes the config key; apps pick it up
  on next start. Live relanguage is a possible future refinement.
- Translators never touch code; contexts follow QML file names.
- No string concatenation for sentences; use `%1` placeholders so word
  order can change per language.
