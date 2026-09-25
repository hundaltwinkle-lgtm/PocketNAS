# Troubleshooting

## App closes immediately

Connect ADB and capture the crash buffer:

```powershell
adb logcat -c
adb shell am force-stop com.pocketnas.wifidrive
adb shell monkey -p com.pocketnas.wifidrive -c android.intent.category.LAUNCHER 1
Start-Sleep -Seconds 3
adb logcat -b crash -d
```

Report the complete `FATAL EXCEPTION`, `VerifyError`, `UnsatisfiedLinkError`, `SIGSEGV` or `SecurityException` block.

## `adb devices` is empty

- Enable Developer options.
- Enable USB debugging.
- Use a data-capable USB cable.
- Select File Transfer/MTP if required by the phone.
- Accept the RSA authorization dialog.
- Run:

```powershell
adb kill-server
adb start-server
adb devices
```

## `STORAGE: PERMISSION REQUIRED`

Grant **All files access** to PocketNAS from Android Settings, then return to the app.

## `NO WIFI OR LAN IP`

- Verify the phone has a reachable IPv4 address.
- Put phone and PC on the same Wi‑Fi/LAN.
- Disable client isolation/guest-network isolation if the router prevents peer-to-peer traffic.

## `PORT 8080 BIND FAILED`

Another process may already be using the address/port, or an earlier PocketNAS listener may not have terminated cleanly. Stop PocketNAS, wait, then restart. Reboot the phone if necessary.

## Windows cannot connect

```powershell
Test-NetConnection PHONE_IP -Port 8080
```

If false, this is network reachability/listener trouble rather than WebDAV authentication.

If true, test:

```powershell
curl.exe -v -X OPTIONS "http://PHONE_IP:8080/"
```

Then:

```powershell
curl.exe -v -X PROPFIND -H "Depth: 0" "http://PHONE_IP:8080/"
```

## Authentication rejected

Use the exact fixed username:

```text
pocketnas
```

Test Digest directly:

```powershell
curl.exe --digest -u "pocketnas:YOUR_PASSWORD" -v -X PROPFIND -H "Depth: 0" "http://PHONE_IP:8080/"
```

If this succeeds but File Explorer does not, clear cached Windows credentials and add the network location again.

## Background service stops

Android vendors may add battery restrictions beyond standard Android behavior. Try:

- App info → Battery → Unrestricted;
- Allow background activity;
- vendor-specific Auto launch / Auto start;
- exclude PocketNAS from aggressive battery optimization.

After an explicit **Force stop**, Android intentionally keeps the application stopped until it is launched manually again.

## Safe bug report

Include:

- app version;
- phone model;
- Android version;
- Windows version;
- auth/read-only state;
- reproducible steps;
- sanitized logcat.

Remove passwords, private IPs if desired, personal filenames, signing material and other secrets.

## Files appear but delete/rename/upload fails

PocketNAS v2.3 distinguishes read/list access from full write/delete access. Look at the Android screen:

```text
STORAGE: FULL READ/WRITE
```

is required for reliable write operations. If it says `STORAGE: LIMITED`, enable **All files access** for PocketNAS and reconnect Windows.

Also verify:

```text
READ ONLY: OFF
```

Android can still deny protected app-private paths such as other apps' `Android/data` content. Test first with a file you created in `Download/`.

## Some media or file formats do not open correctly

v2.3 adds MIME detection and byte-range support. If a file is visible but will not open:

1. Copy/download it to the PC to distinguish protocol issues from the Windows application.
2. Test with `curl.exe` to confirm the server returns the resource.
3. For media, test a byte range:

```powershell
curl.exe -v -H "Range: bytes=0-1023" "http://PHONE_IP:8080/path/to/video.mp4" -o $null
```

Expected status:

```text
HTTP/1.1 206 Partial Content
```

4. If only large files fail through Windows File Explorer, investigate the Windows WebClient configuration/limits separately from PocketNAS.
