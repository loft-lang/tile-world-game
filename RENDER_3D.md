<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The game in 3D: an experiment

The same game drawn as a 3D place: the environment built as geometry with the current pictures as
its textures, and everything else — things, the hero, the enemies, the people, what they hold —
as cardboard cut-outs standing in it.  An experiment (owner, 2026-11-05): a design to try, not a
plan the game depends on.

It runs where OpenGL does: the desktop (OpenGL 3.3) and the browser (WebGL2).  The Tanmatsu keeps
the 2D renderer; it would need real work to allow anything close to 3D (owner).  This is plain
OpenGL and shaders, through the `graphics` library.  TANMATSU.md § One world, drawn at more than
one fidelity is the frame: the world draws nothing, and each machine draws it with the renderer
it can afford.

Every choice below that is not the owner's is **proposed**.

## What stays and what is new

- **Unchanged:** the rules (`rules.loft`), talk, places and doors (`levels.loft`), the editor's
  core, the input, and `motion.loft`.  The 3D renderer reads the same state the 2D one does: the
  level, the figures with their poses (`game::figures`), the sparks, the blasts, the labels.
  Nothing in play changes; a fight is the same fight.
- **New:** `render3d.loft`, which builds and draws the 3D picture, and its shaders.
  `game::draw` picks the renderer (`TW_3D=1`, proposed); the editor stays 2D (below).
- **Pictures:** Kenney's sheets and ours, as they are.  `theme.json` gains a few optional keys for
  what 2D never needed (a box's top, an edge's look); each is derived when left out, and a key
  set by hand overrides it (the owner's rule: the developer places intent, the game derives the
  rest, every detail adjustable).

## Units and axes

A cell is 1 × 1.  A layer is 1 high: the 2D game draws a layer a tile further up, and the
pictures assume a wall is as tall as a tile.  A rise step (4 of a tile's 16 pixels) is ¼, and a
thing's height in the rules counts in the same steps.  `x` is east, `y` is up, `z` is south (the
2D's `y`, growing down the screen).  A tile's 16 pixels span one unit, so a texel is 1/16.

## The environment: geometry with the pictures on it

What the rules call terrain, and the walls between cells, is the environment.  Each kind is
built from its properties, not its name, as the rules read them: a new kind with the same
properties is built the same way.

| What (by property) | Geometry | Texture |
|---|---|---|
| Ground: terrain not `upright` (floor, rail, water, stairs, bridge) | a top quad at its rise; its four corners at the heights the 2D's slopes give (`corner` in `game.loft`: the lift of each corner) | what the 2D draws for the cell: its material's tile, an autotile piece (path, shore), a variant, a track piece by its joins (`kind_sprite`) |
| A bank: ground higher than the ground south of it (and east, west, north: 3D shows all four) | a side quad down to the lower ground | the same tile, darker, as the 2D draws a bank |
| Water | a top quad ⅛ below the ground beside it | its tile; the shore pieces lie on the land cells, as in 2D |
| Stairs (`stairs` ±1) | a ramp quad from this layer's ground to the next one's | the stairs tile |
| A wall: terrain `upright` and `opaque` | a box one layer high | its sides: the cell's own picture (dungeon:40, a house wall's autotile piece); its top: the layer above's ground at that cell when there is one (what the 2D draws over it a tile up), else the kind's `upper` (dungeon:57) |
| A door (`opens_to`) and an open door | the doorway: the walls beside it continue; the door picture on a panel set in the opening — closed across it, open swung against the side | the door tile (dungeon:45, the house doors' town:74 and 78) |
| A fence: `upright`, a line (`along`) | a panel through the middle of the cell along its line, as tall as the picture | the 2D's choice of across or along, on both faces |
| A standing beam, a held-shut door (`barred`) | as a wall or a door, with their pictures | their tiles |
| A roof (a material of kind `wall` with a roof autotile, on the layer over a house) | a slab one layer up, the autotile piece on top; proposed later: a pitched roof from the autotile's rows (the `n` row the slope up, the `s` row the eaves) | the autotile piece |
| The walls between cells (`edges` in `kinds.json`: wall, door, window, fence) | a thin panel (⅛) on the cell boundary | the matching terrain's picture; `theme.json` has no edge looks yet, so they are proposed there: `edges: { wall: …, door: …, window: …, fence: … }` |
| Void | nothing | — |
| Floor text (`labels`) | a decal on the ground, lifted a hair above it | the font's tiles at half size, as `draw_labels` writes them |

A box's faces all take the face picture, so a camera that turns later (below) finds a side
textured too.  The nine-slice pieces of a material exist to draw edges the 2D has no geometry
for; in 3D the box makes those edges, so a wall's sides use the material's middle face piece and
its top the middle top piece (derived; `theme.json` may name them: `faces: { side, top }`).

Trees, wells and basins are terrain in the rules but pictures of objects: they are cards (below),
standing on their cell's ground.

## Everything else: cardboard

A **card** is a quad 1 × 1 (a tile's picture at one texel per 1/16) standing upright on its base
point, the bottom middle of the picture.  It turns about the vertical axis to face the camera
(proposed; with the fixed camera below it shows the picture straight on, as the 2D does).
Texels with alpha below ½ are discarded, so the depth buffer sorts cards and boxes together and
nothing has to be sorted by hand.

- **The board:** a card is drawn twice, the second copy 1/32 behind it in the outline colour
  (`o`, 3f2631), so the cut-out's edge shows as a thin dark board (proposed; cheap, and the
  sheets' own outline already suits it).
- **A shadow:** a soft dark ellipse on the ground under each card, so a card stands rather than
  floats (proposed).
- **Things:** each thing of a cell's stack is a card, its base at the cell's rise plus the heights
  of the things under it (`draw_stack`'s rule: 4 pixels a step becomes ¼); a thing holding
  another (a socket and its torch) is one card when the theme has the pair; a flame shows its
  frame at the level's clock and its low picture in its last minute — the same choices as 2D.
- **Figures:** the hero, the enemies and the people are the list `game::figures` already makes,
  each with its fine pose (`motion.loft`): a body card, a held card at the hand, a shoulder card
  behind.  The pose is read in the card's plane: its x and y become an offset over the ground
  (pixels / 16), the hop lifts it, the tilt rolls the card about its feet, the weapon's angle turns
  the held card about the hand, and the reach pushes it along that angle.  A cut-out figure so
  moves exactly as the 2D figure does, and `motion.loft` and its hand-computed tests carry over
  unchanged: the guard, the flick, the brace, the stagger, the wind-ups, the wounded sway.  Facing
  north, what is held is drawn behind the body, as in 2D (a small depth bias).
- **Effects:** a blast's frames, a spark, an arrow (a card turned along its flight), a torch's
  flame — cards.
- **What is said** (talk bubbles) and the status line (the demo world's only) stay 2D, drawn over
  the 3D picture; a bubble sits at its speaker's head as projected to the screen.

## Textures

The tiles go into one atlas at start, each copied into an 18 × 18 slot with its border pixels
repeated one pixel outward: the 182 tiles `theme.json` uses fit in 256 × 256 (196 slots), all
798 of the six sheets in 1024 × 512.  The repeated border keeps
a nearest sample at a tile's edge from reading its neighbour, which plain sheets (no spacing)
would do.  One atlas means one texture bound for the whole place.

Sampling is nearest, so the pictures keep their pixels; no mipmaps at first (proposed: the
camera stays close).  A tile's place in the atlas follows from the sprite number `theme.loft`
already uses (sheet and index), so every choice the 2D makes names its texture without a new
table.  The atlas is built with `graphics::Canvas` and uploaded with `texture_from`.

## Light

The rules' light, not a lighting model.  The 2D computes a light for every ground corner
(`ground_of`, `corner`: sixteenths of a shade) and for every standing cell's corners
(`standing_light`); those numbers become the vertices' colours, so a torch lights the same cells
in 3D as in 2D and fades the same way.  A card takes its cell's light, as `draw_figure` does.
Night is the rules' night (`daylight`).  The shader multiplies the texture by the light; the
2D's shaded palettes (100 / 75 / 50 / 35 %) become that factor.

Proposed on top: a fixed light from the top left for the box faces only (south faces a little
lighter, west faces darker), matching the 2D's rule that ground falling away from the top left is
darker.  It shapes the boxes without lighting anything the rules leave dark.

## The camera and what is seen

**The camera:** a perspective view (a 35° field, proposed) from the south, looking down at about
55°, the angle the pictures are drawn for.  It follows what `camera.loft` follows (`follow`,
`rooms`), the same target turned into a point over the place.  It keeps its yaw at first: a
camera turning round the place would show cards edge-on.  A small swing (±30°) is the next try,
the cards turning with it.

**What is seen:** DESIGN.md § What the player sees carries over as rules, not as a look:

- under a ceiling only the hero's layer and those below are drawn; nothing above (roofs vanish
  while inside), the same test (`open_sky`);
- under the open sky the layer above is drawn too;
- a wall in front of the hero is cut away (owner: only walls).  In 2D it is the wall cell south of
  the hero; in 3D it is every wall whose box hides the hero's card from the camera — the cells a
  line from the camera to the hero's head crosses (proposed);
- things and figures past a wall are not drawn (`in_view`, owner: only walls hide); the ground is;
- layers below a ledge are drawn darker each layer down, to about four deep, as in 2D.

## The editor

The editor stays 2D: it edits a grid, and its cursor, palette and rectangles are 2D tools.  E
switches to it from the 3D view, and play-testing (P) returns to 3D.  A 3D look at the place
being edited (a key, proposed) is a later step.

## Drawing it fast

- **Meshes per chunk:** tile_world keeps each layer in 32 × 32 chunks; the renderer builds one
  mesh per chunk and layer (`graphics::GroupMeshes`, `upsert` by chunk), rebuilt only when a cell
  in it changed.  tile_world counts no changes today: either the renderer keeps each chunk's last
  cells and compares them (1024 numbers, cheap), or the library gains a change count per chunk
  (a library change, ours to make and test on its testbed).
- **Cards instanced:** one quad, one instance buffer per frame (position, atlas rectangle, roll,
  light, the held card's angle), drawn in one call (`draw_instanced`).
- **Light per vertex:** the ground light is recomputed where the 2D recomputes it (a flame lit,
  moved, gone), so a static place costs nothing per frame beyond drawing.

## Built in steps, each checked

Pictures are made offscreen (a `graphics::Framebuffer`, `capture`, saved as PNG), so every step
is checked by a program and looked at.

1. **The atlas and one place's ground.**  Checked: an orthographic camera straight down, one
   screen pixel a texel, gives the 2D game's ground pixel for pixel.  The 2D frame of the same
   moment is the oracle; a tool compares the two.
2. **Layers, walls as boxes, slopes, banks, stairs.**  Checked: the same top-down comparison for
   the tops; the 3/4 view looked at for the faces.
3. **Cards: things and figures with their poses.**  Checked: a card's transform at a moment
   follows from `motion.loft`'s pose, whose numbers are already tested by hand; `tools/anim.loft`'s
   scenes (the guard, the rat, the bomb) filmed in 3D.
4. **Light.**  Checked: a vertex's light equals the 2D's shade for that corner at the same moment.
5. **What is seen.**  Checked: the rules `checks/game.loft` holds for the 2D (a wall in front cut
   away, only walls hiding) asked of the 3D renderer's list of what it drew.
6. **Effects, bubbles, the editor switch.**
7. **The browser.**  WebGL2 through `loft-gl-wasm.js`, which turns the `330 core` shaders into
   `300 es`: one set of shaders for both.
8. **Frame time** on the desktop and in the browser, in the demo village.

## What it asks of the libraries

`graphics` has what the steps need: a window, shaders, meshes with indices, instancing,
textures from a canvas with nearest filtering, framebuffers and capture, blending and depth
writes; `mesh3d` has the perspective and look-at matrices.  To verify on the way (not known from
the catalogue): control of multisampling and alpha-to-coverage (smoother card edges), and whether
`GroupMeshes` fits per-layer chunks.  tile_world: the per-chunk change count above, if the
comparison is not enough.

## Open, for the owner

- The camera: fixed from the south, or turning (then cards turn with it, or keep facing south).
- Cards for everything that is not terrain, as asked; whether a few big things (a chest, a table,
  a cart) read better as boxes is worth one try.
- Roofs flat or pitched.
- The pixel look: drawn at the window's size with nearest texels (sharp texels, smooth edges), or
  drawn small and scaled up (chunky pixels, as the 2D game).
- The board's edge and the shadow under each card.
