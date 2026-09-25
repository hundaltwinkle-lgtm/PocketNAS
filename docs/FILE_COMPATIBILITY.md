# File type and large-file compatibility

PocketNAS does not intentionally filter files by extension. WebDAV exposes filesystem entries, and unknown file types fall back to:

```text
application/octet-stream
```

PocketNAS v2.3 additionally identifies common MIME types so Windows and applications can handle resources more naturally.

## Built-in MIME mappings

Examples include:

- Documents: TXT, MD, CSV, PDF, DOC/DOCX, XLS/XLSX, PPT/PPTX
- Images: JPEG, PNG, GIF, WEBP, BMP, SVG, HEIC/HEIF
- Video: MP4, MKV, AVI, MOV, WEBM, 3GP
- Audio: MP3, M4A/AAC, WAV, FLAC, OGG/OPUS
- Archives: ZIP, 7Z, RAR, TAR, GZIP
- Android: APK
- Data: JSON, XML, SQLite/DB
- Web: HTML, CSS, JavaScript
- Fonts: TTF, OTF

Everything else remains downloadable as generic binary data.

## Range requests

v2.3 implements single HTTP byte ranges and returns:

```text
HTTP/1.1 206 Partial Content
Accept-Ranges: bytes
Content-Range: bytes START-END/TOTAL
```

This matters for media players, previews, seeking, and clients that do not want to download an entire large file before opening it.

## Windows WebClient size limits

Windows' built-in WebDAV client may impose its own limits independently of PocketNAS. A failure with a large file does not necessarily mean the Android server rejected it. See `docs/TROUBLESHOOTING.md` for Windows-side checks.
