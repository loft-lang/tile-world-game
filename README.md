# tile_world, the game

**Pictures and sounds by [Kenney](https://kenney.nl)**: the Tiny Dungeon, Tiny Town, Tiny Farm,
Tiny Battle and Tiny Ski packs and the Impact, Interface, RPG, Sci-fi and Voiceover sound packs,
all released under CC0.  The torches, the tar barrel, the wall sockets and the horse are the
game's own sheet, drawn in Tiny Dungeon's style and also CC0 (ART.md).  With kind regards to
Kenney for the pictures: the game opens with that credit.

A traversal-puzzle game on the `tile_world` library.  Run it:

```sh
./fetch-libs.sh                        # once: the libraries, into libs/
loft --native --lib libs main.loft
```

The game starts in **the demo village** (`levels/demo.json`): a house for each room that shows
a mechanic, its name written on the ground in front of it.  Walk onto a house's door to go in,
onto the room's door to come back out; every place stays as you left it, and what the hero
holds, its health and its money go with it.  A room that needs a key of its own has it written
on its floor.  A world is the places reached from a start place through doors: `TW_WORLD=<place>`
starts another one (order.json's `start` is the default), and `TW_LEVEL=<n>` plays one level of
`order.json` on its own, as before.

WASD (or the arrows) walk.  Q takes the small item before the hero — the way it last stepped;
walking over one no longer takes it — or puts the held one down there, and held while walking pulls.
The hero shows the way it faces: it leans east or west (mirrored facing west), and facing north it
shows its back (`world:96`, ART.md).  E uses what the hero holds: a swing (held, a heavy
swing), a bow shoots, a bomb is thrown, a potion drunk.  Tab aims.  Shift guards for now (pressed, a
parry; held, a block), until the clash replaces it (DESIGN.md @GM-Combat).
In a browser (`./play-web.sh`), F1 switches to the first-person view and back (RENDER_3D.md § The
first-person view): forward and back walk the way the hero looks, left and right turn.  F3 roams
free (an experiment): the hero moves smoothly, diagonals too, and stands between tiles.
Bats swoop (TEMPLATES.md): a chirp, a zigzag, a dive and a nip on the cell they aimed at, then away.
**The play log**, for finding bugs (`playlog.loft`): every key down and up with the wall time, the
world's clock and how long it was held; every act of every enemy and person at its moment on the
clock; the hero's changes and blows; slow frames.  `./play-web.sh` builds the page and serves it
from one loft server (`tools/playserver.loft`, http://localhost:8080/) that also takes the game's
log over a WebSocket on the same port and writes it to `logs/web-<date>-<time>.log` as you play —
on from the start.  F9 echoes it to the browser's developer console as well (lines starting
`LOG`).  On a desktop `./play-log.sh` writes it to `logs/play-<date>-<time>.log` (`TW_LOG=<file>`;
a desktop window needs X11 — on macOS, XQuartz).  The way there is checked by `checks/playlog.sh`
(the server and the game's log in two processes).  Needs loft's `net` component: `make components`
in the loft checkout.
The hearts at the bottom right are the hero's health: a fall of a layer or more costs some.
Walking into a well or a fountain saves the situation and writes the game to `saves/`: starting
the game again carries on from there (`TW_NEW=1` starts afresh).  Losing all health brings the
hero back there, and B (twice, to confirm) goes back by choice.  R restarts the place as it
was when the hero came in (a level on its own: from its start; N or Enter then goes to the next
one when it is solved), Esc quits.  Walking out through the gap in the village's east edge
ends the game: stills of what follows, chosen by what the hero did (`levels/epilogue.json`).

**F2 opens the editor** (EDITOR.md § 4) on the level being played, and closes it again:

| Key | Does |
|---|---|
| arrows / WASD | move the cursor; the view follows it |
| PgUp / PgDn | the layer up / down: the one below is drawn darker, the one above as outlines |
| Tab / Shift+Tab | the next / previous entry of the palette along the bottom: void, the terrain kinds, the materials (theme.json), the things, the figures (every enemy and person) |
| `1`..`9` | jump to a palette category (theme.json's `categories`: Outdoor, Water, Castle, House, Dungeon, Things) |
| Space | paint the terrain, or put the thing on top of the cell's stack |
| Backspace / Delete | take the top thing off; on a cell with none, unset it (its layer's ground, or void) |
| H, G | the hero starts here; the level is solved here |
| L | the level's light on or off (editing shows it in full light) |
| F | fill: paint the connected cells of the same terrain under the cursor |
| R | mark a corner; R again paints the rectangle from it to the cursor |
| I | pick what is under the cursor into the palette (eyedropper) |
| C, V | copy the marked box (or the cursor's cell), paste it with its corner at the cursor |
| K | the checks: the way from the start to a cell goal (dots; NO PATH! when there is none), chests cut off (red) or hidden (magenta) — hints, since a search ignores pushing |
| Ctrl+Z / Ctrl+Y | undo / redo |
| Ctrl+S | save the level to its file in `levels/` |
| Ctrl+O | open the next level of `levels/order.json` (by name once `input` reads typed text) |
| P or F2 | play-test: the level as edited, from the start, on a copy; F2 comes back to the editor |

| File | What it is |
|---|---|
| `kinds.json` | what each kind of terrain and thing DOES: walk, opaque, height, push/pull, rails, lift, walk-on effects, light — the rules read these, never a name |
| `theme.json` | what each kind LOOKS like and sounds like: the sheets, tile size, font, a sprite per kind by name; the MATERIALS the editor paints with and their autotile sets |
| `materials.loft` | materials: a kind with a look; tile numbers, variants by position, nine-slice autotiling |
| `rules.loft` | the rules over those properties: the hero, push, pull, climb, trains, boost, the staff, combat, enemies and their habits, crafting, followers, tribes |
| `talk.loft` | what people say: lines and barks, the choices while a line is said (stay, bump, walk away), gifts, recipes, and what a line sets, needs or starts (following, staying, guiding) |
| `levels/` | the places, one JSON file each (EDITOR.md § 2) — with their doors to other places and the text on their ground — and `order.json`: the start place and the editor's list; `people.json`, the people who keep to points across places; `epilogue.json`, the stills the ending chooses from |
| `levels.loft` | reads and writes level files; a level's goal decides when it is solved |
| `theme.loft` | reads theme.json and draws a kind, a stack, the hero and text |
| `editor.loft` | the editor's core: cursor, palette, paint, stack, erase, the markers, undo and redo — no picture, no keys |
| `camera.loft` | where the view stands: `follow` and `rooms` while playing, the cursor kept in sight while editing |
| `game.loft` | the game: picture (`tiles`, Kenney's Tiny packs), sounds (`audio`, Kenney's sound packs), controls (`input`) |
| `main.loft` | the loop: shows the picture with `screen`, reads the keys from the window or the machine's keyboard |
| `guide.html` | how to play and edit, for players: `./play-web.sh` serves it beside the game as `guide.html` |
| `render3d.loft` | the first-person view, in a browser page: the place as geometry with the 2D's pictures, cards, the held item (`graphics`) |
| `DESIGN.md` | what the game should become, slice by slice |
| `STORY.md` | the story game, designed before it is built: the owner's story, people, monsters and endings, what the engine offers them, and engine notes kept apart |
| `BUILD.md` | building the story game in small safe steps: tools, mechanics (each in a room of the test world first), the world's systems, then the places |
| `LOCATIONS.md` | each place of the story game: what makes it unique, what is found and learned there, what to look out for; the owner's placements and the proposals (◇) kept apart |
| `TEMPLATES.md` | how each enemy, creature and person acts: its templates (fight, walk, talk, work), their conditions and timings — designed, the owner rules each set |
| `PUZZLES.md` | puzzles made of parts: each a small challenge, interchangeable with a like one, picked by a seed; roles passed between parts, and the check that finds the seeds whose parts together are solvable — designed, the owner rules it |
| `TALKS.md` | what the game's people talk about: what each knows, wants and gets wrong, what the village argues over, what play changes — a writer's sheet from STORY.md |
| `tools/fill.loft`, `tools/map.loft`, `tools/stable.loft` | the drawing kit's tools: `loft --lib libs tools/fill.loft PLACE` fills a place in from its intent (ground, trails, blobs, domes, rooms, corridors, houses, slopes, items: intent.loft) and saves it; `tools/map.loft PLACE OUT.png [SCALE]` pictures the whole place, every layer in full light; `tools/stable.loft PLACE` lists the cells that would not hold (under a ceiling, no wall beside them) |
| `tools/tags.loft` | the story's parts (`@ST-Decree`, STORY.md) and the gameplay mechanics wanted (`@GM-CartLift`, DESIGN.md) as tags, like loft's formal rules: `list`, `check` (the index, the paragraphs and every citation in step; exit 1 when not), `sites <tag>`; a place file names what it serves in its `serves` list |
| `EDITOR.md` | the design of the in-game world editor |
| `RENDER_3D.md` | an experiment: the game in 3D on OpenGL and WebGL — the environment as geometry with the pictures as textures, everything else as cardboard cut-outs; desktop and browser only |
| `tests/` | the rules, played out: `loft --lib libs --tests tests` — `combat.loft` each fighting move timed by hand, `fights.loft` the fight rooms played by an attacker, a parrier and a dodger, `places.loft` the demo village's newer houses (fountain, cave-in, herbs, night, Sable, dome, blasting, creatures, raid) played as a player would, `blasting.loft` the directed blast and the cave-in, `creatures.loft` each new creature's habit, `crafting.loft` the forge, the anvil and the artefact, `talk.loft` lines, barks on conditions, hints and broken words, `following.loft` followers through doors, people at their points and a guide, `tribe.loft` the barricade and the Kharn's raid |
| `checks/` | the game itself (safe points) and the first-person view's faces, as plain programs until `--tests` opens a library's C core (loft#2234): `loft --lib libs checks/game.loft`, `checks/view3d.loft` |
| `assets/` | Kenney's sheets and sounds, CC0 (each pack's `License.txt` beside it), and the game's own sheet `tile-world.png` (CC0, in Tiny Dungeon's style) |
| `ART.md` | the game's own sheet: its layout, its style, how a tile is drawn |
| `INTERACTIONS.md` | where the rules meet: what needs a ruling, what could be unfair, and the puzzles it opens |
| `assets/art/world/` | the own sheet's sources: a tile as 16 lines of letters, its larger drawing in `large/` |
| `tools/` | `sheet.loft` builds `tile-world.png` from those sources (`check`: does the committed sheet match them?), `reduce.loft` turns a larger drawing into a tile, `present.loft` draws the sheet beside Kenney's |
| `fetch-libs.sh` | fetches the libraries into `libs/` from their repositories, until they are all published |

`TW_SHOT=frame.png [TW_LEVEL=1] [TW_MOVES="s d d"] [TW_EDIT="d d tab paint"] [TW_KEYS="101 9 32"] loft --native --lib libs main.loft`
writes a picture of the game after those moves, with no screen.  `TW_EDIT` then opens the
editor and does those edits (the words of `game::edit`); `TW_KEYS` presses keys by code, one
tick each, `+` for keys held together (`133+122` is Ctrl+Z), read exactly as a keyboard's.
