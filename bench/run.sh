#!/bin/sh
# Build bench/actors.c for this machine and for 32-bit RISC-V (the Tanmatsu's instruction set)
# and run both, the second under qemu-riscv32.  Needs cc, zig and qemu-user.
#
#   bench/run.sh            # both
#   bench/run.sh rv32       # the emulator only
set -eu
cd "$(dirname "$0")/.."
out=build/bench
mkdir -p "$out"
if [ "${1:-}" != rv32 ]; then
  cc -O2 -Wall -Wextra bench/actors.c -o "$out/actors_host"
  echo "== this machine"
  "$out/actors_host"
fi
# musl's riscv32 build needs the D extension (its setjmp saves double registers); the device has
# only F.  The benchmark's own work is integer.
zig=$(command -v zig || echo /snap/bin/zig)
"$zig" cc -O2 -target riscv32-linux-musl -mcpu=generic_rv32+m+a+f+d+c bench/actors.c -o "$out/actors_rv32"
echo "== qemu-riscv32"
qemu-riscv32 "$out/actors_rv32"
