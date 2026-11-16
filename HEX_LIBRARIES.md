<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The hex work, into the hex libraries

**Status** — open.  Phases 1 to 6 are the merge the round and eight-sided structures wait on.  Phases 0 and 9 are done (the game uses the library's routines where it had
exact copies; the view keeps its heights); phases 1 to 6 have moved — the merge is done but for phase 4's speed work.

**Goal.**  The game grew a hex way of working on top of the hex libraries (`hexwalls.loft`,
`hexturn.loft`, `hexhouse.loft`, `hexterrain.loft`), and on the way it solved what the libraries
were stuck on: a structure at any 30° angle, straight walls from cells alone, openings that
survive a turn.  Each of those files says "built here first; the routines move to the hex
libraries once they are right".  This is the plan for that move: the libraries get the routines,
the game then uses the libraries, and nothing the game draws changes.

The libraries are `loft-lang/loft-libs-world` (`hex_*`); the game fetches them from `main`
(`fetch-libs.sh`).

## What must hold throughout

1. **The formal rules work is not stopped.**  Read here in both directions:
   - *In the libraries.*  Every library change is an addition.  No existing routine changes what
     it answers, is renamed or is removed, so nothing a library law, test or worked example
     (`@HX…`) says today becomes untrue, and nobody working on those has to wait or rebase.  Where
     the game's answer differs from a library's (below, "Differences"), the library's stays and the
     game's arrives beside it under its own name.  The work is done from this side, as
     `tile_world`'s releases are; it asks nothing of the loft stream.
   - *In the game.*  The rules in `formal/hex-houses.md` (`@FR-H…`) stay the rules.  A routine
     moves with the rules it keeps: its citations go with it, and `rule_tags.py check` passes at
     every commit.  No rule is reworded to make a move easier.
2. **The game's output does not change.**  `levels/orbury.hex.json` regenerated
   (`loft --lib libs tools/hextown.loft level`) is byte for byte the committed file after every
   phase, unless the phase says it changes it and shows the difference.
3. **The owner's rulings stand** (2026-11-09, 2026-11-14): the world is drawn only from the cells;
   turning is an editor step and is never refused; walls are always straight; a road's sides are
   walls with no height, from its cells.

## What was found

Read in full: the game's six hex files (3 468 lines) and all fourteen `hex_*` packages.  Measured
with a probe, not only read:

| The game's | The library's | Measured |
|---|---|---|
| `hexhouse::fill`, one box | `hex_shape::box_fill` | the same cells at all 12 turns, 4 sizes; the game's is exact integers and takes several rectangles, the library's a float with an epsilon and one box |
| `hexturn::turn`, 60° steps | `hex_field::cell_rot` | 0 cells differ |
| `hexwalls::loops` | `hex_field::trace` | the same loops, the same way round; the game's keeps each edge's two cells, the library's only the corners |
| `cell_near` | `hex_field::hex_at` | 4 of 160 000 points differ (ties); `hex_grid::px_to_hex` differs at 76 |
| `centre` | `hex_grid::hex_to_px` × 0.866 | equal to 7e-15, not to the last bit |
| `solve_tops` | `hex_draw::surface_of` | the same line |
| `solve_sides` | `hex_draw::surface_of` | 0.048 unit (4 cm) apart on a 5 × 4 box |
| a box turned 30° (`build`) | `box_fill` at an odd `rot` | other cells: 5 × 4 gives 27 against 23, 6 × 3 gives 20 against 25, 4 × 4 the same |

## How one routine moves

Each move is the same four steps, and a phase is one move or a few that cannot be checked apart.

1. **Beside.**  The routine is added to its library with its tests, under the name it will keep.
   The library's own routines are untouched.  A library test compares the new routine with the old
   one wherever both answer (a box's side, a 60° turn), exactly.
2. **Released.**  Version raised, `USAGE.md` and the guide name the routine, a worked example
   where a caller can go wrong (the package's own `@HX…` series), the bench twin kept in step;
   the PR merges on a green CI.
3. **Compared here.**  `./fetch-libs.sh`, then a test in `tests/` runs the game's routine and the
   library's over the game's own shapes (`tests/hexhouses.loft`'s shapes, every house of Orbury)
   and asserts the same answer, exactly.
4. **Switched.**  The game's routine is deleted, its callers call the library, the comparison test
   goes with it.  The suite, the checks and the regenerated `orbury.hex.json` are as before.

A step 1 that nothing calls proves nothing, so a phase is not done at step 2: it is done at step 4.

**The rules.**  The libraries keep no `@FR` register.  The definitions stay in
`formal/hex-houses.md`, one place, and the moved code cites them there by the same tag.  The check
then reads the fetched sources too:

```sh
RULES_DIR=formal CITE_DIRS=.:tests:tools:libs/hex_draw/src:libs/hex_place/src:libs/hex_shape/src:libs/hex_form/src:libs/hex_field/src \
  CITE_EXTS=.loft python3 scripts/rule_tags.py check
```

The game's tests of a rule stay in the game (they are how the game knows the library still keeps
it); a copy of each goes into the library's tests with the routine.  If the libraries get a rule
register of their own, the definitions move there in one step and this file's check follows.

## Phases

Effort: XS a sitting's part, S a sitting, M several, L many.

### 0. The exact copies — done

`hexwalls::surface_of` deleted (a copy of `hex_draw::surface_of` nothing called).
`hexwalls::turn60` is `hex_field::lattice_rot60`, `hexturn::cell_at` is
`hex_field::lattice_to_cell`, and the two places that wrote that out by hand call it.  The corner
tables in `hexscene.loft` and `render3d.loft` are `hex_field::corner_k` / `corner_m`.
*Compared:* `orbury.hex.json` regenerated, identical; the suite and the checks.

### 1. What the game fetches — done

`hex_edge` is needed by `hex_way` and is not in `fetch-libs.sh`; it resolves today from the
registry's cache on this machine.  Added; `hex_place` follows when phase 6 first uses it.
*Goes red:* a build with an empty registry cache.

### 2. Small additions the game already has copies of — done (loft-libs-world#48)

| Library | Addition | Replaces in the game |
|---|---|---|
| `hex_draw` | the miter of two surfaces (`surface_miter` takes a `Plan` and two sides, and its body is this); a surface's unit direction | `hexwalls::surface_miter`, `wall_dir` |
| `hex_form` | the world dot product of two doubled vectors (it has the square, `vec_n2_4`); the sign of a + b·√3 | `hexwalls::dot4`, `hexhouse::sign3` |
| `hex_field` | `trace` with each edge's two cells kept | `hexwalls::loops`, `boundary` |

*Compared:* in the library, the new miter against `surface_miter` on every side pair of a box at
its 12 orientations; the new trace's corners against `trace`'s.  Here, as step 3 above.
*Rules that move:* HW-Loops, HX-Edge (the boundary), HW-Miter.

As landed: `hex_field::trace_edges` (0.1.8), `hex_form::vec_dot_4` and `sign_root3` (0.1.8),
`hex_draw::surface_meet` and `surface_dir` (0.1.5).  **`orbury.hex.json` changed, in numbering
only:** `trace_edges` starts a loop at `trace`'s canonical corner where the game's walk started at
the first edge it met, so in 13 of the 21 houses wall 0 is another of the same walls.  Every
corner, opening, eaves and rim is the same number to the bit (compared as rotations of each
house's walls); the ground, the roads' sides, the cliffs and the fences are byte for byte the same.

### 3. A plan of rectangles, filled exactly — done (loft-libs-world#49)

`hex_shape` gets the fill of several rectangles by exact integer tests (HP-Fill): no epsilon, any
of the 12 headings, mirrored or not.  `box_fill` stays as it is; a library test asserts the new
fill of one rectangle is `box_fill`'s cells over a sweep of sizes and all 12 rotations (the probe
found no difference in 48 cases; the sweep either confirms it or finds where the epsilon decides).
*Replaces:* `hexhouse::fill`, `Part`, `box_plan`, `l_plan`, `reach`.  *Rules:* HP-Fill, HP-Zero.

As landed: `hex_shape` 0.1.9, `box_part`, `parts_fill`, `parts_reach`.  The sweep (3 888 boxes)
found no case where `box_fill`'s epsilon decides.  `hexhouse::fill` and `reach` are one line each
now; `box_plan` and `l_plan` stay here, as the game's own shapes.

### 4. Straight walls from cells alone — moved (loft-libs-world#49); its speed work open

The centre of the work, and what the library's own notes leave to the consumer
(`hex_shape/USAGE.md`: a chain is handed back in order "to cut a marking into runs").  Into
`hex_draw`, which owns `WallSurface`, as a file of its own:

- 4a. the axes and the runs: `axes_of`, `travel`, `runs`, `merged`, `groups`, `run_heading`
  (HW-Axes, HW-Runs, HX-Size);
- 4b. the two solvers: `solve_tops`, `solve_sides`, `solve_wall` (HW-Tops, HW-Sides,
  HW-Straight) — a library test pins what was measured: on a box's side a tops wall is
  `surface_of`'s line, a sides wall lies beside it, and by how much;
- 4c. the structure built: `Built`, `from_cells`, `from_data` without its openings (HX-Cells,
  HX-Editor, HW-Clear);
- 4d. outlines of any number of loops: `outline_of`, `outlines`, `split_by_line`, `headings`,
  `step_edge` (HW-Step, HW-Slant) — what roads, the stream and cliffs are drawn from.

4a–4c are one release (the runs cannot go red without a solver to disagree with); 4d is a second.
*Compared here:* every wall and corner of every Orbury house and every side of its roads, the
game's against the library's, as exact integers (`ws_num`, `ws_den`, `ws_dk`, `ws_dm`).

As landed: `hex_draw` 0.1.6, a file of its own (`cellwalls.loft`): `cells_loops`, `cells_axes`,
`cells_runs`, `cells_wall`, `cells_built` (`CellWalls`, a kind per wall), `cells_outlines`
(`CellOutline`).  4a to 4d went as one release: the town regenerated from the library's routines
in place of the game's is byte for byte the file it was, which compares the houses (4a–4c) and
the roads' sides and cliffs (4d) at once.  `hexwalls.loft` keeps what the game adds: the openings
and the measurements (731 lines to 200).  **Still open:** the speed work listed for this phase
below (`merged` in one pass, `common` counted once, one reading of each edge's travel) — the
routines moved as they were, so the comparison was exact; the boundary is already walked
(`trace_edges`).

### 5. Openings on such walls — moved

`Opening`, `Sizes` (the widths; reading `hexsizes.json` stays in the game), `openings_of`,
`place` — into `hex_draw` beside `place_opening`, which stays.  *Rules:* HO-Point, HO-Width,
HO-Place, HO-Clear.

As landed: `hex_draw` 0.1.7, `cellopen.loft`: `cells_place_opening` and `cells_openings`, both
on the built walls (`CellWalls`), so any number of openings is one solve; `opening_sizes` for the
widths.  `Sizes` and reading `hexsizes.json` stay here, and `hexwalls::Opening`, the same nine
numbers under the names the game's tools and tests read.

### 6. The turn at any 30° — moved

What the libraries were stuck on.  `hex_place` gets the turn of a structure's cells about a cell
by any number of 30° steps, with its openings, and the point carried through it: `turn`,
`turn_data`, `turned_point`, `middle`, both ways of an odd step (`BY_WALLS`, `BY_OUTLINE`).  It
needs phase 4's walls, so `hex_place` comes to depend on `hex_draw`.

`hex_field::stencil_rotate_deg` keeps refusing what is not a multiple of 60°: its law is that an
exact routine does not round.  The new turn is the approximate one, named as such, and it hands
back the offset it moved the structure by — a reported residual, as the family asks of anything
that is not exact.  A library test asserts its even steps are `stencil_rotate`'s cells and edges.
*Rules:* HT-Pivot, HT-Turn60, HT-Turn30, HT-Near, HT-Shape, HT-Drawing, HO-Turn60, HO-Turn30.

As landed: `hex_place` 0.1.4, `cellturn.loft` (not `hexturn.loft`: a file's name is shared by
every program that uses it, and the game has a hexturn of its own): `cells_turn` answers a
`Turned` (the cells, the pivot, the steps, the offset), `turned_point` reads it, `marked_turn`
carries the openings, `cells_middle` gives a pivot.  With its speed work: the turn is worked out
once and read, the walls are solved once for the drawing and once for the turned structure
however many openings there are, one cosine and sine serve a frame.  `hexturn.loft` here is the
game's names for them (476 lines to 70); `hextown.loft` keeps each house's turn and asks it for
every point that turns with the house.  The town regenerated is byte for byte the file it was.
**Not done:** the cells' window fitted to the turned cells (it changes the window other routines
read their reach from, so it is not an exact step), and `build` at two solves (it is at three for
an even turn and five for an odd one, whatever the openings; it was five and nine at three
openings and more with each).

### 7. The ground (M)

- 7a. `hex_place`: seating with the ground blended back round the footprint.  Its footprint half
  is `seat_height(SEAT_LOW)` and `seat_write`, which the new routine calls (HG-Seat).
- 7b. a new package, `hex_ground` (not `hex_terrain`: that name is the overland model, another
  thing): the ground as `Heights` and `Labels`, a corner at the mean of its three cells, the
  height at a point, terraces made smooth, the hill, digging along straight sides, cliffs and the
  height either side of one (HG-Surface, HG-Terrace, HG-Hill, HG-Dig, HG-Cliff).  The cliff is
  what `CONVERGENCE.md` lists as planned ("a wall whose cause is a height delta").

*Compared here:* every height of the regenerated ground, bit for bit.

### 8. The game on the libraries' own types (S)

After 7: `hexscene::Scene` keeps its ground in `hex_field::Heights` and `Labels` instead of flat
vectors with its own bounds tests; `centre` and `cell_near`, each written twice, become the
library's.  **This one changes output**: `centre` differs in the last bit and `cell_near` at ties,
so the phase regenerates `orbury.hex.json` and shows the difference (expected: last digits, and no
cell changing kind).  The point-in-polygon written five times and the distance to a stretch
written four times become one of each.

## Speed, and larger structures

The routines were written to be right first.  They are; they also do the same work several times
over, and nothing yet tests them on a structure larger than a house.  Both are settled as the
routines move, so a library gets the routine fast and tested at size, not a copy to mend later.

### Measured today

Native, this machine, a throwaway probe (2026-11-16).  A box `w × d` hex steps, alone in a window.

| Box | Cells | Boundary edges | Walls solved (`from_cells`) | Turned 60° | Turned 30° |
|---|---|---|---|---|---|
| 5 × 4 | 27 | 38 | 0.56 ms | 0.20 ms | 0.95 ms |
| 10 × 8 | 95 | 78 | 0.99 ms | 0.34 ms | 1.4 ms |
| 20 × 16 | 389 | 154 | 2.1 ms | 0.97 ms | 3.8 ms |
| 40 × 32 | 1 499 | 310 | 6.2 ms | 4.5 ms | 11.8 ms |
| 80 × 64 | 5 877 | 614 | 20.5 ms | 13.2 ms | 42.3 ms |

Nothing is quadratic in practice: the cost follows the cells of the window, about 3 µs a cell for a
solve and 7 µs for an odd turn.  A solve of 614 edges should cost what its edges cost, not what the
5 877 cells inside them cost.  The waste is in three places:

| What | Measured | Why |
|---|---|---|
| `hexhouse::build`, 3 openings | 2.4 ms at an even turn, 4.2 ms at an odd one | 5 and 9 wall solves: `place` solves the walls again for each opening, `turn_data` solves the cells `turn_full` just solved |
| `hexturn::turned_point` | 0.85 ms a point (odd), 0.12 ms (even) | the whole turn done again for an offset that is one per structure; `hextown::with_house` asks it per yard end, court corner, inner wall end and paved tile's corner — most of `hextown::port`'s 0.92 s |
| the 3D view's card heights (`render3d::cards_of` → `hex_y`) | **52 ms a frame** | 25 × 25 tiles × 5 layers asked afresh every frame: `hexscene::house_at` 10.8 µs (every house's outline, no box first), `height_at` 4.1 µs (nine centres, an `atan2`, hash reads).  The answers do not change while the place does not |

Read, not measured (the mesh build needs a GL context; the split below makes it measurable): a
change of day and night builds every chunk again, the light being in the vertices; every chunk
tests every fence and canopy with a `cell_near`; `hex_cell` builds nested vectors for cutting a
triangle no side crosses; a corner's light is worked out once for each of its three cells;
`in_frame` takes a cosine and a sine per cell of one fixed angle; `BY_OUTLINE` tries 289 outlines
against every cell (37 ms for a 5 × 4 box).

### The work, by phase

Each is checked as every phase is: the old routine and the new give the same answer, exactly, on
the game's shapes and on the larger ones below.  A faster routine that answers differently is a
different routine.

- **In phase 4 (walls).**  The boundary found by walking it from a first edge, not by scanning the
  window; `merged` in one pass round the loop instead of regrouping it for each short run;
  `common` by counting once; `axes_of` and `headings` sharing one reading of each edge's travel.
  *Aim:* a solve's cost follows its edges — 80 × 64 nearer 2 ms than 20.
- **In phase 5 (openings).**  `place` takes the built walls (`Built`), so placing N openings is one
  solve.
- **In phase 6 (the turn).**  The turn is worked out once into a value — the turned cells, the
  offset, the walls of the drawing — and `turn_data`, `turned_point` and `place` read it.  The
  cells' window fitted to the turned cells (`reach_of` takes `dq + dr`, four times the area
  needed).  The frames' cosines and sines from the exact table (`hex_shape::box_cos`, `box_sin`),
  once.  `BY_OUTLINE` stays the comparison it is, unhurried.
  *Aim:* `build` with any number of openings is two solves; a turned point is a few multiplications.
- **Phase 9, in the game: the view's heights — done.**  (`render3d.loft`'s `HexStand`, kept per
  cell as it is first asked; `checks/view3d.loft` compares all 104 566 of Orbury's, kept against
  afresh: none differs, 39 ms against 431 ms.)  As planned:  A table per place of
  each tile's height on each layer, filled when the place's hex version is read and again when the
  place changes, read by `cards_of`; `house_at` by the cell's own label (a house's cells are marked
  already) instead of every outline.  *Compared:* the table against `hex_y` at every tile and layer
  of Orbury, exactly.  *Aim:* under 1 ms a frame.
- **Phase 10, in the game: the mesh build (M).**
  - 10a. The faces apart from their upload: the ground's cells, the cliffs, the houses, the fences
    as a routine that takes the scene and answers faces (`faces3d.loft`'s way), `render3d` only
    sending them to the GPU.  Nothing drawn changes; it is what lets a test and a bench reach them.
    *Compared:* every chunk's faces before and after, exactly.
  - 10b. Each fence's and canopy's chunk kept as each house's is; a triangle no side is near not
    cut; each corner's height and light worked out once and shared by its three cells.
    *Compared:* faces exactly as 10a's.
  - 10c. Day and night without building again: the light in the vertices as it is by day, dimmed
    when drawn.  **This one changes what is sent**, so it is seen in the view, not compared.

### Tests at size

Today's tests use houses of 7 to 60 cells (`tests/hexhouses.loft`).  The shapes below are added to
it, and go into the library's tests with each routine (phase 3 makes the cells, 4 the walls, 6 the
turn):

| Shape | Size | What it is there for |
|---|---|---|
| a hall | 40 × 32 steps, 1 499 cells | long walls: a run of 78 edges and more |
| a keep with wings | 6 rectangles, about 800 cells, 16 walls or more | many walls, reflex corners, walls that only just carry (HX-Size) |
| a cloister | a hall round a court | two loops (HW-Loops, HY-Court) |
| a town wall's ward | 80 × 64, 5 877 cells | the largest a place holds |
| a street | 120 cells long, 3 wide, one slanted stretch, one step | HW-Step and HW-Slant on a long outline |

For each, at all twelve turns:

- the rules that hold for a house hold: HT-Turn60 against `hex_field::cell_rot` cell for cell;
  HT-Shape (as many walls, each turning with the structure); HW-Clear (no centre on a wall);
  HT-Near (each wall within half a row of its turned line); HW-Miter;
- the cells an odd turn gains or loses stay within what the walls' moves allow (today's bound in
  `tests/hexhouses.loft`, scaled by the length of the walls, not by the area);
- twenty openings spread over its walls: as many after the turn, each on the wall it was on, within
  HO-Turn30's bound;
- turned back by the opposite turn from the drawing (HT-Drawing): the 60° steps return the cells.

And for the meshes, after 10a (`checks/view3d.loft` beside its present eight):

- a scene of 256 × 256 cells with 200 houses, 400 fences, a road across it and a cliff — made by a
  routine beside `hextown::showcase`, no file;
- the ground is whole: every side of a cell's fan is met by its neighbour's at the same two
  heights, across chunks too (no crack at a chunk's edge);
- a chunk built again alone gives the faces it gave in the whole build;
- a house's faces: no wall face where an opening is, every wall from its foot to its eaves, the
  roof on the walls (HY-Parts);
- the height table of phase 9 against `hex_y` at every tile.

**Speed is recorded, and guarded loosely.**  The benches are the record: each library's `bench/`
gains the routine at these sizes (its `bench.rs` twin with it), and `bench/` here gains the turn,
the solve and the mesh build.  A test's own guard is a ratio, not a time, so it holds on any
machine: four times the edges costs under eight times the solve; a turned point costs under a
hundredth of a turn; `build` with twenty openings costs under twice `build` with one.

## What the merge is for: round and eight-sided structures

The game is to get round towers and eight-sided walls — towers, bay windows (owner, 2026-11-16) —
and that waits on this merge: they are built from what the libraries hold and from what the game
holds, and the two must be one first.  **Phases 1 to 6 are that merge.**  The ground (7, 8) and the
mesh build (10) do not hold it up and can come after; phase 9 waits on nothing.

What each side brings:

| The libraries have | The game has |
|---|---|
| a round tower's cells by exact shells, its centre read back exactly, a door on it as a wedge (`hex_shape`: `arc_fill`, `arc_recover_centre`, `arc_shells_upto`, `arc_door_wedge`) | straight walls from cells alone, with corners and openings |
| an arc as a surface with its exact distance and normal (`hex_edge::surf_arc`), a door as an interval on it | openings of one width at a point on a wall, kept through a turn |
| the octagon's cells, its angle fixed (`hex_field::form_octagon`: eight sides do not fit six) | the turn at any 30° of anything whose cells carry its walls |
| walls in 24 directions, the twelve between being 1.1° off (`hex_shape::Wall`) | the mesh of a house's walls, parts and roofs |
| a cone and a dome as heights (`hex_roof`) | roofs as drawn faces, by part |

What the merge must leave room for, so it is not done twice — each a point for the phase named,
none a reason to build the tower there:

- **A wall that is not a straight one of twelve** (phase 4).  A structure's walls come back as a
  list of `WallSurface`, each straight along a heading.  A round wall is a centre and a shell; an
  octagon's slanted sides run at 45°, which is none of the twelve headings but one of the 24.  The
  built structure's walls are given a kind as they move (straight on a heading today), so an arc
  and an in-between wall join later without changing what a straight wall's callers read.
- **A structure too small for its cells to carry its walls** (phases 4, 6).  HX-Size sets such a
  thing apart: "a special object with its own rotation".  A tower and a bay are those.  The turn
  takes a pivot and hands back an offset; a round tower's turn is exact at every angle (its centre
  turns, its shell stays), and that is the same value with no offset — the turn's value (phase 6)
  is shaped so a structure can answer it by its own rule.
- **An opening on a wall that curves** (phase 5).  An opening is a centre and a width along its
  wall; along an arc that is an angle and a width, which `hex_edge` already has as an interval.
  `Opening` keeps the place along the wall, not two world points only.
- **A bay window** is today an opening drawn standing out (HY-Bay).  Eight-sided, it is a small
  structure of its own against a wall: the first user of the two points above.

New rules for these are written in `formal/hex-houses.md` when that work starts (a round wall, an
eight-sided one, their turn, an opening on each); this plan only keeps the way open.

## Later: natural curves, caves and cliffs

Not this plan's work (owner, 2026-11-16: for later): roads and paths that curve as they would in
the land, and caves and cliffs in natural forms.  Noted here for what it asks of the phases above:

- **A side that curves.**  A road's side is today a straight wall with no height from its cells
  (HR-Sides), and phase 4d moves that.  A curving road is what `hex_way` is for: a centreline of
  straights and arcs, its sides exact offsets of it (`track_arc`, `track_offset`).  So the wall's
  kind that phase 4 adds for a round tower (straight, arc) is the same one an outline's side needs,
  and the two ways of a road in "Differences" become one choice per road: from its cells where it
  is drawn in cells, from its centreline where it is drawn as a line.
- **A cliff that is not straight.**  Phase 7b's cliff is the straight sides of the cells of a
  level and up.  A natural one is a line through the same cells with its own shape; `hex_terrain`
  already draws such lines between terrain types (a fractal line, the steeper side claiming the
  band).  The cliff is kept as "the boundary of the cells of a level, and how its line is drawn",
  the second part open.
- **A cave** is the ground's own case of a cell set with a wall round it, the wall not straight
  and a roof over it.  Nothing above closes it off; nothing above builds towards it.

## What stays in the game

`hextown.loft` (the square town ported: yards, courts, wings, bays, out-houses — HY-*), the
`hex.json` the 3D view reads, `hexscene.loft`'s reading of it, and the drawing in `render3d.loft`.
They are this game's, built from an intent only this game has.

## Differences that stay differences

Recorded in each library's worked examples when its phase lands; no library law is changed for
them.

- **A box at 30°.**  `box_fill` at an odd rotation fills the rectangle as it lies; the turn (phase
  6) turns the drawn structure and keeps its walls.  They give other cells.  The turn is what a
  structure that must look like itself uses.
- **A sides wall's line.**  `surface_of` averages every edge of a side, the corner's with them;
  `solve_sides` reads the line off the connectors and outvotes the corner's.  4 cm apart.
- **A road's sides.**  `hex_way` keeps a centreline and offsets it; the game draws a road's sides
  from its cells (HR-Sides, the owner's ruling).  Both stay.
- **Openings.**  `place_opening` takes N edges at `t`; the game's takes a width at a point.

## Documentation

What this work must leave true outside this file:

- `fetch-libs.sh` and `README.md` (the libraries the game uses, the files table);
- `formal/README.md` (the check's directories) and `formal/hex-houses.md` (where it names the
  code: `hexwalls.loft`, `hexturn.loft`, `hexhouse.loft`, `hexterrain.loft`);
- `WORLD.md` § One game, square or hex, and the headers of the four hex files as they shrink;
- per library touched: `USAGE.md`, the guide in `docs/`, a worked example, the README, the version;
- `loft-libs-world/README.md` (a `hex_ground` row) and `CONVERGENCE.md` (the cliff, the turn);
- this file: each phase marked done with its commit, and removed when the last one is.

## Open for the owner

1. Is "the formal rules work" read rightly above (constraint 1)?
2. `hex_*` pull requests: merged as `tile_world`'s are (auto-merge on a green CI)?
3. Phase 9 (the view's 52 ms a frame) waits on nothing: do it first?
4. Homes: walls and openings in `hex_draw`, the turn in `hex_place` (with a new dependency on
   `hex_draw`), the ground in a new `hex_ground` — or a package of their own for the walls?
