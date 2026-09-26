# PocketNAS v3.1.0 Beta 1

PocketNAS turns an ARM64 Android phone into a local Wi-Fi file server with a browser file manager, WebDAV compatibility, and a Windows drive that automatically reconnects to the same phone.

## Ready-to-use downloads

- `PocketNAS-v3.1-arm64.apk` — Android 9+ ARM64 application.
- `PocketNAS-Drive-Setup-v3.1.exe` — one-click Windows Drive setup.
- `SHA256SUMS.txt` — release checksums.

## Highlights

- Browser file manager with upload, download, folder creation, rename and delete.
- WebDAV on TCP 8080 for compatibility.
- One-click Windows Drive installer served directly by the Android app.
- Automatic WinFsp/rclone dependency setup through winget.
- Automatic PocketNAS discovery by persistent device ID.
- Automatic Windows drive remount after login, phone reconnect, or DHCP/IP changes.
- Large-file Explorer transfers through rclone + WinFsp instead of the legacy Windows WebClient path.
- Android All files access helper with app-specific settings intent and OEM fallback.
- New PocketNAS branding/icon used by the web UI and Windows integration.

## Security

PocketNAS is intended for trusted local networks. Authentication is off by default and transport is currently unencrypted HTTP. Do not expose TCP port 8080 to the public Internet.

## Beta status

Keep backups of important files and test write/delete operations with non-critical data first.
