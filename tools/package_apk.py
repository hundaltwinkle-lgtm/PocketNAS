#!/usr/bin/env python3
from pathlib import Path
import argparse, zipfile

p=argparse.ArgumentParser(description='Package PocketNAS manifest, classes.dex and ARM64 native library into an unsigned APK.')
p.add_argument('--root', default='build/apkroot')
p.add_argument('--out', default='build/PocketNAS-v3.1-unsigned.apk')
a=p.parse_args()
root=Path(a.root)
out=Path(a.out); out.parent.mkdir(parents=True,exist_ok=True)
required=[root/'AndroidManifest.xml',root/'classes.dex',root/'lib/arm64-v8a/libpocketnas.so']
missing=[str(x) for x in required if not x.exists()]
if missing: raise SystemExit('Missing build inputs: '+', '.join(missing))
with zipfile.ZipFile(out,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
    z.write(required[0],'AndroidManifest.xml')
    z.write(required[1],'classes.dex')
    z.write(required[2],'lib/arm64-v8a/libpocketnas.so')
print(out)
