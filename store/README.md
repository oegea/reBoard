# reBoard App Store catalog

This directory is the source of truth for the reBoard App Store (story 009,
ADR-0006). The store app fetches these files raw over HTTPS.

## Adding an app

1. Add an entry to [catalog.json](catalog.json):

   ```json
   { "id": "koreader", "name": "KOReader", "category": "reading",
     "summary": "Ebook reader with wide format support", "path": "apps/koreader" }
   ```

2. Create `apps/<id>/` containing:
   - `app.json` — full record:

     ```json
     {
         "id": "koreader",
         "name": "KOReader",
         "category": "reading",
         "version": "1.0.0",
         "description": "Longer description shown on the app page.",
         "icon": "icon.png",
         "screenshots": ["shot1.png"],
         "downloads": {
             "rm2": "https://example.com/koreader-rm2.tar.gz",
             "rm1": "https://example.com/koreader-rm1.tar.gz"
         }
     }
     ```

   - `icon.png` and any screenshots referenced above.

3. Each download is a `.tar.gz` containing at least `manifest.app` (a
   standard reBoard application manifest), `icon.png` and the binary/assets
   the app needs. It is extracted to
   `/home/root/.local/share/reboard/apps/<id>/` on the device.

Apps without a download link for the running device are shown as not
available for it.
