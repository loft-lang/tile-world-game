<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Rooms on their own grids, hex ground between

The world is not one grid (owner, 2026-11-16): **the tile grid stays, on smaller scales — rooms,
corridors, roads — each at its own angle, and the hex ground lies between them.**  A figure moves
free through all of it; a thing always ends on a whole tile or a whole hex.  These are the rules
of how the two grids meet; the reasons and the owner's words are in WORLD.md § One game, square or
hex.  The code cites them (`@FR-<Name>`) where it keeps them.

The code: `gridroom.loft` sets a room down on the ground and moves a thing across it;
`fieldplay.loft` walks a figure over both; `game.loft` (§ A field with rooms, § A town on the new
rules) plays it.  Units: metres, x east, y north; a tile's side and a hex step are both 1.5 m; a
heading h (0..11) points h × 30° anticlockwise from east, even ones to a neighbouring hex, odd
ones between two (hex-houses.md).

## A room on the ground

```
  (RG-Grid)     A room is a square place of its own (its tiles, its layers, its rules) set down
                on the ground at one of the twelve angles: its tiles' x runs along that heading,
                its y a quarter turn clockwise of it.  Inside it nothing knows the angle: the
                room's rules play as on any square place.
  (RG-Ways)     A room's four ways are four of the ground's twelve headings, a quarter turn
                apart; so two of the four always run between hexes, whatever the room's angle.
  (RG-Under)    A hex whose centre lies in the room's rectangle is under the room: it is not
                open ground.  Every other hex is.
  (RG-Opening)  A room's ways out are found from the room itself: a tile of its rim a thing can
                stand on (or a door, shut as it may be), on a side that looks out of the room
                and that nothing stops a step on.  A walled room has its doorways, a terrace
                (floor to its rim, something low on the rim's edges) its gates.
  (RG-Pair)     An opening is paired with the hex before it when the room is set down, both
                ways: what goes out of the opening comes onto that hex, what goes back from that
                hex comes onto the opening's tile.  The room is moved by less than a hex so the
                hex before its first opening lies exactly straight out, one tile's length away.
  (RG-Part)     A room may be a part of a larger place (a house in a town's own map): its tiles
                are that place's, from its first tile on; its rules and its things are the
                place's.  A house's wings are parts on the house's own grid.
  (RG-Sky)      Of a part's ways out only those lead onto the ground that come onto a tile under
                the open sky and out of every part of every house: a way into a wing or into a
                bay window is not a door.
```

## Moving a thing

```
  (RG-Whole)    A thing always ends on a whole tile or a whole hex (owner).  Where it will end
                is decided when it is moved; what is seen is steered there.
  (RG-Inside)   Inside a room a thing is moved by the room's own rules, one tile, the mover
                stepping after it.
  (RG-Twelve)   On the ground a thing moves one of twelve ways (owner): an even heading to the
                neighbouring hex; an odd one between two hexes through to the next full hex.
  (RG-Aside)    A move between two hexes that is not full — the hex ahead taken, or a fence on
                the edge the way runs along — goes aside into one of the two it would pass
                between: the side the place prefers, then the other; with neither free it does
                not happen (owner: the tile rule of things pushed aside).
  (RG-Fence)    A fence stands on the edge between two hexes and takes no hex.  It stops the
                step to a neighbour across it.
  (RG-Through)  A thing on an opening's tile pushed the way the opening looks goes out onto the
                paired hex (straight out where that hex is; else aside); a thing on that hex
                pushed straight back in comes onto the opening's tile.  Nowhere else does a
                thing pass a room's rim.
  (RG-Time)     A move takes the time of its real length at one speed (owner): a tile or a
                neighbouring hex, 1.5 m, in 140 ms; between two hexes, 1.5 × √3 m, in 242 ms.
```

## What stands round a room, and what is seen

```
  (RG-Rim)      A room's rim is its sides in order, anticlockwise, each a tile's side that looks
                out: open (a way through), low (not crossed by a thing, seen over), or a wall.
                The sides close.
  (RG-Sight)    Whether one point sees another past a room is asked in the world's own space,
                along the straight line from eye to spot, by height: a wall tile stops it for its
                layer's height; an edge that stops sight (a wall, a hedge) as high as it is; a
                floor the line passes through.  A low wall and a low hedge stop a step, not
                sight.  The tiles the two ends lie on do not count.
  (RG-Layers)   A room has layers.  Where a step leads — onto the next tile, up or down stairs —
                is asked of the room's own rules, so a bridge is walked over and the way under it
                walked under, the same tiles on two layers.
  (RG-Hand)     A maze that is a puzzle is reached and is not walked by keeping one hand on the
                wall (owner: bridges over other walkways, against left- or right-handing it).
                `one_hand` walks it so; `reached` says whether there is a way at all.
```

## A figure walking

```
  (RP-Body)     A figure's body is a point in the world's space and moves any way, at the
                walking pace (a tile's length in 140 ms).  Which grid it is on follows from
                where it is: in a room's rectangle on that room's tiles, else on the ground.
  (RP-Step)     The body coming onto another cell takes the step there through the rules: onto
                a free hex; a thing on a hex pushed along the body's way (RG-Twelve); in at an
                opening from the hex before it; in a room onto the next tile by the room's
                rules; out of a room through an opening.
  (RP-Refuse)   A refused step leaves the body where it was: a room's wall from either side,
                what does not move, a fence, water too deep to wade.
  (RP-Door)     A figure uses an opening when the hex before it is open ground, whether or not
                that hex lies exactly straight out — which only a pushed thing needs (RG-Pair).
  (RP-Low)      A road and a terrace are left at any place (owner): a figure steps over what is
                low on a rim's side (a low wall, a low hedge) wherever the hex before it is open
                ground.  A thing is not pushed across it: it leaves by a gate (RG-Through).
  (RP-Clock)    The field has one clock.  The room the figure is in runs with it; a room it
                comes into is brought to the field's time.
```

## A town

```
  (RT-Town)     A place with a hex version is played with its outdoors as that version's ground
                (its paths, its stream, what stands on it, its fences) and every part of every
                house a room where and at the angle the hex version gives it (RG-Part, RG-Sky).
                A house's inside stays the town's own map.
  (RT-In)       The hero in at a house's door stands on the door's tile of the town's map, on
                the house's layer.
  (RT-Out)      A step out of a door goes straight onto the hex before it — not by a step onto
                the square map's own outdoors, where what stands is not on the ground.
  (RT-Spot)     A door of the town that leads to another place, and stairs down under the open
                sky, are spots on the ground: the hero coming onto that hex is taken there.
```

**Not rules yet** (WORLD.md): what stands outdoors in the square town on the ground; the ground's
heights to the rules; other figures crossing between grids; a pull across a door; a way out of a
room on a layer over its own; a corner walled on both sides against a move between hexes.

## Guarded by

`tests/gridroom.loft`, `tests/maze.loft`, `tests/fieldplay.loft`, `tests/fieldgame.loft` and
`tests/townfield.loft`, the room rules at each of the twelve angles:

| Rule | Test |
|---|---|
| RG-Grid, RG-Under, RG-Pair, RG-Rim (a tile's square) | `gridroom` `test_the_room_lies_on_the_ground_at_every_angle` |
| RG-Ways | `gridroom` `test_a_rooms_four_ways_are_among_the_twelve` |
| RG-Inside, RG-Through, RG-Twelve, RG-Time, RG-Whole | `gridroom` `test_a_rock_goes_out_through_the_door_and_comes_back` |
| RG-Aside, RG-Fence | `gridroom` `test_a_push_that_is_not_full_goes_aside` |
| RG-Opening (a gate), RG-Rim | `gridroom` `test_a_terrace_is_a_room_with_low_walls` |
| RG-Sight | `gridroom` `test_what_is_seen_past_a_rooms_walls_and_hedges`, `test_what_is_seen_over_and_under_a_bridge` |
| RG-Layers | `gridroom` `test_a_bridge_is_walked_over_and_under` |
| RG-Hand | `gridroom` `test_a_bridge_that_hangs_on_no_wall_beats_one_hand`; `maze` `test_the_maze_is_walked_and_not_by_one_hand` |
| RP-Body, RP-Step, RP-Clock | `fieldplay` `test_the_hero_walks_in_and_out_and_pushes_a_rock_out`, `test_from_one_room_to_another_over_the_ground` |
| RP-Refuse | `fieldplay` `test_walls_trees_and_fences_refuse_the_body` |
| RP-Low | `fieldplay` `test_a_terrace_is_left_at_any_place` |
| RP-Step in the game | `fieldgame` `test_the_hero_walks_the_common_into_its_rooms_and_out` |
| RT-Town, RG-Part | `townfield` `test_orbury_starts_out_on_its_ground` |
| RT-In, RT-Out, RG-Sky, RP-Door | `townfield` `test_in_at_every_door_and_out_again` |

RT-Spot has no test of its own.
