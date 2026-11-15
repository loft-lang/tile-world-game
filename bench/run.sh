#!/bin/sh
# Build the benches for this machine and for 32-bit RISC-V (the Tanmatsu's instruction set) and run
# both, the second under qemu-riscv32.  Needs cc, zig and qemu-user.
#
#   bench/run.sh                 # bench/actors.c, both
#   bench/run.sh rv32            # bench/actors.c, the emulator only
#   bench/run.sh frame3d [PLACE]  # bench/frame3d.c: raster3d's frames over a scripted walk
#   bench/run.sh fixedcam [PLACE [X Y Z]]
#                                # bench/fixedcam.c on PLACE's landscape (demo when left out),
#                                # written first by tools/scene3d.loft; pictures in build/bench/
set -eu
cd "$(dirname "$0")/.."
out=build/bench
mkdir -p "$out"
zig=$(command -v zig || echo /snap/bin/zig)
# musl's riscv32 build needs the D extension (its setjmp saves double registers); the device has
# only F.  The benchmarks' own work is integer, but for fixedcam's perspective leg.
rv32() { "$zig" cc -O2 -target riscv32-linux-musl -mcpu=generic_rv32+m+a+f+d+c "$@"; }

if [ "${1:-}" = frame3d ]; then
  place=${2:-demo}
  scene="$out/$place.scene"
  if [ ! -f "$scene" ]; then
    loft --interpret --lib libs tools/scene3d.loft "$place" "$PWD/$scene" 2>&1 | grep -v 'Advice\|Warning' | tail -1
  fi
  cc -O2 -Wall -Wextra bench/frame3d.c local/raster3d/c/raster3d.c -o "$out/frame3d_host" -lm
  echo "== this machine"
  "$out/frame3d_host" "$scene"
  rv32 bench/frame3d.c local/raster3d/c/raster3d.c -o "$out/frame3d_rv32" -lm
  echo "== qemu-riscv32"
  qemu-riscv32 "$out/frame3d_rv32" "$scene"
  exit 0
fi

if [ "${1:-}" = fixedcam ]; then
  place=${2:-demo}
  scene="$out/$place.scene"
  if [ ! -f "$scene" ] || [ "$#" -ge 5 ]; then
    shift 2 2>/dev/null || shift $#
    loft --interpret --lib libs tools/scene3d.loft "$place" "$PWD/$scene" "$@" 2>&1 | grep -v 'Advice\|Warning' | tail -1
  fi
  cc -O2 -Wall -Wextra bench/fixedcam.c -o "$out/fixedcam_host" -lm
  echo "== this machine"
  "$out/fixedcam_host" "$scene" "$out"
  rv32 bench/fixedcam.c -o "$out/fixedcam_rv32" -lm
  echo "== qemu-riscv32"
  qemu-riscv32 "$out/fixedcam_rv32" "$scene"
  exit 0
fi

if [ "${1:-}" != rv32 ]; then
  cc -O2 -Wall -Wextra bench/actors.c -o "$out/actors_host"
  echo "== this machine"
  "$out/actors_host"
fi
rv32 bench/actors.c -o "$out/actors_rv32"
echo "== qemu-riscv32"
qemu-riscv32 "$out/actors_rv32"
