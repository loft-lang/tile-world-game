<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# What rolls and what flows

What is unstable is a rule, not worked out from shapes (owner, 2026-11-16): **the same ground and
the same start give the same end, every time, and every end is a whole hex.**  These are the rules
of a thing rolling down the ground and of water running over it; the owner's words and what the
trials showed are in WORLD.md § One game, square or hex.  The code cites them (`@FR-<Name>`).

The code: `marbles.loft` (things rolling, in time), `water.loft` (where water goes), `game.loft`
`run_river` (the town's river), `fieldplay.loft` `carried` (water taking a body).  The ground is
hexes with a height each; a heading is one of the twelve (hex-houses.md); a unit of speed is worth
0.15 m of height.

## A thing rolling

```
  (UM-Hex)      A marble is one to a hex: each ends on a whole hex of its own, on open ground.
  (UM-Rest)     At rest it stays unless a free neighbour lies clearly lower (two units of drop,
                no less than the ground takes): then it sets off towards the lowest, the place's
                own side first among equals.
  (UM-Roll)     Rolling it turns one step of 30° towards a clearly steeper way down, then goes
                one hex: to the neighbour, or between two hexes through to the next full hex.
                Its speed takes the drop and gives the ground its part (2 a hex, 3 between
                hexes); with nothing left it stays where it is, at rest.
  (UM-Open)     It turns only towards a way that is open: not into what does not move, a marble,
                or a fence it was not heading for.
  (UM-Aside)    At what does not move, a fence or the ground's edge it goes aside — the side the
                place prefers, then the other — a unit slower; with both sides shut it comes
                straight back at half its speed.
  (UM-Strike)   Two marbles change motion: the one struck goes on with the striker's heading and
                speed less one, the striker takes what the other had (rest, if it was at rest).
  (UM-Fall)     Let fall, it is in the air the time its fall takes.  Landing on a slope (a
                neighbour a unit or more lower) it bounces off down it with a quarter of its
                fall as speed; on the flat it lies still.  Onto a marble it rolls off to the
                lowest free neighbour, the same way.
  (UM-Time)     Whichever marble is next to arrive acts next; of two at one moment, the one let
                fall first.  A hex is a marble's own from the moment it sets off for it; it is
                there after the time its way takes at its speed — as fast as a thing fallen the
                height its speed is worth.
  (UM-Fence)    A fence stands on the edges between hexes and takes no hex.  A marble does not
                pass it: to a neighbour, a fence on the edge between; between two hexes, a fence
                on the edge the way runs along, or the corner it leaves by or comes in by shut
                on both sides.
  (UM-Same)     The same hill and the same marbles end the same, hex for hex and millisecond
                for millisecond.
```

## Water

```
  (UW-Kept)     Water is not worked out all the time (owner): when the scene changes — a supply
                fuller, a lock opened, a wall fallen — it is worked out once, hex by hex, and
                kept: how deep it stands, which way it runs, how fast.
  (UW-Way)      From every hex water may be in, its way to an outlet is the way that climbs
                least, and of those the shortest: down a slope the way down; over flat ground
                and through a hollow the shortest way to where the water gets out.  So a pond
                has a way through it, to where it spills.
  (UW-Hollow)   A hex lower than the lowest way out of it holds standing water up to that way's
                height before any passes on.
  (UW-Supply)   Water goes from its source hex by hex down its way, across the bed's whole
                width, never further from the outlet; every hex it wets takes a unit of the
                supply, a hollow a unit more a unit of depth it fills.  Where the supply runs
                out the bed beyond is dry.
  (UW-Depth)    Water stands as deep as its hollow and, over that, the sheet the supply still
                there makes (to 0.6 m): a hex higher than the water's level by more than the
                sheet is a bank, dry, and a full river reaches up its banks.
  (UW-Speed)    Water has speed (owner): the drop to the next hex and a unit for every 40 of
                supply still there, to 12 — and where water passes at all never less than 1, so
                a pond a stream runs through keeps a little speed and what floats in still ends
                up below it (owner).
  (UW-Stale)    Water that was there before and is not reached by any supply stands still: no
                speed, no way.
  (UW-Start)    The town's river is its bed as the ground has it, kept; as the town starts it is
                given nothing, so only the pools that stand in its bed are wet, stale (owner:
                the initial situation).  Given water at its head it runs the bed to its outlet.
  (UW-Carry)    Water running at speed 3 or more takes a body in it along its way, as far as
                that speed goes in the time.  What refuses a step refuses that too.
  (UW-Wade)     A figure wades water up to 0.7 m deep and walks into none deeper; running water
                takes a body into deep water all the same.
```

**Not rules yet** (WORLD.md): which end of the town's river is its head (the bed is level; west is
a choice); anything in play that changes the supply; things that float carried as a rule of play
(`water::drift` only follows the way); water falling clear of a ledge; water under ground.

## Guarded by

| Rule | Test |
|---|---|
| UM-Hex, UM-Rest (settled), UM-Fall (all roll), UM-Fence (held) | `tests/marbles.loft` `test_fifty_marbles_on_the_hill` |
| UM-Same | `tests/marbles.loft` `test_the_hill_plays_the_same_every_time` |
| UM-Roll, UM-Time, UM-Strike, UM-Fall, UM-Fence, UM-Aside | `tests/marbles.loft` `test_the_rules_of_a_marble` |
| UW-Stale | `tests/water.loft` `test_with_no_supply_only_the_old_pools_and_stale` |
| UW-Supply, UW-Depth (banks) | `tests/water.loft` `test_the_supply_runs_out_down_the_bed` |
| UW-Way, UW-Speed, UW-Hollow (full) | `tests/water.loft` `test_a_full_river_runs_through_its_pond` |
| UW-Hollow (part full) | `tests/water.loft` `test_a_hollow_holds_the_water_until_it_is_full` |
| UW-Start, UW-Carry, UW-Kept | `tests/townfield.loft` `test_the_river_stale_as_it_starts_and_running_when_fed` |

UM-Open is guarded only by the hill coming to rest (it did not before the rule); UW-Wade has no
test of its own.
