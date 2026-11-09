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

For an outline of any number of loops — a structure with a courtyard, a road network with the
blocks it goes round — and with stretches in more than two directions (`hexwalls::outlines`):

```
  (HW-Loops)    Every loop of the boundary is walked, each with the structure's cells on its left:
                the outer loop of each piece anticlockwise, the loop round each hole in it (a
                courtyard, a block a ring of road goes round) clockwise.  A hex boundary never
                pinches (each two of the three cells at a corner are neighbours), so each corner
                starts one edge and the walk is never in doubt.
  (HW-Step)     A run is cut where the line it runs along steps over by a row (the midpoint every
                edge marks along a tops wall, every slanted connector along a sides wall, moves to
                another row for two edges or more); a part of fewer than three edges at a run's
                end is a corner's edges caught there and joins its neighbour.  Two walls in a row
                along the same axis are joined by a short wall across, of the other family,
                through an edge of the step that marks that family's line, so every corner is a
                right angle and every line lies between two hex centres.
  (HW-Slant)    A stretch of boundary that keeps to a heading off the loop's two axes for six
                edges or more (SLANT) is a wall along that heading: a road's slanted spur.
                Shorter, it is the bevel of a step and stays on the axes.
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
  (HG-Dig)      A structure dug into the ground (a stream's bed) lies `depth` below the ground round
                it, across a bank a bank wide centred on its straight sides (HW-Loops, HW-Step,
                HW-Slant: the sides the view draws its edge along) and shaped by a smoothstep of
                the distance to them.  Half a bank or more inside its sides the ground is `depth`
                down, exactly; half a bank or more outside it is as it was.  So a bank runs along
                the sides, not the cells' zigzag or the square map's staircase; Orbury's stream is
                dug so (its bed the square map's squares below level 0, the dry ground made smooth
                from level 0 up by HG-Terrace).
  (HG-Hill)     A round hill is added as a cosine: its full height at its top, nothing at its foot
                and beyond, level at both.
  (HG-Seat)     A structure seated on the ground has its footprint at its floor's height, exactly,
                and the ground round it blends back to its own height over the blend's width from
                the footprint's edge; beyond that the ground is as it was.  Its walls go down to
                the lowest the ground was under it.
```

## Fronts: slopes and cliffs

Owner, 2026-11-09: where a slope is drawn the hero can walk it; where the ground does not let the
hero through, it shows a clearer edge than a slope.  Not terraces all through a town: most fronts
slope, a few are cliffs.

```
  (GM-Front)    A terrace's front — where the ground steps up a layer from one cell to the next,
                both open to the sky, outside any house, a tree or a bush on either counted as the
                ground under it (it stands in the way, the ground still slopes) — is crossed as
                the place says (its
                intent's `fronts`).  In a place whose fronts slope, each such front carries a
                `slope` edge on both layers it stands between (`editor::refill_fronts`, after the
                cells are filled), and the hero walks up it onto the layer above and down it
                without a fall; the fronts with a cell in one of the place's cliff areas (`cliffs`)
                and every front of a place that says nothing stay cliffs: climbed only by stairs
                and slopes, and a fall off them costs health (DESIGN.md § Falling).  Creatures walk
                the sloping fronts as the hero does when they chase the hero, go home, keep to a
                route or a goal (`enemy_goes`, their ways over layers), not when they wander; they
                see the hero a layer up or down when both stand under the open sky, and a creature
                and the hero beside each other across a sloping front strike each other there.
                A thing pushed or pulled across it goes up or down onto the ground there
                (`over_front`), the hero walking after it, from the foot, the top or the front
                itself, and an item in the way slides over it; nothing is pushed up or off a
                cliff.  The 2D view draws a sloping front's face as
                a bank of the ground above it (grass, a road's path), darker toward its foot; a
                cliff's face stays the terrace's stone.
  (HG-Cliff)    On the hex ground a front slopes as HG-Terrace's smooth hillside, except in a
                cliff area: there each cell stands at its level's storey, blending back to the
                smooth ground over the blend's width beyond the area, and a cliff runs along the
                straight sides of the cells of each level and up (HW-Loops, HW-Step, HW-Slant),
                cut to the area.  Either side of a cliff is its own surface, its corners the mean
                of the cells on that side only, and a face stands on the cliff's line from the low
                side's surface up to the high side's, so the ground never slopes down a cliff.
                Who stands by a cliff stands on the surface of the side their layer is on.
```

Orbury (2026-11-09): every front slopes but those of the big house's terrace — its front over the
market and the hill behind it, the area x 20–39, y 8–21 — 357 sloping fronts (714 edges); its five
three-cell ramps are kept clear but no longer rise (a ramp beside a sloping front only stops the
hero walking onto it from the side), and the main road's landings are the road's two cells wide.
On the hex ground the big house's terrace carries 7 cliffs (43 stretches of at most 1.5 m), faced
with the terrace's earth as the square view draws it.

The measurements (a storey 2.6 m, a hillside 6 m, a bank 2.5 m, the blend 2.5 m) are in
hexsizes.json's "terrain".  A square place is ported by its intent (`hextown.loft`): one tile one
hex step, its terraces through HG-Terrace, its houses on the cells nearest their middles at their
floors' heights (HG-Seat), its doors where the square houses have them, its trees, fences, well and
bridges as things on the surface; `hextown.html` shows it in 3D, with a hill (HG-Hill) carrying a
house at every angle.

Measured on Orbury's stream (2026-11-09), the drawn surface sampled every 0.1 m along its sides,
where the ideal is half the depth (−1.3 m): the square map's banks gave a spread of 0.44 m, the
banks along the sides 0.25 m (mean −1.34 m; 1777 of 2255 points within 0.2 m).  What is left is
the grid's: a side between two rows passes through hex corners, each the mean of three cells, two
on one side and one on the other (HG-Surface), so the surface along it wobbles up to 0.42 m.  A
wider bank barely helps (0.21 m at 3.5 m, 0.18 m at 5 m); heights kept per corner would.

## Roads, rails and the stream

```
  (HR-Sides)    A road, a rail and a stream are structures like a house: cells inside and out
                (the path cells; the rail cells; the stream's bed and pools), their sides walls
                with no height (owner, 2026-11-09), drawn by the same routines from the cells
                (HW-Loops, HW-Step, HW-Slant) when a place loads (`hexscene.loft`).  The 3D view
                cuts the ground along those sides and gives each piece its surface by the side it
                lies on, so a road's edge is a straight line, not the cells' zigzag.
```

Measured on Orbury (2026-11-09): its paths are 8 loops and 228 walls from 617 cells, its rail one
loop of 28 walls, its stream one of 54; every rail and stream centre is clearly in or out, and all
but four of the paths', every one of those four in a lane too narrow to carry walls (§ Not
covered).

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
| HG-Dig | `tests/hexground.loft`: `test_a_dug_bed_follows_its_sides` |
| GM-Front | `tests/moves.loft`: `test_a_sloping_front_is_walked_up_and_down`; `tests/worlds.loft`: `test_orburys_fronts_slope_but_the_big_house_stands_on_a_cliff`, `test_the_bed_is_left_up_its_town_bank`, `test_no_pocket_in_the_bed`, `test_every_front_outside_the_cliff_area_slopes`; `tests/enemies.loft`: `test_a_rat_chases_the_hero_up_a_sloping_front`, `test_the_hero_strikes_a_rat_across_a_sloping_front`, `test_a_rat_walks_home_down_a_sloping_front`; `tests/moves.loft`: `test_a_thing_is_pushed_up_and_down_a_sloping_front`, `test_a_thing_is_pulled_down_and_up_a_sloping_front`, `test_an_item_is_shoved_over_a_sloping_front` |
| HG-Cliff | `tests/hexground.loft`: `test_cliffs_only_in_their_areas_and_sharp`; `tests/hexroads.loft`: `test_orburys_cliffs_drop_a_storey_and_its_slopes_do_not` |
| HG-Hill | `tests/hexground.loft`: `test_a_hill_is_level_at_its_top_and_its_foot` |
| HG-Seat | `tests/hexground.loft`: `test_a_seated_house_is_level_and_blends_in` |
| HW-Loops | `tests/hexroads.loft`: `test_a_ring_is_two_loops` |
| HW-Step | `tests/hexroads.loft`: `test_a_band_stepping_over_a_row_keeps_its_steps` |
| HW-Slant | `tests/hexroads.loft`: `test_a_slanted_spur_keeps_its_heading` |
| HR-Sides, HW-Clear (roads) | `tests/hexroads.loft`: `test_orburys_roads_rails_and_stream_are_clear` |

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

- **A lane one tile wide running north, or a short slanted spur.**  A row of hex cells running
  north zigzags (each row is offset by half a cell), so a one-cell lane that way has no straight
  sides that keep every centre clear.  Four of Orbury's 617 path cells sit in such lanes (a house's
  door way down to the street, the stair road, the way off the stone bridge) and fall on the wrong
  side of the drawn sides.  A lane running north needs to be two cells wide in hex.

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
