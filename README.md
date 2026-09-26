# PocketNAS v3

> Turn an Android phone into a local Wi‑Fi file server, browser file manager, WebDAV share, and automatically reconnecting Windows drive.

![Status](https://img.shields.io/badge/status-beta-orange)
![Version](https://img.shields.io/badge/version-3.1.0--beta.1-blue)
![Android](https://img.shields.io/badge/Android-9%2B-green)
![ABI](https://img.shields.io/badge/ABI-arm64--v8a-blue)
![Protocol](https://img.shields.io/badge/protocol-WebDAV-purple)
![License](https://img.shields.io/badge/license-Apache--2.0-blue)

PocketNAS is a lightweight Android NAS-style application for trusted local networks. The phone serves its shared storage over Wi‑Fi and can be used in three ways:

1. **Browser File Manager** — open the phone address in Chrome, Edge, Firefox, Safari, etc.
2. **WebDAV** — connect directly from Windows File Explorer or another WebDAV client.
3. **PocketNAS Windows Drive** — download the setup script from the phone once, then Windows automatically discovers the same phone and mounts it as a drive such as `P:` using rclone + WinFsp. This path is intended for large files and avoids the legacy Windows WebClient transfer limitations.

> [!IMPORTANT]
> PocketNAS v3 is beta software. Keep backups of important data. Test delete/rename/upload operations with non-critical files first. Do not expose TCP port 8080 directly to the public Internet.

## Ready downloads

Normal users do **not** need Git, Android Studio, Go, Python, rclone configuration, or manual WebDAV registry editing.

- **Android APK:** [Download PocketNAS v3.1 for ARM64](https://github.com/hundaltwinkle-lgtm/PocketNAS/releases/download/v3.1.0-beta.1/PocketNAS-v3.1-arm64.apk)
- **Windows one-click installer:** [Download PocketNAS Drive Setup v3.1](https://github.com/hundaltwinkle-lgtm/PocketNAS/releases/download/v3.1.0-beta.1/PocketNAS-Drive-Setup-v3.1.exe)
- **Checksums:** [SHA256SUMS.txt](https://github.com/hundaltwinkle-lgtm/PocketNAS/releases/download/v3.1.0-beta.1/SHA256SUMS.txt)

The Android app also serves the same Windows installer directly from the phone at `http://PHONE-IP:8080/PocketNAS-Drive-Setup-v3.1.exe`.

## Highlights in v3

- Same Android WebDAV/browser core as the working v2.x line.
- **Automatic Windows Drive setup** served directly by the APK.
- Windows setup installs **WinFsp** and **rclone** through `winget` when required.
- Automatic phone discovery on the LAN.
- Stable per-install PocketNAS **device ID** so the Windows watcher reconnects to the same phone.
- Automatic Windows drive remount after sign-in, Wi‑Fi reconnect, or phone IP change.
- Large-file transfers through rclone/WinFsp rather than Windows' built-in WebClient redirector.
- Windows WebClient registry tuning retained as a compatibility fallback.
- Browser button: **Windows Drive Setup**.
- New local API: `GET /api/status`.
- Fixed Android **All files access** launcher: app-specific settings first, OEM/general settings fallback second.
- Authentication OFF by default; optional permanent password + HTTP Digest authentication remains available.
- Foreground service, boot start, Wi‑Fi reconnect, browser file manager, byte ranges, upload/download/rename/delete/copy/move remain available.

## Requirements

### Android phone

- Android 9 or newer.
- ARM64 / `arm64-v8a` CPU.
- Phone and client device on the same reachable LAN/Wi‑Fi network.
- **All files access** is required for broad shared-storage upload/rename/delete operations.

### Windows PC — WebDAV only

- Windows 10 or Windows 11.
- Windows WebClient service available.

### Windows PC — automatic `P:` drive

- Windows 10 or Windows 11.
- `winget` / App Installer available for first-time dependency installation.
- Internet access during the first Windows setup if WinFsp/rclone are not already installed.
- Administrator approval once for dependency installation and WebClient compatibility settings.

After setup, the Android APK remains the server and the Windows watcher automatically finds the phone over the local network.

---

# Quick Start

## 1. Install PocketNAS

ADB installation:

```powershell
adb install PocketNAS-v3.1-arm64.apk
```

If Android reports a signing-certificate mismatch with an older experimental build, uninstall that development build once and install the public v3.1 APK. Future public releases must use the same v3.1 release signing identity.

```powershell
adb install PocketNAS-v3.1-arm64.apk
```

Open PocketNAS once after installation.

## 2. Grant full storage access

PocketNAS reports one of these states:

```text
STORAGE: FULL READ/WRITE
```

or:

```text
STORAGE: LIMITED - ENABLE ALL FILES ACCESS
```

If access is limited, tap:

```text
GRANT FULL STORAGE ACCESS
```

v3 first requests the app-specific Android page:

```text
Settings → Special app access → All files access → PocketNAS
```

If an OEM does not implement that page, PocketNAS falls back to the general **All files access** list.

Android may still protect parts of `Android/data` and `Android/obb` even when broad file access is granted.

## 3. Note the PocketNAS address

The Android screen shows something similar to:

```text
http://192.168.1.20:8080/
```

The IP may change when the phone reconnects to Wi‑Fi. Browser/WebDAV users can use the current displayed address. PocketNAS Windows Drive v3 can rediscover the phone automatically.

---

# Option A — Browser File Manager

Open the PocketNAS address in a browser:

```text
http://192.168.1.20:8080/
```

The browser interface supports:

- folder navigation;
- search/filter of the current directory;
- open/stream browser-supported media;
- explicit downloads;
- HTTP byte-range reads for large media;
- multi-file upload;
- create folder;
- rename;
- delete;
- read-only mode;
- optional PocketNAS authentication.

Unknown file extensions are still served as `application/octet-stream`; PocketNAS does not intentionally filter files by extension.

For very large downloads, browser mode is often more reliable than Windows' legacy WebClient implementation.

---

# Option B — Windows File Explorer WebDAV

Windows File Explorer can use PocketNAS directly as a WebDAV network location.

1. Open **This PC**.
2. Choose **Add a network location**.
3. Choose **custom network location**.
4. Enter the URL displayed by PocketNAS, for example:

   ```text
   http://192.168.1.20:8080/
   ```

5. Give it a friendly name such as `PocketNAS Phone`.

When PocketNAS authentication is enabled:

```text
Username: pocketnas
Password: <your permanent password>
```

### Windows WebClient limitations

Microsoft's built-in WebDAV redirector has legacy file-size, timeout and caching behavior. PocketNAS v3 keeps WebDAV for compatibility, but the recommended Windows workflow for multi-GB files is **PocketNAS Windows Drive** below.

---

# Option C — PocketNAS Windows Drive (recommended for Windows)

PocketNAS v3.1 serves its one-click Windows Drive installer directly from the phone.

On the Windows PC:

1. Open the PocketNAS URL in Chrome/Edge/Firefox.
2. Click **Install Windows Drive**.
3. Save `PocketNAS-Drive-Setup-v3.1.exe`.
4. Run the EXE.
5. Approve the normal Windows administrator/UAC prompt if dependency installation is required.

The installer then:

1. configures Windows WebClient fallback limits;
2. installs/updates **WinFsp** through `winget`;
3. installs/updates **rclone** through `winget`;
4. discovers the PocketNAS phone on the LAN;
5. records the phone's stable PocketNAS device ID;
6. creates an rclone WebDAV configuration;
7. asks for the permanent PocketNAS password only if Android authentication is currently ON;
8. chooses `P:` when available (otherwise another free letter from `P:`–`Z:`);
9. creates a hidden Windows auto-mount watcher;
10. starts that watcher at Windows sign-in;
11. mounts the phone as a Windows drive;
12. automatically rediscovers/remounts the phone if its Wi‑Fi IP changes.

Typical result:

```text
This PC
├── Local Disk (C:)
├── Data (D:)
└── PocketNAS (P:)
    ├── DCIM
    ├── Download
    ├── Documents
    ├── Movies
    ├── Music
    └── Pictures
```

### Advanced fallback

`PocketNAS-Windows-Setup.ps1` remains in the source tree for troubleshooting and advanced/manual installation. Normal users should use the EXE.

### Why v3 uses rclone + WinFsp

The Android server already streams large files correctly through HTTP/WebDAV. Windows File Explorer's built-in WebClient redirector can nevertheless remain at `Calculating…`/`0%` or hit historical WebDAV limits. The v3 Windows Drive path gives Explorer a filesystem drive backed by rclone/WinFsp rather than the WebClient redirector.

### Automatic reconnection

The Windows watcher repeatedly checks PocketNAS availability. It identifies the correct phone through `/api/status` and the stored PocketNAS device ID. If the phone changes from one DHCP address to another, the watcher can update the rclone endpoint and remount automatically.

The watcher files are stored under:

```text
%LOCALAPPDATA%\PocketNAS\
```

The startup launcher is placed in the current Windows user's Startup folder.

---

# Android Application Controls

## Start / Stop Server

PocketNAS listens on TCP port `8080` while the server is running.

## Read Only

When enabled, mutating operations are rejected. Read operations remain available.

## Set Permanent Password

The current native password editor accepts 4–16 digits. The value is saved in app-private storage and normally survives reboot and same-package updates. Clearing app data or uninstalling PocketNAS removes it.

## Authentication

Authentication is OFF by default. When enabled, PocketNAS uses HTTP Digest authentication:

```text
Username: pocketnas
Realm: PocketNAS
```

The Windows v3 setup asks for the password only when `/api/status` reports authentication enabled.

## Background operation

PocketNAS includes:

- Android foreground service;
- `BOOT_COMPLETED` receiver;
- package-update restart receiver;
- sticky service recovery;
- local network/IP monitoring.

Android/OEM battery-management policies can still stop third-party background processes. On aggressive firmware, allow PocketNAS unrestricted/background battery use.

---

# Local API

## `GET /api/status`

PocketNAS v3 exposes a small unauthenticated LAN discovery endpoint so the Windows watcher can find the phone before it has mounted the authenticated WebDAV share.

Example:

```json
{
  "app": "PocketNAS",
  "version": "3.0",
  "deviceId": "c13a6c1c65e84251",
  "auth": false,
  "readOnly": false,
  "storageWritable": true,
  "ip": "192.168.1.20",
  "port": 8080
}
```

The endpoint does **not** expose the saved PocketNAS password or file listing.

## `GET /PocketNAS-Windows-Setup.ps1`

Downloads the Windows setup script served from the APK. This endpoint is intentionally available on the LAN even before WebDAV authentication so a new Windows PC can bootstrap the client configuration.

---

# WebDAV Methods

| Method | Purpose | v3 |
|---|---|---:|
| `OPTIONS` | Capability discovery | ✅ |
| `PROPFIND` | Directory/resource metadata | ✅ |
| `GET` | Read/download/stream | ✅ |
| `HEAD` | Metadata | ✅ |
| `PUT` | Upload/overwrite | ✅ |
| `DELETE` | Delete | ✅ |
| `MKCOL` | Create directory | ✅ |
| `MOVE` | Rename/move | ✅ |
| `COPY` | Copy | ✅ |
| `PROPPATCH` | Compatibility response | Basic |
| `LOCK` | Compatibility lock | Basic |
| `UNLOCK` | Compatibility unlock | Basic |

PocketNAS is a pragmatic local Windows-oriented WebDAV implementation, not a claim of complete RFC 4918 conformance.

---

# Architecture

```text
Android phone
┌──────────────────────────────────────────────┐
│ PocketNAS v3                                 │
│                                              │
│ NativeActivity UI                            │
│ Foreground service + boot receiver           │
│ Storage permission/state manager             │
│ WebDAV/HTTP server :8080                     │
│ Browser File Manager                         │
│ /api/status discovery endpoint               │
│ Windows setup script endpoint                │
└──────────────────┬───────────────────────────┘
                   │ Same LAN / Wi‑Fi
       ┌───────────┼───────────────────┐
       │           │                   │
       ▼           ▼                   ▼
 Web browser   WebDAV client     PocketNAS Windows Drive
                                   │
                                   ├─ rclone WebDAV client
                                   ├─ WinFsp filesystem
                                   ├─ auto-discovery watcher
                                   └─ P: drive in Explorer
```

The Android native library is implemented primarily in C. A very small generated DEX layer provides the Android foreground service and boot receiver.

---

# Building from Source

See [`docs/BUILDING.md`](docs/BUILDING.md).

High-level Android build requirements:

- Android NDK;
- ARM64 target `aarch64-linux-android28`;
- `-mno-outline-atomics` for compatibility with tested devices;
- Python 3 for manifest/DEX/APK packaging helpers;
- a persistent private release signing key for distributable upgrades.

Example Linux build:

```bash
export ANDROID_NDK_HOME=/path/to/android-ndk
./scripts/build-linux.sh
```

The repository intentionally does **not** contain the private APK signing key.

---

# Repository Layout

```text
PocketNAS/
├── .github/                 GitHub workflows and templates
├── assets/                  project artwork/icon concept
├── docs/                    architecture, build and support documentation
├── release/                 release notes/checksums guidance
├── scripts/                 build scripts
├── tools/                   manifest/DEX/APK helper tools
├── windows/                 PocketNAS Windows setup source
├── pocketnas.c              Android/WebDAV/browser native core
├── README.md
├── CHANGELOG.md
├── CONTRIBUTING.md
├── SECURITY.md
├── LICENSE
└── NOTICE
```

---

# Security Model

PocketNAS is intended for a **trusted local network**.

- Do not port-forward `8080` to the Internet.
- Enable PocketNAS authentication on networks you do not fully trust.
- The browser and WebDAV interfaces operate with the Android storage rights granted to PocketNAS.
- The Windows setup script installs third-party open-source components (WinFsp and rclone) using Windows Package Manager.
- Windows UAC approval is required before system-level dependency installation.
- `/api/status` intentionally reveals a limited device-identification/status record to the local LAN for discovery. It does not reveal passwords or file contents.

Report security issues using [`SECURITY.md`](SECURITY.md).

---

# Known Limitations

- Current APK release is ARM64 only.
- Android may restrict `Android/data` and `Android/obb` even with broad storage access.
- OEM background restrictions can affect always-on behavior.
- Native Windows WebDAV remains subject to Windows WebClient behavior; use the v3 Windows Drive for large-file workflows.
- First Windows Drive setup requires Internet access if rclone/WinFsp are not already installed.
- The icon file in `assets/` is the current design concept; this minimal native APK build pipeline does not yet compile a custom Android resource table for the launcher icon.

---

# Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md). Bug reports should include Android model/version, PocketNAS version, client OS, connection method (browser/WebDAV/Windows Drive), and relevant logs where possible.

# License

Apache License 2.0. See [`LICENSE`](LICENSE) and [`NOTICE`](NOTICE).
