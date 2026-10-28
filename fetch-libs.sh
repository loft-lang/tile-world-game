#!/bin/sh
# Fetch the libraries the game uses into libs/, each from its repository at the ref named here.
# Until they are all published to the loft registry, this is how the game gets them; then the
# list below becomes the game's loft.toml dependencies.
#
#   ./fetch-libs.sh          # fetch every library (replaces libs/)
set -eu
cd "$(dirname "$0")"

# package          repository           ref
LIBS='
tile_world         loft-libs-world      main
audio              loft-libs-game       main
input              loft-libs-game       main
screen             loft-libs-game       main
tiles              loft-libs-graphics   tuxedo-tiles-shade
imaging            loft-libs-graphics   tuxedo-imaging-c
graphics           loft-libs-graphics   main
mesh3d             loft-libs-assets     main
glb                loft-libs-assets     main
'

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
rm -rf libs
mkdir -p libs
echo "$LIBS" | while read -r pkg repo ref; do
  [ -n "$pkg" ] || continue
  mkdir -p "$tmp/$repo-$ref"
  if [ ! -f "$tmp/$repo-$ref/.done" ]; then
    curl -sfL "https://codeload.github.com/loft-lang/$repo/tar.gz/$ref" | tar -xz -C "$tmp/$repo-$ref" --strip-components=1
    touch "$tmp/$repo-$ref/.done"
  fi
  cp -r "$tmp/$repo-$ref/$pkg" "libs/$pkg"
  echo "libs/$pkg  <- loft-lang/$repo@$ref"
done
