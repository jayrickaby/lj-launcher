#!/usr/bin/env bash
set -euo pipefail

uv run python -m nuitka src/lj-launcher/main.py \
  --follow-imports \
  --enable-plugin=pyside6 \
  --output-dir=dist \
  --output-filename=lj-launcher \
  --onefile \
  --include-data-dir=src/lj-launcher/qml=qml \
  --include-package-data=minecraft_launcher_lib \
  --noinclude-dlls=*assetdownloader* \
  --noinclude-qt-plugins=multimedia,pdf,designer \
  --include-qt-plugins=qml \
  --noinclude-qt-translations \
  --assume-yes-for-downloads

rm -rf dist/main.build dist/main.dist dist/main.onefile-build