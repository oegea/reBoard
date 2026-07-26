# Story 012 — UX polish, round 2 (feedback from device testing)

## User story

As a reBoard user, I want the details to keep converging on the iOS feel:
the unlock knob should be a rounded pill (not a square-ish block), long
scrollable content should show an iOS-style scroll indicator, and Back
navigation should look like the platform convention (chevron + label,
top-left, borderless) instead of an improvised bordered button.

## Scope

1. **SlideToUnlock knob**: pill-shaped (fully rounded ends), like the
   original iOS slider.
2. **reKit `ScrollIndicator`**: thin rounded vertical bar tracking any
   Flickable's position, shown only when the content overflows. Applied to
   the Settings main list and the license viewer; mandatory from now on for
   every scrollable screen.
3. **reKit `BackButton`**: drawn left chevron + translated "Back" label,
   borderless, generous touch target, placed top-left; the page title stays
   centered while a back navigation is active (nav-bar convention).

## Acceptance criteria

- [ ] Unlock knob reads as a pill; chevron stays crisp.
- [ ] License page and Settings list show the indicator while their
      content overflows; it tracks the scroll position.
- [ ] Back in the license page is the standard component, top-left, with
      centered page title.
