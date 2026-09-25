# Android storage access

PocketNAS exposes the phone's shared storage by using normal Android/Linux file APIs. On modern Android, **declaring storage permissions in the manifest is not enough** to obtain broad file access.

## Required for full read/write/delete

For full shared-storage access, enable Android's special **All files access** permission for PocketNAS:

```text
Settings
→ Apps / Special app access
→ All files access / Manage all files
→ PocketNAS
→ Allow
```

Vendor wording varies. On Realme/ColorOS, use Settings search for **All files access**, **Manage all files**, or **Special app access**.

PocketNAS v2.3 actively probes a temporary file in the shared-storage root and reports one of these states:

```text
STORAGE: FULL READ/WRITE
```

or

```text
STORAGE: LIMITED - ENABLE ALL FILES ACCESS
```

`STORAGE: LIMITED` means directory listing may still work while upload, rename, overwrite, or delete may fail.

## Android platform restrictions that remain

Even with `MANAGE_EXTERNAL_STORAGE`, Android does **not** give a normal third-party application unrestricted access to everything on the device. In particular, modern Android restricts access to app-private storage and may restrict other apps' locations under:

```text
/storage/emulated/0/Android/data/
/storage/emulated/0/Android/obb/
/data/
```

PocketNAS intentionally does not attempt to bypass the Android sandbox, root the device, or exploit system permissions.

## Why a file can appear but still fail to delete

Android/FUSE may allow directory enumeration while denying a later `unlink()` or `rmdir()` operation. PocketNAS therefore treats listing access and write/delete access as separate capabilities.

If Windows reports **Access denied** when deleting:

1. Confirm `READ ONLY: OFF` inside PocketNAS.
2. Confirm `STORAGE: FULL READ/WRITE` inside PocketNAS.
3. Check Android's **All files access** setting.
4. Test with a normal file in `Download/` first.
5. If the file is under another app's protected directory, Android may intentionally block the operation.
