# Security policy

## Supported versions

PocketNAS is currently in beta. Security fixes are expected to target the latest published release line only.

| Version | Security support |
|---|---|
| Latest beta | ✅ Best effort |
| Older experimental builds | ❌ No |

## Reporting a vulnerability

Please do **not** open a public GitHub issue for vulnerabilities that could:

- expose phone files without intended authorization;
- bypass authentication;
- enable path traversal;
- allow arbitrary file modification outside the shared root;
- execute code on the phone;
- expose signing credentials;
- create unintended remote-network exposure.

Use GitHub's **Private vulnerability reporting** feature if enabled for the repository. If it is not enabled, contact the maintainer through a private channel listed in the repository profile/release metadata.

Include:

- PocketNAS version/commit;
- Android version/device model;
- network assumptions;
- proof-of-concept steps;
- impact;
- logs with sensitive data removed;
- suggested mitigation if available.

## Current security characteristics

PocketNAS v3.1 is intended for trusted LAN use.

- Transport: plain HTTP/WebDAV; no TLS.
- Default authentication: OFF.
- Optional authentication: HTTP Digest, MD5, qop=auth.
- Password: 4–16 numeric digits in the current UI.
- Password storage: app-private config file, mode 0600; no app-level encryption/Keystore wrapping.
- Storage privilege: broad shared-storage access may be granted through Android All files access.
- Auto-start: service may start after boot.

These characteristics are architectural limitations, not security guarantees. Users should avoid public/untrusted Wi-Fi and maintain backups of important data.

## Release signing

A production release signing key is security-critical. Never commit a keystore/private key or password to this repository. Rotate GitHub secrets if compromise is suspected. Because Android application updates require signing continuity, a signing-key compromise may require a package-name migration for safe future releases.
