# GitHub update required

The public repository was inspected on 2026-09-26 and still contains the v2.3.0-beta.1 source line.

This source package is the newer PocketNAS v3.0.0-beta.1 line and adds:

- Browser File Manager
- `/api/status` device discovery endpoint
- PocketNAS Windows Drive workflow based on rclone + WinFsp
- automatic phone rediscovery/reconnect logic
- large-file Explorer workflow that avoids Windows WebClient limitations
- Android All-files-access settings flow improvements
- generated PocketNAS professional icon under `assets/pocketnas-icon.png`

Replace/update the public repository source from this package before publishing a v3 release.

Do not publish private signing keys or keystores.
