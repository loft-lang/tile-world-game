#!/bin/sh
# Build the game as a web page and play it in a browser on this machine, with its play log:
#
#   ./play-web.sh                 # build, then serve on http://localhost:8080/
#   LOFT=/path/to/loft ./play-web.sh
#
# One loft server (tools/playserver.loft) serves the page and its guide, never cached, and takes the
# game's play log over a WebSocket on the same port: every key with its timing and every act of every
# enemy and person, written to logs/web-<date>-<time>.log as it happens.  Ctrl-C stops it; the page
# stays in build/web/index.html.
# Needs a loft whose page build packs the files the game names (loft after 2026.11.0), loft's `net`
# component (`make components` in loft), libs/ from ./fetch-libs.sh, and wasm-opt (binaryen).
set -eu
cd "$(dirname "$0")"
LOFT=${LOFT:-$HOME/workspace/loft/target/release/loft}
out=build/web

if ! grep -q 'LOFT_BROWSER' libs/screen/c/screen.c 2>/dev/null; then
  echo "libs/screen has no browser section: run ./fetch-libs.sh first" >&2
  exit 1
fi
mkdir -p "$out" logs
"$LOFT" --native-c --html "$out/index.html" --lib libs main.loft
cp guide.html hexhouses.html "$out/"
if ! grep -q 'theme.json' "$out/index.html"; then
  echo "the page carries none of the game's files: $LOFT is too old (set LOFT to a newer loft)" >&2
  exit 1
fi
log="$(pwd)/logs/web-$(date +%Y%m%d-%H%M%S).log"
cd tools
TW_LOG="$log" exec "$LOFT" --lib ../libs playserver.loft
