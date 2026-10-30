<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Interactions: what the rules do to each other

The rules in [DESIGN.md](DESIGN.md) are written one at a time; the game happens where they meet.
This file walks the pairs — built rules, designed ones, and the planned enemies — and sorts what
it finds:

- **Ruling** — the rules are silent, or say two things; the owner decides.
- **Fairness** — the rules answer, but the answer can hurt or trap the player in a way the
  screen did not warn of.
- **Puzzle** — an interaction worth a level built around it.

A finding moves to DESIGN.md (as a rule) once ruled, and out of this file.

## Time and motion

Rolling carts and blasts are the game's first things that move by themselves.  Everything else
waits for the hero.

1. **Settled: the game is real time** (owner; DESIGN.md § The principle).  Its fairness asks are
   items 2 and 3.
2. **Settled: a doomed save is allowed** (owner) and rolled back once (DESIGN.md § Guards, doors
   and wells; built).
3. **Ruling — a blast's moment** (proposed with real time: a ripple, 150 ms a link).  A bomb "ignites on impact" — no fuse — so a chain is
   instantaneous.  Either all of a chain resolves in one instant (then the order, farthest first,
   decides who lands where), or each bomb goes off a tick after the one that set it off (a
   visible ripple, and room to react).

## Carts, tracks and bombs

4. **Puzzle — the track hop.**  A bomb beside a cart throws it onto the next track (DESIGN.md).
   Two parallel tracks, one leading to the goal: the player must place the bomb on the right
   side, at distance 1, with the landing rail free.
5. **Ruling — the hero aboard when the cart derails.**  A cart past an open end derails into a
   fixed thing; the hero riding it stays on top (it is stood on) — or is thrown forward and
   hurt, like a passenger in a crash?
6. **Ruling — the hero aboard when a bomb hits the cart.**  At distance 1 the cart is blown off
   its track: does the hero ride the blow (hurt, carried), or is the hero struck separately and
   pushed its own way?
7. **Puzzle — the bomb on a cart** (proposed in DESIGN.md: it goes off when the cart is
   stopped).  A delivery puzzle: set the junctions so the cart reaches the wall to break, not the
   hero's own room.
8. **Puzzle — junctions as a memory.**  A lever beside two junctions throws both at once:
   coupled switches, where setting one route sets another wrong.
9. **Ruling — a lever through a wall.**  Measured: a lever throws a junction beside it even with
   a wall edge between them (2026-10-30).  A defect (levers should reach only through open
   sides), or a feature (a lever in one room working the points in the next)?
10. **Puzzle — a cart as a broom.**  A rolling cart shoves light items off its rails; over a
    ledge they fall to the layer below.  Sweeping sticks down to where the tar is.
11. **Fairness — linking at a junction.**  Two carts arriving on different branches link into one
    train across the junction; pushing that train then follows the junction's setting for the
    lead cart only.  Sensible, but a player may expect the train to split.  No level explains it
    (nothing is tutorialized): it must behave so every time, so the player learns it by seeing it.
12. **Puzzle — a derailed cart as a step.**  Derailing places a height-1 fixed thing: a step up
    to a table or a ledge, made where the track ends.  A bomb can make one anywhere beside a track.

## Bombs and everything else

14. **Ruling — the blast and held things.**  Does a blast knock the item out of the hero's hands
    (light, flung like any item)?  It would make a burning torch fly into the dark.
15. **Fairness — blast then fall.**  A blast pushes the hero; pushed off a ledge, the hero falls
    too.  The costs add (blast 3 + a two-layer fall 3 = all 6 health) — a kill nobody saw coming
    unless the ledge was in view.  Options: the blast's push stops at a ledge (as stones stop),
    or it is meant, and levels show their ledges.
16. **Ruling — the blast and layers.**  Light spreads within a layer; does the blast?  A bomb on a
    ledge: does it hurt what stands below, push items up out of a pit?  Puzzle either way: a pit
    as a blast shelter, or a blast through a hole.
17. **Ruling — the blast and fences, doors, trees.**  Fences stop pushed things but not light:
    does a flung item fly over a fence (it is light), a sliding stone stop at it?  Does a blast
    open a door, fell a tree (dropping sticks — a bomb as a harvester), break a fence?
18. **Puzzle — water.**  A stone blown into water: does it sink (gone), or fill the cell (a
    stepping stone — the classic)?  Items blown into water are lost (falling rules).
20. **Puzzle — arrows as remote hands.**  Arrows set off bombs (owner); if they also throw levers,
    a junction can be set from across a chasm.  (Ruling: do arrows throw levers?)

27. **Puzzle — lighting a room from afar.**  A blast lights the unlit torches in the sockets it
    reaches (owner): a bomb thrown into a dark room lights it — or an arrow sets off a bomb that
    does.  A bomb in water throws water around (owner): its blast douses the torches it reaches —
    the same bomb lights a room or puts it out depending on where it lands, and a dark room is
    made darker on purpose (to sneak past cyclopes).

## Light, fire and enemies

21. **Ruling — fire spreading.**  A tar barrel bursts into fire (proposed); a burning torch flung
    against a barrel, a table, a tree: does fire spread to wood?  A strong system (a fuse of
    barrels, a burning bridge), and a large one — decide its edges before building any of it.
22. **Puzzle — a light on a cart.**  A burning torch rests on a cart (items rest on carts): a
    moving light.  Sent down the track, it lights the far room; with enemies, it sweeps rats aside
    and draws bats along — a lure on rails.
23. **Ruling — darkness and the clear area** (open since the first evaluation).  The well's clear
    area counts enemies in sight; in the dark the player cannot see them.  With cyclopes, sight
    must mean something in the dark.  The cyclopes' design will settle it; until then a well in a
    dark room may refuse for a reason the screen does not show.
24. **Puzzle — stealth against a cart.**  A cart rolling past a cyclops group draws its eye (noise
    or light) away from the hero sneaking by.

## Small items and big things

25. **Puzzle — sticks from above.**  A tree on a ledge drops its sticks to the floor below
    (built); with tar below and the tree above, each torch costs a climb.
26. **Ruling — an item on a cart that derails** keeps its item (built); on a cart blown off its
    track, the item is lighter than the cart and is flung off ahead (designed) — so the same cart
    keeps its item in one crash and loses it in the other.  Consistent by weight; worth a
    sentence in DESIGN.md so a puzzle can rely on it.
