#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="$ROOT/build"
APKROOT="$BUILD/apkroot"
mkdir -p "$APKROOT/lib/arm64-v8a" "$ROOT/dist"

: "${ANDROID_NDK_HOME:?Set ANDROID_NDK_HOME to an installed Android NDK}"
PREBUILT="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64"
export PATH="$PREBUILT/bin:$PATH"
CC="$PREBUILT/bin/aarch64-linux-android28-clang"
if [[ ! -x "$CC" ]]; then echo "Compiler not found: $CC" >&2; exit 1; fi

"$CC" -std=c11 -O2 -fPIC -shared -mno-outline-atomics \
  -fvisibility=hidden -ffunction-sections -fdata-sections \
  -Wl,--gc-sections -Wl,-z,relro,-z,now -Wl,-soname,libpocketnas.so \
  "$ROOT/pocketnas.c" -landroid -o "$APKROOT/lib/arm64-v8a/libpocketnas.so"

POCKETNAS_MANIFEST_OUT="$APKROOT/AndroidManifest.xml" python3 "$ROOT/tools/make_manifest.py"
POCKETNAS_DEX_OUT="$APKROOT/classes.dex" python3 "$ROOT/tools/make_dex.py"
python3 "$ROOT/tools/package_apk.py" --root "$APKROOT" --out "$BUILD/PocketNAS-v3.0-unsigned.apk"
python3 "$ROOT/tools/verify_release.py" "$BUILD/PocketNAS-v3.0-unsigned.apk"

echo
echo "Unsigned APK: $BUILD/PocketNAS-v3.0-unsigned.apk"
echo "Sign it with Android build-tools apksigner. See docs/BUILDING.md."
