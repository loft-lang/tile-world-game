<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The game's own sheet

The Kenney sheets have no torch that burns out, no tar and no horse.  `tile-world.png` is a
sheet of our own that sits BESIDE them.  It is drawn in the Tiny Dungeon style, so a tile of
ours next to one of Kenney's reads as one set.  It also leaves room for what comes later, such
as more monsters; the horse is its first addition beyond light.  The rules its tiles serve
(torches, tar, sockets, light) are in [DESIGN.md § Torches and light](DESIGN.md#torches-and-light).

![The sheet beside Kenney's Tiny Dungeon, and each new tile beside the Kenney tile it must match](assets/sheets/tile-world-presentation.png)

## The sheet

- **File.**  `assets/sheets/tile-world.png`, CC0 like Kenney's (`tile-world-License.txt`
  beside it).  It is named `world` in `theme.json`, listed last.
- **Grid.**  16 × 16 tiles with no spacing, 12 columns (192 px), the same as
  `tiny-dungeon.png`.  The sheet grows DOWN by whole rows.
- **Indices never move.**  `materials.loft` numbers a tile `1 + slot * 256 + tile`, and levels
  and stamps store that number.  So a tile, once added, keeps its index.  A retired tile stays
  in place (blank), and `world` is listed LAST in `theme.json`'s `sheets` so no existing number
  shifts.  The cap is 256 tiles a sheet (21 rows); a second sheet of ours follows when that fills.
- **Rows by family**, so a new addition has a home without a reshuffle:

| rows | family | now |
|---|---|---|
| 0–1 | fire and light | tiles 0–12 below |
| 2 | travel | 24: the horse |
| 3–4 | tracks | 36–51: junctions; 52, 53: the lever |
| 5 | bombs, the bow | 60–62: the bomb, lying and lit; 63–65: the blast; 66: the bow; 67: an arrow |
| 6 | the mine's work | 72: a loaded cart; 73, 74: a lit tar barrel; 75, 76: a burning barricade; 77: a coiled rope; 78: a door held shut by a beam; 79–82: crystals, large and small, spent and charged; 83: a lightning rod |
| 7 | crafting | 84, 85: the lit forge |
| 8 | the hero | 96: its back |
| 9–10 | water | 108–120: deep water's shore |
| 11– | later additions (monsters, …) | |

### The tiles

| index | tile | used for |
|---|---|---|
| 0 | stick | a plain stick; also what a torch is once it has burnt out |
| 1 | torch | tarred, not burning |
| 2 | burning torch, frame A | held or lying; the flame leans left |
| 3 | burning torch, frame B | the same torch, the flame leaning right: A and B differ only in the flame |
| 4 | burning torch, low | its last minute: a small red flame |
| 5 | tar barrel | turns a stick into a torch |
| 6 | wall socket | an empty iron ring on a wall |
| 7 | socket with a torch | not burning |
| 8, 9 | socket with a burning torch, frames A and B | |
| 10 | socket with a low torch | its last minute |
| 11 | socket with a stick | a torch that burnt out in the socket |
| 12 | socket with the staff | Kenney's staff (dungeon:129) as it is |
| 24 | horse | side view, facing right, saddled |
| 36–51 | junctions | eight junctions — a straight (north-south or east-west) and a bend sharing a side, its toe — each set straight (even index) and set to turn (odd): Kenney's rail ring pieces laid over each other, the route set on top |
| 52, 53 | lever | beside a junction: handle left (set straight), right (set to turn) |
| 60 | bomb | an iron ball, its fuse at the top right: lying, not lit (and in the hand) |
| 61, 62 | bomb, lit, frames A and B | the fuse spitting sparks — yellow (A), redder and turned (B) — while it is thrown and rolling, or set off by a blast; 100 ms a frame |
| 63, 64, 65 | blast: flash, fireball, smoke | the three frames of a blast, spread over its 300 ms flash on the cell it went off in; the flash also lights the room (a light that fades three shades) |
| 66 | bow | drawn shooting up, as everything carried is drawn pointing up: lying across, its curve up, the string below, the grip in the middle (held by its grip, 7, 6) |
| 108–120 | deep water's shore | Tiny Battle's shore (battle 18–20, 36–38, 54–56, 90–93) in `theme.json`'s PIECES order (nw n ne w c e sw s se, the inner corners nw ne sw se), the water a step darker from the palette's own blues (75e3ff → 44c6e4, 9febff → 75e3ff, 46c4e4 → 0099db), the grass and the foam as they are (owner, 2026-11-08: deep water a slightly darker blue; the shallows keep Tiny Battle's own) |
| 96 | the hero's back | Kenney's knight (dungeon:97) seen from behind (owner, 2026-11-07): the visor and face painted over as the back of the helmet, a ridge down its middle, the neck guard in shadow; drawn when the hero faces north |
| 67 | arrow | flying up: an iron head, a wooden shaft, red and white fletching; turned the way it flies |
| 72 | loaded cart | Kenney's cart (town:57) with the rock (farm:89) heaped in it, reduced: a cart full of rubble |
| 73, 74 | lit tar barrel, frames A and B | the tar barrel (5) with the burning torch's flame (2, 3) on its lid |
| 75, 76 | burning barricade, frames A and B | Kenney's boarded planks (dungeon:63) with two of the torch's flames, swapped between the frames |
| 77 | rope | a coil of hemp, two turns, lit from the top left; also what the shoulders carry |
| 78 | door held shut | Kenney's door (dungeon:45) with the log (town:106) laid across it, reduced: a beam barring it |
| 79, 80 | crystal, spent and charged | Kenney's grey stone (dungeon:56); charged, its light cells turned blue-green |
| 81, 82 | small crystal, spent and charged | the same, reduced to ten pixels |
| 83 | lightning rod | Kenney's post (town:47) with an iron spike and a spark on top |
| 84, 85 | lit forge, frames A and B | Kenney's forge face (dungeon:28) with fire behind its grate's bars, hotter at the bottom; the flame's colours swapped between the frames |

## The style, measured from `tiny-dungeon.png`

- **Palette.**  The 27 colours `tiny-dungeon.png` uses, one letter each in
  `assets/art/world/palette.txt`, and the few a tile took from another Kenney pack (the rope's
  Tiny Farm browns, the rod's post) or its own glow (a charged crystal), listed below them;
  `tools/sheet.loft` refuses any other.  A new colour is added there by name, never only in the
  picture.
- **Outline.**  `#3f2631` (`o`), 2 px thick on the outside with rounded corners: every empty
  pixel within dx² + dy² ≤ 5 of the figure (measured on the staff, dungeon:129).  Where two
  things overlap, the line between them is 1 px.  **Iron on a wall is outlined in navy**
  (`n`, `#262b44`), as Kenney's wall fittings are (dungeon:7, 19); wood and cloth keep `o`.
- **Ramps.**  Wood: `b` `m` `t` (`#763b36` `#bd6c4a` `#eaa56c`).  Iron: `n` `s` `g` `l`.
  Flame: `r` rim, `y` body, `Y` heart, one `w` hot pixel.  Tar: `n` with an `s` or `g` gloss.
- **Light comes from the top left**: a highlight edge, a shadow edge, flat fills between.  No
  dithering, no anti-aliasing.
- **Items stand upright and centred**, like the sword (104) and the staff (129).  The flame
  never touches the tile's sides.
- **Sockets are overlays**: transparent except the ring and what it holds, so a socket shows on
  any wall's bricks.

## How the sheet is made

Kenney's tiles are drawn larger first and reduced to their size, so that is how these are made.

1. **Drawn large.**  A tile starts as a scene in `assets/art/world/large/NN-name.draw`, 16 times
   the tile (256 × 256), in the scene language of crawler's drawing set
   ([SPRITES.md](../crawler/SPRITES.md)).  Render one with crawler's `tools/draw.py`
   (`SKETCH_OUT=<dir> python3 ../crawler/tools/draw.py --once <scene>`), or with the loft
   `drawing` library, which renders the same scenes byte for byte.  No outer ink is drawn: the
   reduction adds the outline.
2. **Reduced.**  `loft --lib libs tools/reduce.loft <canvas.png> <tile.txt> [outline letter]`
   turns each 16 × 16 block into one pixel.  A block less than half covered becomes
   transparent; otherwise it takes the palette colour most of its pixels are nearest to.  Then
   the outline goes on.
3. **The hand pass.**  The tile, `assets/art/world/NN-name.txt`, is 16 lines of 16 letters (`.`
   transparent).  The hand pass happens there: a pixel is one character.  Done so far:
   - the flame tips (2, 3);
   - the base of flame B, made the same width as A's so the head stays still while the flame
     flickers (3);
   - the glint on the tar (1);
   - the horse's ear, eye and legs (24).

   Reducing a tile again starts it over and drops its hand pass.
4. **Composed.**  The socket tiles (6–12) are not drawn one by one; `composed.txt` puts each
   together from three parts:
   - the ring's back half (`layers/socket-back.txt`);
   - what the socket holds (a tile of the sheet, or Kenney's staff in `layers/kenney-staff.txt`),
     cut where the ring hides it;
   - the ring's front band (`layers/socket-front.txt`).

   The ring is Kenney's own size (dungeon:7's spout ring, 6 px with a 2 px hole), too small to
   survive a reduction, so its layers are drawn at 16 px.  The bracket and the torch are
   therefore the same pixels in every socket tile.

   The junctions (36–51) are composed the same way from Kenney's rail ring pieces
   (`layers/kenney-rail-*`), with `over`: the top piece's rails and sleepers win, its dark ground
   only where nothing is beneath — so the route a junction is set to is the one drawn on top.
5. **Built.**  `loft --lib libs tools/sheet.loft` writes `tile-world.png`.  With `check` it
   instead exits 1 when the committed sheet differs from the sources (`check`, not `--check`:
   a `--` flag after the script is loft's own).  Every tile has a source: a tile put into the
   picture by hand is lost at the next build.  Run `check` after any change to the sheet.

## Presenting it

`loft --lib libs tools/present.loft` writes `assets/sheets/tile-world-presentation.png` (above),
at 4×:

1. the two sheets side by side;
2. each new tile beside the Kenney tile it must match: the stick and the torches by the staff
   and the sword, the barrel by the chest, the sockets on the wall's bricks beside the banner,
   and the horse by the rat.

The third part is the game itself: `the torch room` with the light on, at its start, carrying
the lit torch, and with it hung in the far socket —
`TW_SHOT=room.png TW_LEVEL=2 [TW_MOVES="s d d d d d w w a a a w d d d"] loft --lib libs main.loft`
(play is always drawn in its light; L switches it only in the editor).

## Animation

The burning tiles come in two frames (A, B) alternating every 250 ms of the level's clock
(`theme::FRAME_MS`): a thing's look in `theme.json` takes `"frames": ["world:2", "world:3"]`,
and a `"low"` picture for a flame's last minute.  A thing that holds another (the socket) has a
`"holding"` look per thing it can hold, drawn as one picture.
