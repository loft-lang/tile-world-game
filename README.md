# tile_world, the game

**Pictures and sounds by [Kenney](https://kenney.nl)**: the Tiny Dungeon, Tiny Town, Tiny Farm,
Tiny Battle and Tiny Ski packs and the Impact, Interface, RPG, Sci-fi and Voiceover sound packs,
all released under CC0.  With kind regards to Kenney for the pictures: the game opens with
that credit.

A traversal-puzzle game on the `tile_world` library.  Run it:

```sh
./fetch-libs.sh                        # once: the libraries, into libs/
loft --native --lib libs main.loft
```

Arrows or WASD move.  Hold Shift (or P) while moving to pull.  R restarts the level, N (or
Enter) goes to the next one when this one is solved, Esc or Q quits.

| File | What it is |
|---|---|
| `kinds.json` | what each kind of terrain and thing DOES: walk, opaque, height, push/pull, rails, lift, walk-on effects, light — the rules read these, never a name |
| `theme.json` | what each kind LOOKS like and sounds like: the sheets, tile size, font, a sprite per kind by name |
| `rules.loft` | the rules over those properties: the hero, push, pull, climb, trains, boost, the staff |
| `levels/` | the levels, one JSON file each (EDITOR.md § 2), and `order.json`, the order they are played in |
| `levels.loft` | reads and writes level files; a level's goal decides when it is solved |
| `theme.loft` | reads theme.json and draws a kind, a stack, the hero and text |
| `game.loft` | the game: picture (`tiles`, Kenney's Tiny packs), sounds (`audio`, Kenney's sound packs), controls (`input`) |
| `main.loft` | the loop: shows the picture with `screen`, reads the keys from the window or the machine's keyboard |
| `DESIGN.md` | what the game should become, slice by slice |
| `EDITOR.md` | the design of the in-game world editor |
| `tests/` | the rules, played out: `loft --lib libs --tests tests` |
| `assets/` | Kenney's sheets and sounds, CC0 (each pack's `License.txt` beside it) |
| `fetch-libs.sh` | fetches the libraries into `libs/` from their repositories, until they are all published |

`TW_SHOT=frame.png [TW_LEVEL=1] [TW_MOVES="s d d"] loft --native --lib libs main.loft`
writes a picture of the game after those moves, with no screen.
