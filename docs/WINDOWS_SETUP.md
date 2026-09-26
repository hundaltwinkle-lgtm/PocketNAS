# Windows 10/11 setup

## Add PocketNAS as a network location

1. Put the phone and PC on the same reachable local network.
2. Open PocketNAS and confirm `SERVER: RUNNING`.
3. Note the displayed URL, for example `http://192.168.1.20:8080/`.
4. Open **File Explorer → This PC**.
5. Choose **Add a network location**.
6. Select **Choose a custom network location**.
7. Enter the PocketNAS URL.
8. Finish the wizard.

With authentication enabled:

```text
Username: pocketnas
Password: the permanent password saved in the app
```

## Do not use SMB syntax

PocketNAS v3.1 is WebDAV, not SMB. These are not equivalent:

```text
Correct: http://192.168.1.20:8080/
Wrong:   \\192.168.1.20\Storage
```

## Check connectivity

```powershell
Test-NetConnection 192.168.1.20 -Port 8080
```

Expected:

```text
TcpTestSucceeded : True
```

## WebClient service

Windows File Explorer WebDAV support is associated with the WebClient service.

```powershell
Get-Service WebClient
```

If stopped and available:

```powershell
Start-Service WebClient
```

## Protocol test

```powershell
curl.exe -v -X OPTIONS "http://192.168.1.20:8080/"
```

Expected status: `200 OK`.

```powershell
curl.exe -v -X PROPFIND -H "Depth: 0" "http://192.168.1.20:8080/"
```

Expected status with auth off: `207 Multi-Status`.

When authentication is on:

```powershell
curl.exe --digest -u "pocketnas:12345678" -X PROPFIND -H "Depth: 0" "http://192.168.1.20:8080/"
```

Replace the example password.

## PocketNAS v3 automatic Windows drive

For large files and automatic reconnection, v3 recommends the setup served by the APK itself. Open the PocketNAS address in a Windows browser and click **Windows Drive Setup**. See [WINDOWS_AUTO_DRIVE.md](WINDOWS_AUTO_DRIVE.md).
