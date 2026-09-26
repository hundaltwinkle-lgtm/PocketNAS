# PocketNAS architecture

## Overview

PocketNAS v3.1 is a deliberately small Android application consisting of a native C core plus a minimal DEX bridge for Android service/receiver lifecycle integration.

## Runtime components

### `android.app.NativeActivity`

The launcher activity loads `libpocketnas.so` through the `android.app.lib_name=pocketnas` manifest metadata. The native entry point `ANativeActivity_onCreate` initializes configuration, storage state, network monitoring, the WebDAV listener and the native UI.

### `libpocketnas.so`

`pocketnas.c` contains:

- framebuffer-style native UI drawing;
- Android NativeActivity/input callbacks;
- IPv4 interface selection;
- WebDAV HTTP request parsing;
- URL decoding/path mapping and traversal rejection;
- file/directory operations;
- recursive copy/delete;
- HTTP Digest authentication;
- app-private configuration persistence;
- request and transfer counters;
- foreground-service launcher via JNI;
- network monitoring and server restart logic.

The storage root is derived from the NativeActivity external-data path by stripping the `/Android/...` suffix, producing the shared-storage root (commonly `/storage/emulated/0`).

### `PocketNasService`

A tiny generated DEX class extends `android.app.Service`. It:

1. enters foreground mode and posts an ongoing notification;
2. loads the native library;
3. calls the native `nativeStart` entry point;
4. returns `START_STICKY` from `onStartCommand`;
5. invokes `nativeStop` when destroyed.

### `BootReceiver`

A small generated broadcast receiver starts the foreground service for:

- `android.intent.action.BOOT_COMPLETED`;
- `android.intent.action.MY_PACKAGE_REPLACED`.

## Server lifecycle

The native state includes `g_server_wanted`, `g_server_running`, `g_server_starting` and `g_listen_fd`. A network-monitor thread checks the preferred IPv4 address roughly once per second. If the address changes, it stops and recreates the listener.

The listener binds specifically to the selected IPv4 interface address on port `8080`, sets `SO_REUSEADDR`, listens with backlog 16 and creates one detached POSIX thread per accepted client.

## WebDAV request path

```text
TCP accept
  → read HTTP request headers/body
  → parse method/URI/WebDAV headers
  → OPTIONS bypasses auth for capability discovery
  → optional Digest authentication
  → URL decode
  → reject `..` path segments
  → map URL path under shared-storage root
  → dispatch method
  → filesystem operation
  → HTTP/WebDAV response
```

## Authentication

Authentication is off by default. When enabled, the server expects HTTP Digest credentials with:

```text
realm="PocketNAS"
algorithm=MD5
qop="auth"
username="pocketnas"
```

The configured password is stored in the app-private configuration file. The implementation calculates Digest HA1/HA2 and validates the client response.

## DEX generation

The repository does not compile Java/Kotlin source for v3.1. `tools/make_dex.py` emits a small DEX file directly. This unusual design keeps the application minimal but increases maintenance risk. v3.1 retains the fix for Dalvik format `35c` invoke encoding: the G register nibble and argument-count nibble must be placed correctly.

A future Gradle/Java/Kotlin service layer would be easier for contributors and is recommended as a modernization path.

## Native ARM64 compatibility

The native library must be compiled with `-mno-outline-atomics`. An earlier experimental build referenced unresolved AArch64 atomic helper symbols such as `__aarch64_swp4_acq_rel`, causing `UnsatisfiedLinkError` on the tested device.

## Concurrency

The server uses POSIX threads and small spin locks based on compiler atomic builtins. Important shared state includes:

- activity/window/input pointers;
- service/server lifecycle flags;
- client/request/byte counters;
- current IP and status strings;
- authentication/read-only settings.

The implementation is intentionally compact. Contributors changing lifecycle, networking or UI synchronization should test aggressively for races and deadlocks.

## Security boundaries

The app-private config file is protected by the Android application sandbox. Shared files are not copied into a private sandbox; WebDAV operations act on the mapped shared-storage paths directly.

The HTTP transport is unencrypted. Treat the network as part of the trust boundary.
