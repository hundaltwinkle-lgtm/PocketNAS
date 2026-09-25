# Release assets

For GitHub, publish APK binaries under **Releases** rather than committing them to the main source history.

Current beta source version: `v2.3.0-beta.1`.

Recommended release assets:

```text
PocketNAS-v2.3-arm64.apk
SHA256SUMS.txt
```

The repository release workflow signs APKs from GitHub Secrets. Keep the signing keystore and passwords out of Git.
