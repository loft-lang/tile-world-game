# The world editor — design

An editor inside the game, on every platform it runs on (a desktop window, a browser, a
Tanmatsu).  It builds every kind of place: the outer world (grass, paths, trees, fences),
water (rivers, lakes, shores), castles (walls you can walk on, gates, towers), houses (walls,
doors, roofs, rooms) and dungeons (cellars and caves on the layers below).  It is the game's,
not a library's: the libraries gain only the generic pieces it needs (§ 7).

**Scope.** Levels and regions of tens of chunks: a dungeon, a castle, a village with its
fields.  A huge open world, with streaming, binary chunk files and incremental saves, is outside
this plan.

## 1. The model it edits

Three files describe a place, and the editor writes only the third:

| File | Holds | Who edits it |
|---|---|---|
| `kinds.json` | how each kind of terrain and thing BEHAVES (walk, opaque, height, push, ...) | the game's designer, by hand |
| `theme.json` | how each kind LOOKS, and the MATERIALS the editor paints with | the game's designer, by hand |
| `levels/<name>.json` | one place: its cells, its things, its markers | the editor |

**Kind = behaviour, material = look.**  `tile_world` stores a cell as a kind (4 bits) and a
tile number (12 bits).  So a large outdoor world needs few kinds and many materials:

- grass, sand, a path and a castle floor are all the kind `floor`, with different tiles;
- cave rock, castle stone and a house wall are all `wall`.

A material in `theme.json` is one paintable entry, written as a kind plus a look:

```json
"materials": {
  "grass":        { "kind": "floor", "tile": "town:0", "variants": ["town:1", "town:2"] },
  "path":         { "kind": "floor", "autotile": "path" },
  "water":        { "kind": "water", "autotile": "shore" },
  "castle_wall":  { "kind": "wall",  "autotile": "castle" },
  "house_wall":   { "kind": "wall",  "tile": "town:72" },
  "roof":         { "kind": "floor", "tile": "town:52", "layer": "above" },
  "cave_rock":    { "kind": "wall",  "tile": "dungeon:40" }
}
```

**Autotiling.**  Shores, paths, castle walls and fences have edge and corner pieces in the
Kenney sheets.  An `autotile` entry maps a neighbour mask (which of the 8 neighbours hold the
same material) to a tile.  The editor works out the tile WHEN IT PAINTS, and repaints the
neighbours too.  The cell then simply holds a tile number, so drawing stays a lookup, which is
cheap on the Tanmatsu.

## 2. The level file

A level holds one place: the cells of each layer, the things, and the markers.

- **Chunked**, so a big outdoor world is not one huge grid.  A chunk is `tile_world`'s
  32 × 32, and the file holds only the chunks that exist.
- **Readable**: each row of a chunk is a string, one character per cell.  The `legend` maps a
  character to a material.  Tile overrides (a cell whose tile is not its material's) are rare
  and listed apart.

```json
{
  "name": "the first climb",
  "legend": { "#": "cave_rock", ".": "stone_floor", "~": "water", "=": "rail" },
  "chunks": [
    { "cx": 0, "cy": 0, "z": 0, "rows": ["#############", "#.....~.....#", "..."] },
    { "cx": 0, "cy": 0, "z": 1, "rows": ["        ####", "        ...."] }
  ],
  "things": [ { "at": [9, 2, 0], "stack": ["table", "chair"] } ],
  "start": [1, 1, 0],
  "goal": { "type": "layer", "z": 1 },
  "signs": [ { "at": [4, 7, 0], "text": "the ledge is north" } ]
}
```

**Also in the file** (built in `levels.loft`):
- `camera`: `"follow"` or `"rooms"` (§ 4).
- `ground`: a layer's open land, one entry per layer.  A space in a row is unset and reads as
  that ground; `_` is a pit, void set on purpose.
- `edges`: `[x, y, z, side, kind]`, each cell's N and W edge.
- `rises`: `[x, y, z, steps]`, a cell's own height.

**Goals are data:**

| `goal` | Solved when |
|---|---|
| `{"type": "layer", "z": 1}` | the hero reaches layer 1 (the first climb) |
| `{"type": "daylight"}` | the hero stands under the open sky (the cellar) |
| `{"type": "cell", "at": [x, y, z]}` | the hero reaches a cell |
| `{"type": "chests"}` | every chest of the region is open (DESIGN.md's point goal) |

The two built-in levels become the first two files.  `rules.loft` keeps the code that reads
levels, never any one level.

## 3. Building the five kinds of place

Each kind of place needs particular materials, kinds and rules:

| Place | Materials | Kinds and rules it needs |
|---|---|---|
| **Outer world** | grass, path (autotile), sand, trees, bushes, flowers, fence | `tree` (blocks; light passes); `fence` (climbed over, nothing pushed through — DESIGN.md) |
| **Water** | water with shores (autotile), shallow ford, bridge | `water` blocks walking; a bridge is floor over water on the same layer |
| **Castle** | castle wall (autotile), gate, battlement floor on the layer above, tower stairs | `door` (open/closed), `stairs` (walking onto it moves a layer up or down) |
| **House** | house wall (cells, or edges for thin inner walls), door edges, roof (on the layer above), interior floor, furniture | edges: `tile_world` keeps a wall, door, window or fence BETWEEN two cells; the roof is cut away while the hero is inside (DESIGN.md: what the player sees) |
| **Hills** | grass with a rise per cell (`set_rise`), cliffs as wall cells | a slope is cells rising one step at a time; with 4-step layers (DESIGN.md), the top of a slope steps onto the layer above |
| **Dungeon** | cave rock, stone floor, rails, torches (the staff) | layers below 0; a ceiling = the layer above is set; `stairs` down from the world above |

The new kinds (`tree`, `fence`, `door`, `stairs`) are a line of `kinds.json` each, plus one rule
each in `rules.loft`:

| Kind | Rule |
|---|---|
| `stairs` | arrive → change layer |
| a slope's top | the step-up rule with layers of 4 steps: from rise 3 onto the next layer's floor |
| `door` | walk into a closed one → it opens |
| `fence` | climb over, nothing pushed across |

`tree` needs no new rule: it is a blocking, non-opaque terrain kind.

## 4. The editor, as a player sees it

**The camera** while playing: `follow` moves the view a whole tile at a time when the hero
nears an edge of a dead zone, and stops at the level's edges; `rooms` flips a screen at a
time (a dungeon of puzzle rooms).  Always-centred is not offered: on a puzzle board the moving
background is noise.

**Toggling.**  `E` switches between playing and editing.  Editing shows the level at full light
(lighting off, `L` toggles it back for a check) and a status line naming the cursor's cell,
layer and material.

| Key (any keyboard; the Tanmatsu has one) | Does |
|---|---|
| arrows / WASD | move the cursor; the view scrolls to keep it in sight (the world is unbounded) |
| PgUp / PgDn | the layer up / down; the layers above and below are drawn faintly |
| Tab / Shift+Tab | the next / previous material or thing in the palette; `1`..`9` jump to a category |
| Space | paint the material, or put the thing on top of the cell's stack |
| Backspace / Delete | take the top thing off; on an empty cell, clear the terrain to void |
| `F` | fill: flood the connected area of the cursor's material with the palette's |
| `R` (hold Shift + move) | rectangle: paint the rectangle from where Shift was pressed |
| `I` | pick up the material under the cursor into the palette (eyedropper) |
| `H`, `G` | put the hero's start, the goal cell here |
| `T` | edit the sign text here (typed with the keyboard) |
| `C`, `V` | copy the rectangle, paste it at the cursor (a house, a tower: STAMPS) |
| Ctrl+Z / Ctrl+Y | undo / redo |
| Ctrl+S, Ctrl+O | save / open a level by name |
| `P` | play-test from the hero's start; `E` returns to the editor with the level as it was |

**The palette** sits along the bottom of the screen, drawn with the theme's own sprites.  Its
categories are Outdoor, Water, Castle, House, Dungeon and Things, and `theme.json` assigns each
material a category.

**Stamps.**  A stamp is a saved rectangle of cells and things, across layers (a house with its
roof, a tower with its stairs).  Stamps live in `stamps/<name>.json`, in the level format, so
building a village is pasting houses.

**Undo** is a log of edits, each "cell (x, y, z) was A, is B" or "stack was S, is T".  Undo
walks the log back; a fill or a paste is one entry.

**Play-test** copies the level, plays the copy, and drops it on `E`; the edited level is never
the played one.

## 5. Checks the editor shows

The `tile_world` routines answer the questions a level designer asks:

| Question | Routine | Shown as |
|---|---|---|
| can the hero reach the goal from the start? | `shortest_path` | the path drawn over the level, or the goal marked unreachable |
| is every chest reachable? | `shortest_path` per chest | an unreachable chest blinks |
| is a chest hidden (DESIGN.md: never behind a wall)? | `line_of_sight` from the cells around it | a chest no walkable cell can see is flagged |
| how dark is it here? | `shades` | the `L` view: the level with its lighting |

A path search ignores what pushing could change, so these are hints, never proof that a level
can be solved.  A level's solution test (DESIGN.md's principle) stays the proof.

## 6. Every platform

| Need | Desktop | Browser | Tanmatsu |
|---|---|---|---|
| keys | `screen::keys()` | the canvas's keys (when `screen` has its browser section) | `input::input_tick_device()` |
| typing (names, signs) | the window's key presses, through `input` | same | the keyboard's letters |
| pointer (optional) | `screen` pointer, X11 mouse | the canvas pointer | none: keyboard only |
| saving levels | `levels/` beside the game | the browser's storage, through loft's file system there | the SD card |

Keyboard-first is the rule: everything in § 4 works without a pointer, because the Tanmatsu
has none.

## 7. What the libraries gain (generic, no game in them)

| Library | Addition | For |
|---|---|---|
| `tile_world` | `layers(w)`, `chunks(w, z)`, `stacks(w)` — built (0.3.1); edges, ground and rise — built (0.4.0); `ground_layers(w)` — on a branch (0.4.1) | saving a level; the view's scroll bounds; houses, open land, slopes |
| `tile_world` | `region(w, x, y, z, limit)`: the connected cells of one kind and tile | flood fill |
| `tile_world` | `copy(w, rect, z0, z1)` / `paste(w, from, at)` of cells and stacks | stamps, copy and paste |
| `tiles` | `rect(surface, x, y, w, h, colour)` and `frame(...)`: a filled and an outlined rectangle | the cursor, a selection, the palette's frame |
| `screen` | the pointer (position, buttons) where the window has one | optional mouse editing on a desktop |
| `screen` | the browser section (a canvas) | the editor and the game in a browser |
| `input` | typed text: the characters pressed this tick (not only key codes) | names, sign text |

## 8. Order of work

1. **Level files** — built: `levels.loft` reads and writes the format of § 2, the two levels are
   `levels/*.json` played in `levels/order.json`'s order, and goals are data.  Materials (§ 1)
   join the legend in step 3; until then a legend names terrain kinds.
2. **The editor's core** — built: `editor.loft` (tested in `tests/editor.loft`), drawn and
   keyed in `game.loft`, the play view's camera in `camera.loft`:
   - the `E` toggle, cursor and scrolling, layers, the palette;
   - paint, erase and stack things;
   - the start and goal markers, save and open;
   - undo and play-test.

   Until materials (step 3) the palette is void, the terrain kinds and the things.  Opening is
   the next level of `order.json`: by name waits for typed text (step 7).  Uses `tiles`
   `rect` / `frame` (0.3.0).
3. **Materials and autotiling**: grass, paths, shores, castle walls and fences from the Kenney
   sheets.  Built: `materials.loft` (tested in `tests/materials.loft`) — a cell's tile number
   names a picture (`1 + sheet slot * 256 + tile`, slots in theme.json's sheet order, so a new
   sheet goes at the end), a material is a kind with a tile, variants by position or a
   nine-slice autotile set with inner corners, worked out when painted or loaded; level
   legends name materials (`levels::load_with` / `save_with`), the palette offers them, and an
   edit logs every cell it retiled.  Materials: grass, path, lake (shores), castle wall (a set
   with no inner corners: a missing piece is drawn as the centre).  The palette's categories
   are theme.json's `categories`, `1`..`9`.  Fences stay a line kind (across / along) until
   a set reads a cell's four neighbours as a line rather than a slice; the town sheet's
   one-row wall strip (99..101) waits for the same.
4. **The new kinds and their rules**: `stairs`, `door`, `fence`, `tree` — built (kinds.json,
   rules.loft, tests/moves.loft) — and the roof cut-away; then castles, houses and dungeons can
   be built and walked through.
5. **Brushes and stamps**: fill, rectangle, eyedropper, copy and paste, stamp files.  (Needs
   `tile_world` `region` / `copy` / `paste`.)
6. **The checks of § 5.**
7. **Platforms**: the desktop pointer, typed text, browser storage, the Tanmatsu's SD card.
