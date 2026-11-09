<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Structures on the hex grid at any 30° angle

The hex grid work rests on one claim (owner, 2026-11-09): **a structure drawn in one direction can
be drawn at any of the twelve 30° angles and still look like itself.**  Half of those angles cannot
be exact — a 30° turn is no symmetry of the hex grid — so the claim is a set of rules about what is
kept exactly, what is approximated, and by how much.  These are the rules; the code cites them
(`@FR-<Name>`) where it enforces them, and `tests/hexhouses.loft` checks each one.

The code: `hexwalls.loft` draws the walls from a structure's cells, `hexturn.loft` turns a
structure, `hexhouse.loft` makes a house's cells from its plan.  Units are `hex_field`'s: a cell
centre is (2q + (r & 1), 3r) in doubled coordinates (k, m), at x = k·√3/2, y = m/2 world units; a
hex step is √3 units (1.5 m), one unit is 0.866 m.  A heading h (0..11) points h × 30°
anticlockwise from east; even headings are the grid's edge directions, odd ones its corner
directions (ROUNDTRIP §2.1).

## The model

```
  (HX-Cells)    A structure is a set of hex cells: each cell is inside it or outside it.  Its
                shape is nothing more; no plan and no angle is kept with it.
  (HX-Edge)     A thin wall is an edge between a cell inside and a cell outside: it lies between
                two hex centres, one on each side.  A wall costs no floor, and every cell is
                plainly in or out.
  (HX-Editor)   Turning is an editor step.  The world is drawn only from the cells (owner).
  (HX-Size)     A structure drawn this way is one piece without a courtyard, its walls along two
                axes at a right angle, and big enough for its cells to carry its walls: its cells
                show a direction (HW-Axes finds no tie) and each wall stands for at least two
                edges.  Anything smaller is a special object with its own rotation (owner).
```

**In words.** What the world knows of a house is which hexes are inside it.  The walls the rules use
are the edges between inside and outside, so a wall always sits between two hex centres and never
on one.  The editor may know more — a plan, an angle — but what it hands the world is cells.  A
hex marked in or out on its own, or a hut of seven cells, cannot carry straight walls; such things
are objects with a rotation of their own, not structures of cells.

## Drawing the walls from the cells

```
  (HW-Axes)     The two axes a structure's walls run along are read off its cells.  Each edge of
                the boundary votes for the heading nearest the way the boundary travels from two
                edges before it to two edges after it; the pair of axes with the most votes wins.
                A tie means the cells show no direction; only then a hint (the angle the
                structure was placed at) decides.
  (HW-Runs)     The boundary, walked with the structure on its left, is cut into runs, one per
                straight wall: each edge goes with the way the boundary travels round it, along
                one axis or the other, one way or the other.  A run shorter than two edges is the
                turn of a corner, not a wall, and joins the run before it.
  (HW-Tops)     A wall along an even heading is a TOPS wall, the wobble across the tops of a row
                of hexes.  Turned to run due east (an exact 60° turn of the lattice), every one of
                its edges has its midpoint on the wall's line; the line runs through the
                midpoint most of its edges share.
  (HW-Sides)    A wall along an odd heading is a SIDES wall, along the sides of hexes under each
                other.  Turned to run due north, every slanted connector between two upright sides
                has its midpoint on the wall's line; the line runs through the midpoint most
                connectors share (through the mean of all midpoints when a run has none).
  (HW-Straight) Every wall is straight and runs exactly along one of the structure's two axes:
                its direction is the heading's step, an exact integer vector.
  (HW-Miter)    Each wall meets the next at a right angle, exactly (the world dot product of their
                heading steps is 0), and the two are mitred where their lines cross, so the
                outline closes.
  (HW-Clear)    ⚑ Every hex centre is clearly inside or outside the drawn walls: a centre lies
                inside the outline exactly when its cell belongs to the structure, and no centre
                is nearer a wall's middle line than √3/4 unit (half the band between two rows),
                so none lies under a wall drawn no thicker than √3/2 (HS-Wall) (owner: the one
                thing that matters).
```

**In words.** The walls are not taken from a plan; they are read from the cells.  The boundary of
the cells is a zigzag of hex edges; a straight wall is laid along each straight stretch of it.
The two kinds of stretch are different patterns — a wobble across the tops of a row, or a line
down the sides of stacked hexes — and each has one exact line through it, which the edges' or the
connectors' midpoints mark.  An edge of the next wall caught in a stretch at a corner has a
midpoint of its own and is outvoted.  Because a wall line runs midway between the rows of centres
on either side of it, every centre is clearly in or out.

## Turning

```
  (HT-Pivot)    A structure turns about a cell centre by a whole number of 30° steps.
  (HT-Turn60)   A 60° step is exact: every cell is turned about the pivot, cell for cell (a 60°
                turn is a symmetry of the lattice, ROUNDTRIP X1).  The structure keeps every cell
                and its shape.
  (HT-Turn30)   An odd step of 30° is approximate, and never refused (owner: the turn is not an
                exact routine).  The structure's drawn walls are turned 30° and each is moved to
                the nearest line a wall can stand on in the turned grid: midway between two rows
                of hex centres, the rows 1.5 units (1.3 m) apart across an odd heading and √3/2
                units (0.75 m) apart across an even one.  Before that, per axis, the whole
                structure sits on its pivot or half a row beside it, whichever moves its walls
                least in total.  Every cell inside the moved walls is the turned structure's.
                Moving a wall to its nearest line changes no cell (the same centres lie on each
                side of it); it is where the wall stands.  The offset is what decides the cells.
  (HT-Near)     After an odd step, with the structure moved by its offset (none or half a row,
                per axis), every wall lies within half a row of its drawn place turned 30°.  So a
                gap between two walls changes by at most one row: 1.3 m across an odd heading,
                0.75 m across an even one.
  (HT-Shape)    The turned structure has the drawn one's walls: as many, each turning the same
                way into the next.  (For a structure whose parallel walls stand at least a row
                apart, HX-Size.)
  (HT-Drawing)  Turn from the drawing, not from an earlier turn: two odd steps are two
                approximations, and need not give what one exact 60° step gives.
```

**In words.** Turning by 60° moves every hex onto a hex, so nothing changes but the direction.
Turning by 30° cannot: the turned grid's rows fall between the old ones.  So the walls are turned
and each is set down on the nearest line between two rows; a room centred on a hex can only be an
odd number of rows wide, so the whole structure may first move half a row to let an even number
fit.  That choice is the whole of the approximation: once the structure sits where it does, the
cells inside its turned walls are fixed, and the walls drawn from them stand on the lines nearest
their old places.  It is what keeps a T a T: at 0° its arm, stem and arm are 3.75, 5.25 and 3.75
m wide, at 30° 3.9, 5.2 and 3.9 m; on its pivot instead, the stem could only be 3.9 or 6.5 m.

## Doors and windows

```
  (HO-Point)    In the data a door or a window is a material on wall edges, never a gap: its
                edges stay wall, and the walls are solved as if it were not there.  To the walls
                it is a point on its wall's line, its centre: the mean of its edges' midpoints,
                taken onto the line (owner).
  (HO-Width)    An opening is drawn at one width for its kind, on every wall of both kinds and at
                every angle — the width hexsizes.json gives its kind — centred on its point.
                The wall leaves room for it there and a mesh fills the room, so one mesh fits
                every opening of a kind (owner).
  (HO-Place)    The editor places an opening at a point: on the wall nearest the point, its
                centre where the point falls along that wall, kept clear of the wall's ends by
                half its width and the corner clearance (HS-Corner); it marks the edges of that
                wall whose
                midpoints, along it, lie within half its width of the centre, or the nearest edge
                when none does.  So it covers one edge or two.
  (HO-Clear)    An opening lies wholly on its wall: its drawn span stays between the wall's two
                corners, at least the corner clearance less the most its edges can move it
                (HO-Turn30's bound) from each — half a unit (0.43 m) with today's sizes.
  (HO-Turn60)   A 60° step turns an opening's edges with the cells, exactly.
  (HO-Turn30)   An odd step turns an opening's centre with the structure, its offset included
                (`hexturn::turned_point`), and places it again on the turned wall (HO-Place).
                Every opening is kept, kind by kind.  Along its wall it lands near its turned
                place (kept clear of the wall's ends): within the most the centre of the edges
                under it can be from a point — for an opening w wide on a wall whose edges'
                midpoints lie s apart (√3/2 unit along a tops wall, 3/4 along a sides wall), s/2
                when w < s, else max(r, s − r)/2 with r the remainder of w over s: (√3 − 1)/2
                unit (0.32 m) for today's one-unit doors and windows.  Across, it moves with its
                wall (HT-Near).  So the distance between two openings changes a little (owner).
```

**In words.** A door is wall in the data — a mark on a few of the wall's edges — so the wall is
straight and whole whatever doors it has.  What the wall gets from a door is a point, and the
drawing makes room for a mesh there, of one width for every door, whether the wall is a tops wall
or a sides wall and whatever the angle.  Placing a door marks the one or two edges under it; along
a tops wall the edges' midpoints lie 0.866 units apart, along a sides wall 0.75, so with a width of
one unit there is always an edge under it, and the centre the edges give is never more than
(√3 − 1)/2 unit from where the door was asked for.  Turning carries the door's point with the
structure and places it again; that is where the distances between doors change a little.

## Sizes

```
  (HS-File)     The measurements are read from hexsizes.json, in metres: each kind of wall's
                thickness (outer, inner, fence: each its own, owner), each kind of opening's
                width, the corner clearance.  The routines take them as given; changing them
                changes no routine (owner, 2026-11-09).
  (HS-Wall)     A wall is drawn no thicker than √3/2 unit (0.75 m), the band between two rows of
                hex centres, so no centre lies under it (HW-Clear).
  (HS-Corner)   The corner clearance — how far an opening's edge keeps from its wall's corner —
                is at least √3/2 unit (0.75 m).  Nearer a corner a wall's edges lie further apart
                (the corner takes one), and an opening there cannot land near its place: kept
                only half that, three of 384 measured openings landed 0.46 to 0.57 unit off.
```

`tests/hexhouses.loft` reads hexsizes.json and checks every rule with the sizes it holds, so a
change to the file is a change the tests judge.

## The ground

```
  (HG-Surface)  The ground is a height and a surface kind per cell; it is drawn as a smooth
                surface, each cell a fan of six triangles from its centre, each corner at the mean
                height of the three cells round it, so neighbouring cells never step.
  (HG-Terrace)  A stepped height map (the square world's layers) is made smooth: its lowest level's
                height, and for every step up a storey's rise across the step's edge, a
                hillside wide (a stream's bank, for a step up from below level 0, a bank wide),
                centred on it and shaped by a smoothstep.  Half a hillside or more from any edge
                the ground is flat at its level's storey, exactly.
  (HG-Hill)     A round hill is added as a cosine: its full height at its top, nothing at its foot
                and beyond, level at both.
  (HG-Seat)     A structure seated on the ground has its footprint at its floor's height, exactly,
                and the ground round it blends back to its own height over the blend's width from
                the footprint's edge; beyond that the ground is as it was.  Its walls go down to
                the lowest the ground was under it.
```

The measurements (a storey 2.6 m, a hillside 6 m, a bank 2.5 m, the blend 2.5 m) are in
hexsizes.json's "terrain".  A square place is ported by its intent (`hextown.loft`): one tile one
hex step, its terraces through HG-Terrace, its houses on the cells nearest their middles at their
floors' heights (HG-Seat), its doors where the square houses have them, its trees, fences, well and
bridges as things on the surface; `hextown.html` shows it in 3D, with a hill (HG-Hill) carrying a
house at every angle.

## The house's plan

```
  (HP-Fill)     A house at 0° is the cells whose centres lie in one of its plan's rectangles,
                tested exactly (no float) and inclusively, as hex_shape::box_fill does for one box.
  (HP-Zero)     The house at 0° stays as its plan gives it; only the turn changes (owner).
```

## Measured (2026-11-09)

Over boxes (5 × 4, 3 × 2, 2 × 2), two L's, a T and a U at the twelve angles (`hexhouses.html`):

- every wall straight on the structure's axes and every corner square, at every angle;
- no hex centre on the wrong side of the drawn walls, and none nearer a wall's middle line than
  0.43 units (√3/4), in any of the 40 houses the page shows;
- the 3 × 3 m box (seven cells) the one tie: the same cells at every angle;
- a T drawn at 30° and turned by 60° the same cells as the T turned from 0° by 90°; turned by 30°
  instead, 44 cells against 42, 8 cells apart (HT-Drawing).

## Guarded by

`tests/hexhouses.loft`, over boxes, two L's, a T and a U at all twelve angles (`loft --lib libs
--tests tests/hexhouses.loft`):

| Rule | Test |
|---|---|
| HT-Turn60 | `test_a_sixty_degree_step_turns_every_cell_exactly` |
| HT-Shape | `test_every_angle_keeps_the_walls_and_their_turns` |
| HW-Straight, HW-Axes | `test_every_wall_is_straight_along_the_axes_read_off_the_cells` |
| HW-Miter | `test_every_corner_is_square` |
| HW-Clear | `test_every_hex_centre_is_clearly_inside_or_outside` |
| HT-Near | `test_an_odd_step_moves_each_wall_less_than_half_a_row` |
| HT-Turn30 (the offset) | `test_a_t_keeps_its_proportions_at_thirty_degrees` |
| HX-Size, HW-Axes (the tie) | `test_the_smallest_box_shows_no_direction` |
| HT-Drawing, HX-Cells | `test_a_structure_drawn_at_thirty_degrees_turns_from_its_cells` |
| HP-Fill, HP-Zero | `test_a_box_at_zero_degrees_is_the_box_fill` |
| HO-Width | `test_every_opening_has_its_kinds_width` |
| HO-Point, HO-Place | `test_an_opening_is_marked_edges_and_leaves_the_walls_alone` |
| HO-Clear | `test_every_opening_stands_clear_of_its_corners` |
| HO-Turn60 | `test_a_sixty_degree_step_turns_openings_exactly` |
| HO-Turn30 | `test_an_odd_step_moves_an_opening_little_along_its_wall` |
| HO-Place, HO-Clear (at a corner) | `test_an_opening_asked_for_at_a_corner_is_kept_clear_of_it` |
| HS-File, HS-Wall, HS-Corner | `test_the_sizes_file_keeps_the_rules` |
| HG-Terrace | `tests/hexground.loft`: `test_terraces_are_flat_away_from_their_edges`, `test_a_terrace_edge_is_a_smooth_slope` |
| HG-Hill | `tests/hexground.loft`: `test_a_hill_is_level_at_its_top_and_its_foot` |
| HG-Seat | `tests/hexground.loft`: `test_a_seated_house_is_level_and_blends_in` |

Measured for HO-Turn30: a door and a window at every half step along every wall of five boxes
(384 openings, corners included), turned 30°: the worst 0.317 units along the wall, under the
bound of 0.366.  Kept only half a wall's thickness from the corners, three places came out at
0.46 to 0.57, all within about one unit of a corner: what HO-Clear's full thickness is for.

Each was seen to fail (2026-11-09) when the code broke its rule: the axes not read off the cells
(five tests fail), a 60° step turned one step too far (two), the structure never moved half a
row (the T's); for the openings, a width taken from the count of edges (three), the offset not
carried to the openings (one), no clearance from the corners (one), a 60° step turning the marked
edges too far (two).  Leaving the walls where they are instead of moving them to their nearest lines
fails nothing, and should not: it changes no cell (HT-Turn30).

## Not covered

- **Walls shorter than three hex steps may be lost.**  The runs are cut where the boundary's way,
  read over two edges either side (HW-Runs), turns; a notch only two steps wide and deep can be
  read as a slope instead.  Measured on L's: every wall of three steps (4.5 m) or more kept, at both
  angle families; of the L's whose inner corner has two-step walls, a 4 × 2 body with a 2 × 2 or a
  3 × 2 wing lost them at every angle, a 5 × 3 body with a 3 × 3 wing at 30° only, and three others
  kept them.  Until the runs are read better, HX-Size's "big enough" means walls of three steps.

- a courtyard (a structure with a hole), inner walls, and walls not at a right angle;
- mirroring (an exact lattice reflection, ROUNDTRIP X2, not yet a routine of `hexturn`);
- the count of cells: an odd step keeps the walls near their places (HT-Near), not the count —
  a U loses 6 of its 55 cells at 30°, its 4.5 m arms becoming 3.9 m.
