#!/bin/sh
# Play the game on this machine with the play log on: every key down and up (its name, the wall
# time, the world's clock, how long it was held), every act of every enemy and person at its
# moment on the clock, the hero's changes and blows, and slow frames — written as it happens to
# logs/play-<date>-<time>.log (readable while playing; complete when the game stops).
#
#   ./play-log.sh                     # the world, from the save (TW_NEW=1 to start afresh)
#   TW_WORLD=the_bat_cave ./play-log.sh
#   LOFT=/path/to/loft ./play-log.sh
#
# A desktop window needs X11 (on macOS: XQuartz).  Without one, play in a browser and press F9:
# the same log goes to the developer console.
set -eu
cd "$(dirname "$0")"
LOFT=${LOFT:-$HOME/workspace/loft/target/release/loft}
mkdir -p logs
log="logs/play-$(date +%Y%m%d-%H%M%S).log"
echo "play log: $log"
TW_LOG="$log" "$LOFT" --native --lib libs main.loft
echo "play log written: $log"
