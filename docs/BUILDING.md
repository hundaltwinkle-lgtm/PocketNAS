# Building PocketNAS from source

This document describes the v3.0 build model. PocketNAS does not currently use Gradle.

## Prerequisites

- Python 3.10+.
- Android SDK command-line tools.
- Android NDK with an ARM64 API 28 compiler.
- Android build-tools containing `apksigner`.
- JDK `keytool` for creating a release keystore.
- Optional: `adb` for installation/testing.

The provided CI pins Android NDK `26.3.11579264` and build-tools `35.0.0`.

## Windows setup

Install Android command-line tools / Android Studio, then install:

```text
platform-tools
build-tools;35.0.0
ndk;26.3.11579264
```

Set environment variables similar to:

```powershell
$env:ANDROID_HOME = "$env:LOCALAPPDATA\Android\Sdk"
$env:ANDROID_NDK_HOME = "$env:ANDROID_HOME\ndk\26.3.11579264"
```

Build the unsigned APK:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\scripts\build-windows.ps1
```

Output:

```text
build\PocketNAS-v3.0-unsigned.apk
```

## Linux build

```bash
export ANDROID_HOME="$HOME/Android/Sdk"
export ANDROID_NDK_HOME="$ANDROID_HOME/ndk/26.3.11579264"
./scripts/build-linux.sh
```

## Native compiler requirements

The relevant compiler target is:

```text
aarch64-linux-android28
```

The build intentionally uses:

```text
-mno-outline-atomics
```

Do not remove this without testing on older/vendor Android runtimes. The release verifier checks the native ELF for unresolved `__aarch64_*` helper references when `readelf` or `llvm-readelf` is available.

## Generate a permanent public signing key

For the first public release, generate a release keystore and protect it carefully:

```powershell
keytool -genkeypair `
  -keystore pocketnas-release.jks `
  -alias pocketnas `
  -keyalg RSA `
  -keysize 3072 `
  -validity 10000
```

Back up `pocketnas-release.jks` offline. **Losing this key prevents future APKs from updating existing installations.** Leaking it allows an attacker to impersonate your updates.

Never commit it to Git.

## Sign an APK

Locate `apksigner` in the Android SDK build-tools directory.

Windows example:

```powershell
$ApkSigner = "$env:ANDROID_HOME\build-tools\35.0.0\apksigner.bat"
& $ApkSigner sign `
  --ks .\pocketnas-release.jks `
  --ks-key-alias pocketnas `
  --out .\dist\PocketNAS-v3.0-arm64.apk `
  .\build\PocketNAS-v3.0-unsigned.apk
```

Verify:

```powershell
& $ApkSigner verify --verbose --print-certs .\dist\PocketNAS-v3.0-arm64.apk
```

## Generate checksum

```powershell
(Get-FileHash .\dist\PocketNAS-v3.0-arm64.apk -Algorithm SHA256).Hash.ToLower()
```

For a release checksum file:

```powershell
$hash=(Get-FileHash .\dist\PocketNAS-v3.0-arm64.apk -Algorithm SHA256).Hash.ToLower()
"$hash  PocketNAS-v3.0-arm64.apk" | Set-Content .\dist\SHA256SUMS.txt
```

## Install on a test device

```powershell
adb devices
adb install -r .\dist\PocketNAS-v3.0-arm64.apk
```

If the installed experimental build uses a different signing identity, Android will reject the update. In that case, back up needed settings/data and uninstall the old package first:

```powershell
adb uninstall com.pocketnas.wifidrive
adb install .\dist\PocketNAS-v3.0-arm64.apk
```

## Smoke test

Launch:

```powershell
adb shell monkey -p com.pocketnas.wifidrive -c android.intent.category.LAUNCHER 1
```

Check crash buffer:

```powershell
adb logcat -b crash -d
```

From Windows, test server capabilities:

```powershell
curl.exe -v -X OPTIONS "http://PHONE_IP:8080/"
curl.exe -v -X PROPFIND -H "Depth: 0" "http://PHONE_IP:8080/"
```

With authentication:

```powershell
curl.exe --digest -u "pocketnas:YOUR_PASSWORD" -X PROPFIND -H "Depth: 0" "http://PHONE_IP:8080/"
```

## Release-build hygiene

Before publishing:

- build from a clean Git commit;
- verify no private key, password or device identifier is in the repository;
- review requested Android permissions;
- verify APK signature/certificate;
- calculate SHA-256;
- install the signed APK on at least one clean device;
- test read/write/delete operations on expendable files;
- test authentication on/off;
- test reboot behavior;
- test Windows File Explorer access;
- publish the exact source commit used to build the release.
