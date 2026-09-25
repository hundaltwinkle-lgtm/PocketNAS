# Changelog

All notable changes to PocketNAS are documented here.

## [2.3.0-beta.1] - 2026-09-25

### Fixed
- Detects the difference between merely being able to list shared storage and having full read/write/delete access.
- DELETE handling now retries unknown Android/FUSE entry types and returns a clearer `403 Forbidden` when Android storage policy prevents deletion.
- Corrects the misleading `STORAGE: READY` state that could appear without full write/delete permission.
- Corrects the HTTP `Server` response version to `PocketNAS/2.3`.

### Added
- MIME type detection for common documents, images, video, audio, archives, APKs, SQLite databases, fonts and web files.
- HTTP single-range requests (`Range: bytes=...`) with `206 Partial Content`, improving Windows/media playback and large-file access.
- `416 Range Not Satisfiable` handling.
- UI storage state `STORAGE: FULL READ/WRITE` vs `STORAGE: LIMITED - ENABLE ALL FILES ACCESS`.
- Better failure status in the Android UI when write/delete is blocked by Android permissions.

### Compatibility note
Android itself blocks access to some protected locations, especially other applications' `Android/data` and `Android/obb` directories on modern Android. PocketNAS does not bypass Android's sandbox.

## [2.2.0-beta.1] - 2026-09-25

### Added

- WebDAV Wi-Fi file access for Windows File Explorer.
- Native Android UI.
- Upload/download/open support.
- Directory creation.
- Rename/move/copy/delete operations.
- Read-only mode.
- Optional permanent 4–16 digit password.
- HTTP Digest authentication.
- Foreground service for background operation.
- Auto-start after `BOOT_COMPLETED` and package replacement.
- Network-address monitoring and server restart.
- Request/transfer counters.

### Fixed

- Digest authentication compatibility issue from the early v1 test line.
- ARM64 startup crash caused by unresolved outlined-atomic helper symbols; native build now requires `-mno-outline-atomics`.
- Generated DEX `VerifyError` caused by incorrect Dalvik format `35c` invoke nibble encoding.

### Known limitations

See the README's **Known limitations** section. This release should be published as a pre-release/beta rather than a stable production release.
