# Release checklist

## Code and version

- [ ] Confirm intended version name/code in `tools/make_manifest.py`.
- [ ] Confirm source banner/version strings.
- [ ] Update `CHANGELOG.md`.
- [ ] Update README known limitations.
- [ ] Run static/manual review of networking and filesystem changes.

## Build

- [ ] Build from a clean Git commit.
- [ ] Compile native code with `-mno-outline-atomics`.
- [ ] Generate manifest and DEX.
- [ ] Package unsigned APK.
- [ ] Sign with the permanent release key.
- [ ] Verify signature with `apksigner verify --verbose --print-certs`.
- [ ] Run `tools/verify_release.py`.
- [ ] Generate SHA-256 checksum.

## Device test

- [ ] Clean install.
- [ ] Upgrade from previous public release.
- [ ] App launches without crash.
- [ ] Storage permission handling.
- [ ] Server starts/stops.
- [ ] Authentication OFF connection.
- [ ] Permanent password save.
- [ ] Authentication ON Digest connection.
- [ ] Read-only behavior.
- [ ] Upload/download/create/rename/copy/delete.
- [ ] Screen-off/background behavior.
- [ ] Reboot auto-start behavior.
- [ ] Wi-Fi disconnect/reconnect behavior.

## Windows test

- [ ] `OPTIONS` → `200`.
- [ ] `PROPFIND` → `207`.
- [ ] Add Network Location works.
- [ ] Browse/open/download works.
- [ ] Upload/overwrite works.
- [ ] Rename/move works.
- [ ] Delete works.

## Security/repository

- [ ] No signing key in Git history.
- [ ] No credentials in source.
- [ ] No personal device identifiers/logs.
- [ ] `SECURITY.md` current.
- [ ] License present.

## GitHub release

- [ ] Tag uses semantic release name (example `v3.1.0-beta.1`).
- [ ] Release marked **pre-release** while project remains beta.
- [ ] Upload signed APK.
- [ ] Upload `SHA256SUMS.txt`.
- [ ] Include concise installation/security notes.
- [ ] Link exact source commit.
