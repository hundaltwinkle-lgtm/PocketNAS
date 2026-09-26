$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Build = Join-Path $Root "build"
$ApkRoot = Join-Path $Build "apkroot"
$LibDir = Join-Path $ApkRoot "lib\arm64-v8a"
New-Item -ItemType Directory -Force -Path $LibDir | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $Root "dist") | Out-Null

if (-not $env:ANDROID_NDK_HOME) { throw "Set ANDROID_NDK_HOME to your Android NDK directory." }
$NdkBin = Join-Path $env:ANDROID_NDK_HOME "toolchains\llvm\prebuilt\windows-x86_64\bin"
$env:Path = "$NdkBin;$env:Path"
$CC = Join-Path $env:ANDROID_NDK_HOME "toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android28-clang.cmd"
if (-not (Test-Path $CC)) { throw "Android NDK compiler not found: $CC" }

& $CC -std=c11 -O2 -fPIC -shared -mno-outline-atomics `
  -fvisibility=hidden -ffunction-sections -fdata-sections `
  -Wl,--gc-sections -Wl,-z,relro,-z,now -Wl,-soname,libpocketnas.so `
  (Join-Path $Root "pocketnas.c") -landroid -o (Join-Path $LibDir "libpocketnas.so")

$env:POCKETNAS_MANIFEST_OUT = Join-Path $ApkRoot "AndroidManifest.xml"
$env:POCKETNAS_DEX_OUT = Join-Path $ApkRoot "classes.dex"
python (Join-Path $Root "tools\make_manifest.py")
python (Join-Path $Root "tools\make_dex.py")
python (Join-Path $Root "tools\package_apk.py") --root $ApkRoot --out (Join-Path $Build "PocketNAS-v3.1-unsigned.apk")
python (Join-Path $Root "tools\verify_release.py") (Join-Path $Build "PocketNAS-v3.1-unsigned.apk")
Write-Host "`nUnsigned APK: $Build\PocketNAS-v3.1-unsigned.apk"
Write-Host "Sign it using apksigner. See docs\BUILDING.md."
