# Contributing to PocketNAS

Thank you for contributing.

## Before starting

For substantial changes, open an issue first describing:

- the problem;
- proposed behavior;
- affected Android/Windows versions;
- security or compatibility implications.

## Development principles

1. Preserve Windows File Explorer compatibility.
2. Do not weaken path validation or expand network exposure silently.
3. Keep release signing material out of the repository.
4. Prefer testable, deterministic behavior over hidden magic.
5. Document new permissions and security implications.
6. Keep changes narrowly scoped where possible.

## Pull requests

A PR should include:

- clear description;
- reason for the change;
- test steps;
- affected files/protocol methods;
- device/OS used for testing;
- screenshots/logs when UI/lifecycle-related;
- documentation updates.

## C code

The native core is intentionally low-level and manually declares the platform APIs it uses. Changes should compile cleanly for `aarch64-linux-android28` and must retain `-mno-outline-atomics` unless compatibility testing justifies a change.

Avoid unbounded copies, unchecked path concatenation and unsafe handling of untrusted HTTP headers.

## DEX generator

`tools/make_dex.py` directly emits DEX bytecode. Changes require careful verifier testing on a real Android device. If you modify an `invoke-*` encoder, validate the argument/register encoding and run `adb logcat -b crash -d` after installation.

Long term, replacing this generated layer with ordinary Android Java/Kotlin sources is welcome if it preserves the lightweight runtime goals.

## Testing

At minimum:

```powershell
curl.exe -v -X OPTIONS "http://PHONE_IP:8080/"
curl.exe -v -X PROPFIND -H "Depth: 0" "http://PHONE_IP:8080/"
```

Also test File Explorer read/write operations on expendable files and check the Android crash buffer.

## Commit hygiene

Do not commit:

- APK signing keys;
- keystores;
- passwords;
- personal phone logs without sanitization;
- build output;
- device identifiers.

## License

By submitting a contribution, you agree that it may be distributed under the repository's Apache License 2.0.
