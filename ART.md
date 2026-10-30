<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The game's own sheet

The Kenney sheets have no torch to burn out, no tar and no horse.  This file designs a sheet
of our own that sits BESIDE them.  It is drawn in the Tiny Dungeon style, so a tile of ours
next to a tile of Kenney's reads as one set.  It is a design: nothing here is drawn yet.  The
rules the tiles serve (torches, tar, light) are in [DESIGN.md § Torches and
light](DESIGN.md#torches-and-light).

## The sheet

- **File.**  `assets/sheets/tile-world.png` (with `tile-world-License.txt` beside it), named
  `world` in `theme.json`.
- **Grid.**  16 × 16 tiles with no spacing, 12 columns (192 px), the same as
  `tiny-dungeon.png`.  The sheet grows DOWN by whole rows.
- **Indices never move.**  `materials.loft` numbers a tile `1 + slot * 256 + tile`, and levels
  and stamps store that number.  So a tile, once added, keeps its index.  A retired tile stays
  in place (blank), and `world` is listed LAST in `theme.json`'s `sheets` so no existing number
  shifts.  256 tiles a sheet is the cap, 21 rows; a second sheet of ours follows when that fills.
- **Rows by family**, so a new addition has a home without a reshuffle:

| rows | family | now |
|---|---|---|
| 0 | fire and light | the twelve tiles below |
| 1 | travel | reserved: the horse (and its saddle, a mounted hero) |
| 2–4 | monsters | reserved: frames of the rats, bats and spiders the Kenney sheet lacks |
| 5– | later additions | |

### Row 0: fire and light

| index | tile | used for |
|---|---|---|
| 0 | stick | a plain stick; also what a torch is once it has burnt out |
| 1 | torch | tarred, not burning |
| 2 | burning torch, frame A | held, or lying on the floor |
| 3 | burning torch, frame B | the flicker (see Animation) |
| 4 | burning torch, low | its last minute: a smaller flame, the warning it is about to go out |
| 5 | tar barrel | turns a stick into a torch |
| 6 | wall socket | an empty iron bracket on a wall |
| 7 | socket with a torch | holding a torch that does not burn |
| 8 | socket with a burning torch, frame A | |
| 9 | socket with a burning torch, frame B | |
| 10 | socket with a low torch | its last minute |
| 11 | socket with a stick | a torch that burnt out in the socket |

The five tiles asked for are 1, 2, 6, 7 and 8.  The rest follow from the rules: tile 0 is the
stick tar turns into a torch, and tile 5 is the tar barrel.  Tiles 3, 4, 9, 10 and 11 cover the
flicker and burning out, and each can be dropped (see the open questions).

## The style, measured from `tiny-dungeon.png`

- **Palette.**  Only the 27 colours `tiny-dungeon.png` already uses; no new colour.  The tool
  that builds the sheet refuses any other.  The ramps a tile here needs:
  - outline: `#3f2631`, one pixel, around every object (never black);
  - wood: `#763b36` shadow, `#bd6c4a` body, `#eaa56c` light (the staff and the chests);
  - iron: `#262b44`, `#52607c`, `#8b9bb4`, `#c0cbdc` (the sword, the anvil, the walls);
  - flame: `#e84537` rim, `#feae34` body, `#fee761` heart, `#ffffff` a single hot pixel (all in
    the banner and potions already);
  - tar: `#262b44` with a `#52607c` gloss, on `#3f2631`.
- **Light comes from the top left**: one highlight edge, one shadow edge, flat fills between.
  No dithering, no anti-aliasing, and no gradient longer than the ramp.
- **Items stand upright and centred**, like the sword (104) and the staff (129): a held or
  dropped torch is a vertical stick 2–3 px wide with its head in the top third.  The flame
  rises out of the tile's top rows but never touches the tile edge.
- **Sockets are overlays.**  Tiles 6–11 are transparent except for the bracket and what it
  holds.  The game draws a thing over its cell's terrain, so a socket on a wall cell shows on
  the wall's own bricks (`dungeon:40`), and on any other wall material too.  The bracket sits
  low on the face, about rows 8–12, so a burning torch's flame stays inside the tile.
- **Things on the floor stand on the bottom rows**, like the chest (89): the tar barrel's
  base is on row 14–15 and it is about as wide as the chest.

## Animation

Tiny Dungeon has no animation, and the game draws one picture per thing today.  A flame that
does not move reads as a painted flame.  The design therefore gives the burning tiles two
frames (A and B, alternating every 250 ms of the level clock), which needs `theme.json` to
accept `"frames": ["world:2", "world:3"]` for a thing.  Without that, frame A alone is drawn
and nothing else changes.

## How the sheet is made

- **Source as text.**  Each tile is a file `assets/art/world/<index>-<name>.txt`: sixteen lines
  of sixteen characters, one letter per palette colour and `.` for transparent.  The letter
  key is the first lines of `assets/art/world/palette.txt`.  A tile is then reviewable in a
  diff, and redrawing one pixel is a one-character change.
- **Built by loft.**  `tools/sheet.loft` reads the tile files and writes `tile-world.png`
  through `imaging`.  It refuses a tile that is not 16 × 16, a colour outside the palette and
  an index used twice.  The PNG is committed, and a game check rebuilds it and compares, so the
  source and the picture cannot drift apart.
- **Drawn in passes** (the `draw` method): block in the silhouette and outline, render at 4×
  beside the Kenney neighbours (staff 129, sword 104, chest 89, the wall 40), and compare
  against the style rules above.  Then refine and render again, until the tile sits in the set
  without standing out.

## Presenting it

`tools/present.loft` writes one picture, `assets/sheets/tile-world-presentation.png`, at 4×:

1. **The sheets side by side**: `tiny-dungeon.png`, then `tile-world.png`, on the same scale
   and background, each titled.
2. **Neighbours.**  Each new tile beside the Kenney tile it must match: the torch by the staff
   and the sword, the barrel by the chest, the sockets on the wall bricks.
3. **In the game**: `the_cellar` dark, lit only by socket torches (a `TW_SHOT` picture), then
   the same with the hero carrying one.

The picture is what gets shown when the sheet is finished.  An artifact page with the same
three parts can carry it further.

## Open questions

- **The licence of the sheet.**  CC0 like Kenney's, so the whole art set is one licence, or the
  repository's LGPL-3.0?  The design assumes CC0.
- **The extra tiles.**  Keep the low-flame warning (4, 10)?  Keep the flicker frames (3, 9),
  which need the renderer change above?  Keep the socket with a stick (11), or should a torch
  that burns out in a socket fall to the floor below it?
- **The sheet's name.**  `world` / `tile-world.png`, or another name?
