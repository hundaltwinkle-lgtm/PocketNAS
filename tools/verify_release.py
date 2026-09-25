#!/usr/bin/env python3
from pathlib import Path
import argparse, hashlib, subprocess, shutil, zipfile, tempfile
p=argparse.ArgumentParser()
p.add_argument('apk')
a=p.parse_args(); apk=Path(a.apk)
if not apk.exists(): raise SystemExit(f'Not found: {apk}')
with zipfile.ZipFile(apk) as z:
    names=set(z.namelist())
    req={'AndroidManifest.xml','classes.dex','lib/arm64-v8a/libpocketnas.so'}
    if not req.issubset(names): raise SystemExit('APK missing required entries: '+str(req-names))
    bad=z.testzip()
    if bad: raise SystemExit('ZIP CRC failure: '+bad)
    so=z.read('lib/arm64-v8a/libpocketnas.so')
print('ZIP structure: OK')
print('SHA256:',hashlib.sha256(apk.read_bytes()).hexdigest())
readelf=shutil.which('llvm-readelf') or shutil.which('readelf')
if readelf:
    with tempfile.NamedTemporaryFile(suffix='.so') as f:
        f.write(so); f.flush()
        s=subprocess.run([readelf,'-Ws',f.name],text=True,capture_output=True,check=True).stdout
        unresolved=[ln for ln in s.splitlines() if ' UND ' in ln and '__aarch64_' in ln]
        if unresolved:
            print('\n'.join(unresolved)); raise SystemExit('ERROR: unresolved __aarch64_* helper(s) detected')
        print('AArch64 outlined-atomic helper check: OK')
else:
    print('readelf not found; skipped ELF symbol verification')
