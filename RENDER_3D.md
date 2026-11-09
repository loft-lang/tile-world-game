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
the 2D renderer; it would need real work to allow anything close to 3D (owner) — and is where an
early-phone style of 3D is to be tried later (§ Later).

**Scope since 2026-11-06 (owner):** a *pure* 3D version — the world model built in full, every
angle it holds (the 45-degree slants, the slopes, the ceilings), the current pictures as its
textures — wanted soon; the Looking Glass tile model (Ultima Underworld, System Shock) is the
comparison, on stacked layers.  Then two later steps: the same places mapped onto the hex grid, as
an import tool for the hex editor, and the phone-style experiment on the Tanmatsu (§ Later).  This is plain
OpenGL and shaders, through the `graphics` library.  TANMATSU.md § One world, drawn at more than
one fidelity is the frame: the world draws nothing, and each machine draws it with the renderer
it can afford.

Every choice below that is not the owner's is **proposed**, or a **default** taken to change later
(§ Defaults).

## Why it matters (owner, 2026-11-06)

The mode looks like a gimmick, and is the opposite: it is the step that takes an indie developer
from a concept tested in 2D to a game they can sell.  The pictures stay simple, but a 3D game
reaches an audience a 2D one does not, and the developer gets room to play in it — the same world
model, the same rules, seen as a place.  Through the hex-world conversion (§ Later) it opens a
further dimension: the hex tools, more precise layers, and models exported from Blender and the
like.  The game already held an almost-3D world (layers, rises, slants, ceilings); this view makes
that official.

## The first-person view (built, 2026-11-06)

Built first, ahead of the steps below (owner): `render3d.loft`, in a browser page only, beside the
2D game.  **F1** switches between the two where the frame is drawn by a page's GPU; the desktop and
the Tanmatsu stay 2D, and the editor is always 2D (F2 switches the view off).

- **What is seen:** the place round the hero (12 cells each way, two layers up and down) from the
  hero's eyes, its facing eased as it turns.  Floors at their rise, bent at the corners as the 2D
  bends its ground, with banks to lower ground; walls as boxes; slanted walls as prisms with their
  cut face and the floor and ceiling of their open half; ceilings at their drop, bent the same way;
  the undersides of floors and rock above the hero — over a room too (a house's ceiling) and over
  a stream's bed (a bridge seen from below).  The thin walls between cells (a house's inner walls,
  owner, 2026-11-08: walls the hero bumped into unseen) stand on their edges, faced with the
  house's own wall picture: a wall a panel floor to ceiling, a door its two posts and the lintel,
  a window the wall round an opening, a fence or a grate's frame a low rail.  Stairs up are four
  steps to the layer above, rising toward the wall they stand against (`stairs_way`: a house's in
  its corner, a stair road's into the terrace), and the stairwell over them is open (owner,
  2026-11-08: the stairs in 3D).  Trees, fences, open doors and things are cards
  turned to the eye (an item on the floor half a cell, furniture a little under one, a figure 0.85);
  the enemies and people glide to their cells.  The light is the rules' (`tile_world::shades`), and
  the distance fades into the sky by day or night under the open sky, into the dark under rock.
- **In front:** what the hero holds — a sword, an axe, a torch — at the bottom right, swaying as it
  stands, bobbing as it walks, turned by the swing as the 2D turns it.  Empty hands show nothing.
- **Over it:** the note and the hearts, on the game's own surface.  Talk bubbles are not drawn yet.
- **The keys:** forward walks the way the hero looks; back steps back still looking ahead (with
  pull held, it pulls what is in front); left and right turn on the spot.  They are turned into the
  2D game's keys before it reads them, so the game's code is the 2D game's (owner: keep it so).
- **The pictures:** each, the first time it is needed, is drawn by the theme onto a small
  offscreen surface, read back and put in a 1024 x 1024 atlas — the 2D's colours exactly, and the
  theme untouched.
- **Checked:** `checks/view3d.loft` (a box, a prism, a ceiling at its drop, a bent floor, the thin
  walls, the stairs and their open stairwell, a floor's underside over a room, a pitched roof, by hand, both
  backends); pictures taken in headless Chromium in the village and the caves, and in Orbury on
  the stair road and in Hollis's house.
- **Not yet:** talk bubbles, a door's panel (a door is a box), a figure's held
  card, culling (every face is drawn; a place round the hero is a few thousand faces).

A key pressed and released between two frames is not seen: `screen` keeps which keys are down, not
the presses, and the game reads that once a frame.  At 60 frames a second a tap spans several
frames; a slow frame can lose one.  The cure is `screen`'s — a press latched until the next poll.

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
| A roof (a material of kind `wall` with a roof autotile, on the layer over a house) | one slope facing south, as the 2D's pictures show it (§ Roofs) | the autotile pieces, row by row |
| The walls between cells (`edges` in `kinds.json`: wall, door, window, fence) | a thin panel (⅛) on the cell boundary | the matching terrain's picture; `theme.json` has no edge looks yet, so they are proposed there: `edges: { wall: …, door: …, window: …, fence: … }` |
| A slanted cell (`tile_world::slant`, DESIGN.md § Slanted rock) | its kind's geometry cut along the diagonal: a wall becomes a triangular prism one layer high, its two whole sides and its cut face standing on the cell's half away from the open corner; the open half is the neighbour's ground (a triangle at that neighbour's height), and under a ceiling its ceiling too | the sides as a wall's; the cut face takes the face picture stretched along the diagonal (1.41 units for 16 texels); the top the wall's top, cut as the 2D cuts it |
| A sloped top on a slanted or whole wall | the top's four corners at the cell's rise as the ground's are (`corner`): a rock face on a hillside ramps instead of standing in flat steps | the top picture, bent as the ground's is |
| A ceiling (§ Ceilings) | a quad facing down at the underside of the first set cell above a space, lowered by the drop of the topmost open cell beneath it; its corners from the neighbours' drops, as a ground's from their rises | the rock's `upper` picture (proposed; `theme.json` may name a `ceiling` look) |
| Void | nothing | — |
| Floor text (`labels`) | a decal on the ground, lifted a hair above it | the font's tiles at half size, as `draw_labels` writes them |

A box's faces all take the face picture, so a camera that turns later (below) finds a side
textured too.  The nine-slice pieces of a material exist to draw edges the 2D has no geometry
for; in 3D the box makes those edges, so a wall's sides use the material's middle face piece and
its top the middle top piece (derived; `theme.json` may name them: `faces: { side, top }`).

Trees, wells and basins are terrain in the rules but pictures of objects: they are cards (below),
standing on their cell's ground.

### Roofs

A roof follows what the 2D's pictures say it is (owner).  Its autotile's north row (town:52–54,
48–50) carries the ridge cap along its top edge; every row south of it, the south row included,
is the same run of shingles (64–66, 60–62): the front slope, seen from the south, rising to the
ridge.  The back slope is out of view in 2D (owner, 2026-11-08: the village tiles show the house
sloping up to its middle and down out of view again).  **Built (2026-11-08)** for the roofs that
are floors walked on (`house_roof`, `house_slate`: `"pitched": true` in theme.json; the walled
roofs of the demo village stay boxes):

- a roof slopes up from its eaves at the front and back walls, on the layer's floor, to a ridge
  **one layer higher in the middle of the house** (owner: sloping down to the walls, rising a level
  between them) — steeper on a shallow house, flatter on a deep one;
- the front slope takes the shingles, the row meeting the ridge the piece with the cap; the back
  slope the same pictures mirrored, its cap at the ridge too; the end columns their end pieces;
- the triangles at its east and west ends, the gables, are closed in the house's own wall picture
  (the wall ring below them);
- a hole where the roof came down (the inn's) stays open, the roof spanning it on either side,
  no gable standing in it;
- standing on a roof the hero walks it as a floor, so the roofs of that layer are drawn flat then;
- under it, the top floor's ceiling is flat.

Kenney's sheet also has gable fronts (town:63, 67) that the 2D does not use; a roof turned the
other way (its ridge north–south) would want them, and none is drawn so yet.

### Ceilings

The world holds a ceiling for every cell (owner, 2026-11-06; `tile_world::ceiling`, 0.9.0): how
many steps it hangs below the floor of the layer above, 0 — its underside — when nothing says
otherwise.  The 2D draws none and no rule reads it; it is modelled so this renderer can draw what
the Looking Glass games did with theirs — a low passage, a sloping ceiling, a vault — on stacked
layers, where theirs had one height map.

- **Which cell holds it.**  A space's ceiling is the underside of the first set cell above it,
  lowered by the drop of the topmost open cell beneath that: under a dome three layers high the
  ceiling over the floor cell (x, y, 0) is held by (x, y, 2), an empty cell.  A cell under the open
  sky has no ceiling, whatever it holds.
- **Slopes and vaults** come from the neighbours, as the ground's do: a corner's height is the mean
  of the drops of the open cells sharing it (within a step of each other; a bigger difference is a
  step in the ceiling, its side a quad like a bank's).
- **Set by hand** in the editor: Shift+`[` lowers the ceiling over the cursor a step, Shift+`]`
  raises it (0 .. 3; a layer is 4), saved in a place's `ceilings`.  **Derived** for domes as a vault
  — the dome's ceiling height in steps rather than layers, so its ceiling curves instead of
  stepping — is a step of this renderer (below): until something draws it, nothing is derived.
- **Seen** only from below: the fixed camera from the south draws only the hero's layer and those
  below under a ceiling (§ The camera), so ceilings show when a camera can look up at them — the
  pure 3D version's question (§ Later).

## Everything else: cardboard

**On a CPU (owner, 2026-11-09; BUILD.md Phase E):** the cards that move — the hero, the people,
the monsters, the things in motion — are not drawn through the 3D routines but blitted as
sprites, scaled by their distance and depth-tested against the kept picture of the landscape,
which is drawn again only when it changes (@PLN218's landscape and actors).  What follows is the
GPU's drawing, and the picture both must give.

A **card** is a quad 1 × 1 (a tile's picture at one texel per 1/16) standing upright on its base
point, the bottom middle of the picture.  It turns about the vertical axis to face the camera
(proposed; with the fixed camera below it shows the picture straight on, as the 2D does).
Texels with alpha below ½ are discarded, so the depth buffer sorts cards and boxes together and
nothing has to be sorted by hand.

- **The board:** a card is drawn twice, the second copy 1/32 behind it in the outline colour
  (`o`, 3f2631), so the cut-out's edge shows as a thin dark board (a default; cheap, and the
  sheets' own outline already suits it).
- **A shadow:** a soft dark ellipse on the ground under each card, so a card stands rather than
  floats (a default).
- **A kind's shape is one key** (owner: cardboard for now, swapped out later where it pays):
  `theme.json` may give a thing or an enemy a `shape` (`card` when left out, the only one built).
  A box or a model for one kind later replaces that kind's card and nothing else.
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

**The camera** stays close to what the 2D shows (owner), a little more horizontal.  The 2D view
draws a cell's ground and a wall's face equally tall: a wall one layer high takes one tile, and
so does the cell behind it.  That is what a camera looking down at 45° from the south sees, so
45° is the 2D's own angle; the 3D camera starts a little lower, at 40° (a default), so faces and
cards stand a little taller than the ground they stand on.  The field is narrow (25°, a default),
so the perspective stays mild and a row far off is not much smaller than one near.  It follows
what `camera.loft` follows (`follow`, `rooms`), the same target turned into a point over the
place, and it does not turn: the pictures are drawn to be seen from the south.

**What is seen:** DESIGN.md § What the player sees carries over as rules, not as a look:

- under a ceiling only the hero's layer and those below are drawn; nothing above (roofs vanish
  while inside), the same test (`open_sky`);
- under the open sky the layer above is drawn too;
- **the hero is never hidden** (owner): the walls in front of it are cut away, as in 2D, and only
  walls.  The 2D cuts the wall cell south of the hero (and the ledge's top drawn over it): at the
  2D's angle no wall further south reaches the hero.  The same rule in 3D asks which walls cover
  the hero's card from the camera.  With the camera lower than the 2D's, a wall two or more cells
  south, or one taller than a layer, can reach it too.  So the renderer projects the card's
  rectangle toward the camera and cuts every wall box it passes through (in the hero's column
  and, where the card is wider than a cell, the columns beside it), with what stands on those
  walls on the layers above.  At 45° this cuts exactly the 2D's cell;
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
   away, only walls hiding) asked of the 3D renderer's list of what it drew; at 45° the cut is
   the 2D's cell exactly; at the 3D camera's angle, a hero behind two walls in a row, and behind a
   wall two layers high, is still seen in part.
6. **Effects, bubbles, the editor switch.**
6b. **The angles: slants, sloped tops, ceilings.**  Checked: a slanted wall's prism seen from
   straight above is the 2D's cut picture pixel for pixel (step 1's oracle); a ceiling's corners
   equal the drops' means by hand on a small place; the dome derives its vault, and the dome house
   is looked at from a camera that sees up.
7. **The browser.**  WebGL2 through `loft-gl-wasm.js`, which turns the `330 core` shaders into
   `300 es`: one set of shaders for both.
8. **Frame time** on the desktop and in the browser, in the demo village.

## Later

- **A camera that looks up** (to decide with the owner).  The pure 3D version asks for every angle
  the world holds to be seen, and a ceiling is seen only from below.  Proposed: beside the fixed
  camera from the south (decided), a second one, free or at the hero's eyes, as the Looking Glass
  games had; the walls-in-front cut-away is the fixed camera's only, and cards turn to face it.
- **The hex grid: an import tool.**  The same places mapped onto the hex grid (`loft-libs-world`'s
  `hex_world` and the packages round it) for the more complex hex editor (owner): a square cell's
  kind, rise, slant and ceiling turned into hex cells, so a place made here opens there.  Its
  design waits for this renderer: the 3D model is the common ground the two grids are mapped
  through.  **One way only** (owner, 2026-11-09): square maps into hex, never back — a hex place's
  curves and twelve facings have nothing to land on in squares.  So the square place is the draft
  that plays now; the more realistic town (a winding stream, houses at angles, the hard part of
  level design, and why the hex grid was made) is modelled in hex after the import, and what is
  refined there does not come back.  **How, since** (owner, 2026-11-09): not the cells but the
  drawing kit's shapes (a room, a house, a road, a river, a tree) are filled into the hex grid,
  as moros's hex editor fills its own, with houses at 30° angles and walls thinner than a cell
  (WORLD.md § One game, square or hex).
- **The Tanmatsu: an early-phone style of 3D** (owner: an experiment, after this one).  No GPU:
  the techniques of the phones before one — low polygon counts, fixed-point transforms, a software
  rasteriser drawing affine-textured spans, flat or per-vertex light — tried on the same world
  model.  **Now a phase** (owner, 2026-11-09): BUILD.md Phase E ports the 3D routines to the
  Tanmatsu before the hex grid comes into the area designs, so no backend is lost.  What is
  ported is the building of the picture (what is seen, the geometry, the cards, the light); the
  GPU's drawing is replaced by one in software.

## What it asks of the libraries

`graphics` has what the steps need: a window, shaders, meshes with indices, instancing,
textures from a canvas with nearest filtering, framebuffers and capture, blending and depth
writes; `mesh3d` has the perspective and look-at matrices.  To verify on the way (not known from
the catalogue): control of multisampling and alpha-to-coverage (smoother card edges), and whether
`GroupMeshes` fits per-layer chunks.  tile_world: the per-chunk change count above, if the
comparison is not enough.

## Decided (owner, 2026-11-05)

- The camera stays close to the 2D's view, a little more horizontal; it does not turn.
- Cardboard for everything that is not the environment, for now; a kind's `shape` swaps it later.
- Roofs follow the 2D's pictures: one slope facing south, ridge at the north.
- The hero is never hidden: the 2D's cut-away of the walls in front of it, found by projection.

## Defaults, to change later

Taken as reasonable defaults (owner: change them later).  Each is one named constant in
`render3d.loft`, so a change is a number, not a rewrite.

- **The pixel look:** drawn at the window's own size, texels sampled nearest — sharp pixels on
  the pictures, smooth edges on the geometry.  `RENDER_SCALE` (1) draws into a smaller target
  and scales it up nearest instead: 1/3 gives the 2D's chunky pixels.  The window's size wins
  first because a perspective drawn small shimmers as the camera follows the hero.
- **The board's edge:** on, a copy 1/32 behind each card in the outline colour (`BOARD`).
- **The shadow:** on, a soft ellipse ¾ of a cell wide under each card, darker the nearer the card
  stands to the ground (`SHADOW`).
- **The ridge:** one layer above the eaves (`RIDGE`).
- **The camera:** 40° down, a 25° field (`PITCH`, `FIELD`).
- **The cut-away:** a wall is cut when it covers any of the hero's card (`CUT_COVER` 0: any part),
  the strictest reading of "never hidden".
