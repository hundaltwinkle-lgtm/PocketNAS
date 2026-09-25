# Publishing PocketNAS professionally on GitHub

This is a maintainer checklist for turning this source tree into a public open-source repository.

## 1. Create the repository

Suggested repository name:

```text
PocketNAS
```

Suggested description:

> Turn an Android phone into a lightweight WebDAV Wi‑Fi drive for Windows File Explorer — no root required.

Suggested topics:

```text
android
webdav
wifi-file-transfer
nas
windows
file-server
network-drive
arm64
native-android
open-source
```

Recommended initial visibility: **Public** when you are ready for the code/security details to be public.

## 2. Initialize Git locally

From the extracted repository folder:

```powershell
git init
git add .
git commit -m "Open-source PocketNAS v2.3 beta"
git branch -M main
```

Create the empty GitHub repository, then add its URL:

```powershell
git remote add origin https://github.com/YOUR-USERNAME/PocketNAS.git
git push -u origin main
```

## 3. Repository settings

Recommended:

- Enable **Issues**.
- Enable **Discussions** if you want user support/community threads.
- Enable **Private vulnerability reporting** under Security.
- Enable Dependabot/security alerts where applicable.
- Protect the `main` branch after the initial push.
- Require pull requests for external changes.
- Keep GitHub Actions permissions minimal.

## 4. Signing identity before public release

Do not use an experimental signing key unless you securely control and have backed up its private key.

Create a new permanent public-release keystore on your PC and keep offline backups. The first public APK signed by that key becomes the update identity for public users.

If your own phone currently has an experimental build signed with another key, Android may require you to uninstall it before installing the first public release.

## 5. Build and test

Follow `docs/BUILDING.md` and `docs/RELEASE_CHECKLIST.md`.

Do not publish until:

- app launches without crash;
- File Explorer connection works;
- authentication on/off works;
- reboot/background behavior is tested;
- APK signing is verified;
- SHA-256 is generated.

## 6. Tag the beta

Recommended first public tag:

```text
v2.3.0-beta.1
```

Commands:

```powershell
git tag -a v2.3.0-beta.1 -m "PocketNAS v2.3.0 beta 1"
git push origin v2.3.0-beta.1
```

## 7. Create the GitHub release

Title:

```text
PocketNAS v2.3.0 Beta 1
```

Mark it as **Pre-release**.

Upload:

```text
PocketNAS-v2.3-arm64.apk
SHA256SUMS.txt
```

Recommended release body:

```markdown
## PocketNAS v2.3.0 Beta 1

PocketNAS turns an ARM64 Android 9+ phone into a WebDAV Wi-Fi drive that can be accessed from Windows File Explorer on the same reachable local network.

### Highlights
- Windows WebDAV access on port 8080
- Browse, upload, overwrite, create folders, rename/move, copy and delete
- Read-only mode
- Optional permanent password / Digest authentication
- Foreground background service
- Auto-start after Android reboot
- Wi-Fi/IP reconnect handling

### Installation
1. Download `PocketNAS-v2.3-arm64.apk`.
2. Install it on an ARM64 Android 9+ phone.
3. Grant All files access if requested.
4. Open PocketNAS and note the displayed `http://PHONE-IP:8080/` address.
5. On Windows use **This PC → Add a network location** and enter that address.

### Security notice
Authentication is off by default and the connection uses unencrypted HTTP. PocketNAS is intended only for trusted local networks. Read `SECURITY.md` before regular use.

### Status
This is a beta/pre-release. Please report reproducible issues with Android model/version, Windows version and sanitized ADB logs.
```

## 8. Release assets and checksums

Publish the SHA-256 next to the APK. Users can verify on Windows with:

```powershell
Get-FileHash .\PocketNAS-v2.3-arm64.apk -Algorithm SHA256
```

## 9. Screenshot/video

Add at least:

- PocketNAS home screen showing server status and URL;
- Windows “Add a network location” screen;
- Windows File Explorer browsing phone storage.

Do not expose real passwords, private personal filenames, device IDs or sensitive IP/network information in promotional screenshots.

## 10. After release

- Triage bugs in Issues.
- Keep security reports private until patched.
- Update CHANGELOG for every release.
- Never delete/recreate tags for an already-published binary.
- Sign every future APK with the same public-release key.
