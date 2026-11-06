#!/bin/sh
# Build the game as a web page and serve it to this machine's network, so a browser on any
# machine there can play it:
#
#   ./play-web.sh [port]          # default 8080
#   LOFT=/path/to/loft ./play-web.sh
#
# Needs a loft whose page build packs the files the game names (loft after 2026.11.0), libs/
# from ./fetch-libs.sh (screen with its browser section), and python3 to serve.  Ctrl-C stops
# serving; the page stays in build/web/index.html, one file that opens from disk too.
set -eu
cd "$(dirname "$0")"
LOFT=${LOFT:-loft}
port=${1:-8080}
out=build/web

if ! grep -q 'LOFT_BROWSER' libs/screen/c/screen.c 2>/dev/null; then
  echo "libs/screen has no browser section: run ./fetch-libs.sh first" >&2
  exit 1
fi
mkdir -p "$out"
"$LOFT" --native-c --html "$out/index.html" --lib libs main.loft
cp guide.html "$out/guide.html"
if ! grep -q 'theme.json' "$out/index.html"; then
  echo "the page carries none of the game's files: $LOFT is too old (set LOFT to a newer loft)" >&2
  exit 1
fi

echo "Open on any machine on this network (the guide: guide.html beside it):"
for ip in $(hostname -I); do
  case "$ip" in
    *:*) echo "  http://[$ip]:$port/" ;;
    *)   echo "  http://$ip:$port/" ;;
  esac
done
echo "  http://$(hostname).local:$port/   (where the network resolves .local names)"
exec python3 -m http.server "$port" --bind :: --directory "$out"
