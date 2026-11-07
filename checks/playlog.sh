#!/bin/sh
# The play log over a WebSocket, end to end: the play server (tools/playserver.loft) on a port of
# its own writing to a temporary file, the game's play log (playlog.loft, through
# checks/playlog_client.loft) sending three lines to it; they must be in the file, in order, after
# the note of the game connecting.  Needs loft's `net` component (`make components` in loft).
#   checks/playlog.sh             exit 0 when they arrived, 1 when not
set -eu
cd "$(dirname "$0")/.."
LOFT=${LOFT:-$HOME/workspace/loft/target/release/loft}
PORT=${TW_PORT:-18780}
log=$(mktemp -t playlog.XXXXXX)
out=$(mktemp -t playserver.XXXXXX)
(cd tools && TW_PORT=$PORT TW_LOG="$log" exec "$LOFT" --lib ../libs playserver.loft >"$out" 2>&1) &
server=$!
stop() {
  for p in $(lsof -nP -t -iTCP:$PORT -sTCP:LISTEN 2>/dev/null); do kill $p 2>/dev/null; done
  kill $server 2>/dev/null || true
  wait $server 2>/dev/null || true
  rm -f "$log" "$out"
}
trap stop EXIT
n=0
until lsof -nP -iTCP:$PORT -sTCP:LISTEN >/dev/null 2>&1; do
  n=$((n + 1))
  if [ $n -gt 240 ]; then echo "playlog: the play server did not start"; cat "$out"; exit 1; fi
  sleep 0.5
done
TW_PORT=$PORT "$LOFT" --lib libs checks/playlog_client.loft 2>&1 | grep 'playlog client'
sleep 1
got=$(grep -v '^# tile_world' "$log")
want_order='a game connected
clock 300  bat#2 moved (3,6,0)->(2,6,0)  next 420
clock 400  key E (101) down
clock 450  key E (101) up, held'
i=0
fail=0
while IFS= read -r line; do
  i=$((i + 1))
  have=$(echo "$got" | sed -n "${i}p")
  case "$have" in
    *"$line"*) ;;
    *) echo "playlog: line $i should hold '$line', is '$have'"; fail=1 ;;
  esac
done <<WANT
$want_order
WANT
if [ $fail -ne 0 ]; then echo "$got"; exit 1; fi
echo "checks/playlog.sh: the lines arrived in order"
