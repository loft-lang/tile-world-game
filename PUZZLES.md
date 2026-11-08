<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Puzzle parts: scenes made of interchangeable pieces

What TEMPLATES.md is for figures, this is for puzzles.  The owner's idea (2026-11-08):
- Puzzles are made of **parts**: each a small challenge in itself, and each interchangeable with
  another similar part.
- The complexity lies in **how the parts chosen for a scene interact** with each other, not in
  any one part.
- **A world seed picks the parts.**  Several seeds can be tried, and an algorithm checks whether
  the parts together are solvable.

**Everything below is proposed**: the format, the vocabulary, the check and every example, for the
owner to rule.  It is built from rules that exist (DESIGN.md, INTERACTIONS.md); nothing is built.

## Why parts fit this game

- **A dungeon is a puzzle** (DESIGN.md § The principle), and **every rule holds everywhere, the
  same way every time**: that consistency is what lets two parts written apart combine.  A part
  never relies on a special rule; it relies on the world's rules, so what it does to its
  neighbour is what the world does.
- **The developer depicts, the game fills in** (WORLD.md § The developer's view).  A slot in a
  place is one more thing to depict: "a gate here that wants a light".  The seed fills it, the
  editor shows what it chose, and a part pinned by hand is an adjustment kept apart, as a cell
  painted by hand is.
- **Roles, not kinds**, as kinds.json does for the rules ("the rules read these, never a name")
  and templates do for figures (the body and the weapon apart).  A part that needs *a step*
  takes a chair, a crate or a derailed cart; which one the scene offers is the seed's choice.
  That is where the interchanging happens.
- **A test enforces it** (DESIGN.md): every kept seed carries a solution played by the base
  character, as `tests/first_climb.loft` does for a level.

## How a part is written

A part has:
- **its mechanic**: the `@GM-` tag or the INTERACTIONS.md puzzle it is built on;
- **its footprint**: a box of cells and layers, drawn as a small place (the place file format,
  EDITOR.md § 2), with its intent where it has one;
- **its ports**: the openings on its edge where the hero, and moved things, come in and go out,
  each with a side, a layer and a height;
- **what it needs** (roles in), at which port, and whether a role is used up;
- **what it gives** (roles out), at which port;
- **its challenge**: one sentence, what the player has to see or do;
- **its solution**: the moves (`rules::play`'s words) from each entry port to each port it opens,
  for each kind it accepts for a role it needs;
- **its side effects**: what its solution does beyond its edge, with the reach (a blast's 3, a
  blast heard within 8, a rolling cart along its track);
- **how it is spoiled**: the moves that leave it unsolvable (the cart wedged against the rock in
  `first_climb`), and whether R or a well brings it back;
- **its weight**: how hard it is alone, 1 to 3, so a scene can ask for an easy or a hard slot.

**The checks every part must pass alone** (as every template set must, TEMPLATES.md):
- its solution, from each entry, with each kind it accepts, opens what it says it opens;
- without a role it needs, it does not open (the need is real);
- each way of spoiling it that it names does spoil it, and nothing else listed as a solution does;
- its side effects stay within their stated reach.

## The roles: what passes between parts

A role is what one part gives and another needs.  Proposed, drawn from what is built:

| Role | Given by (built mechanics) | Needed by |
|---|---|---|
| **a way** | any part, at the port it opens | the next part along |
| **a step** (a movable thing to stand on, height 1) | a chair, a crate, a cart derailed into a fixed thing (INTERACTIONS 12) | a ledge one too high, a table climb |
| **a light** (a burning torch held, or a socket lit) | tar and sticks crafted (DESIGN.md § Torches), a lit socket to take from, a blast that lights sockets (INTERACTIONS 27) | a dark room, a room whose sockets must be lit |
| **a dark** (a room's torches out) | a bomb in water douses (INTERACTIONS 27), a torch taken down | sneaking past cyclopes |
| **a blast** (a bomb, carried or found) | a bomb cache, a cyclops baited to throw (INTERACTIONS 29) | a barred wall, a cart hop (INTERACTIONS 4), a scare (28) |
| **a remote hand** (something that acts from afar) | the bow and its arrows (INTERACTIONS 20) | a lever across a chasm, a bomb out of reach |
| **a crossing** (over water or a gap) | a cart pulled off a bridge (`first_climb`), a flotilla (@GM-Flotilla), a pump's lowering (@GM-Flooded) | the far bank |
| **a cart on a track** | a cart freed, a lift (@GM-CartLift) | a delivery (INTERACTIONS 7), a broom (10), a light on rails (22) |
| **a fact** (a junction set, a water level, a nest emptied) | a lever, a pump, a scare | whatever reads it |

Roles can **turn into each other** through the world's rules, and this is half of where the
complexity lies: a blast thrown into a dark room gives *a light*; the same bomb dropped into
water gives *a dark*; a cart derailed gives *a step* and takes away *a cart on a track*.  The
checker knows these turns as rules of their own, written once (below), never per part.

## Where the complexity lies: how parts meet

The parts are simple; a scene is interesting when they touch.  The ways they touch:

1. **A supply chain.**  One part gives what another needs.  The seed picks which giver and which
   taker, so the same scene asks for a craft (tar and sticks) on one seed and a bomb into a dark
   room on another.
2. **One resource, two uses.**  A single bomb and two walls; a single step and two ledges.  The
   order matters, and the scene is solvable only one way round.
3. **Side effects across an edge.**  A blast in part A lights the sockets of part B within its
   reach, so B's dark stealth becomes a lit fight; a blast heard within 8 scares the rats out of
   part C's nest (INTERACTIONS 28), clearing it for a while.  The parts were written apart; the
   world's rules join them.
4. **Shared things.**  One track through two parts, a lever throwing junctions in both
   (INTERACTIONS 8), one body of water a pump drains in both.
5. **Spoiling across an edge.**  Solving part B first spoils part A: water drained in B takes
   away the crossing A was built around; a cart sent down B's track wedges A's bridge.

The checker's job is to find which of these a seed's parts set up, and whether the scene is
still solvable with them.

## A scene: a frame with slots

A **scene** is a place drawn by hand (LOCATIONS.md) with slots in it:
- **a slot** has a box, its ports, and what it may hold: a weight, the roles it must give or
  take ("a gate on the way from the lift to the seam, wanting a light or a blast"), the
  mechanics allowed (`@GM-Flooded` only, in the flooded tunnels);
- **the frame** is the rest: the hand-drawn ground, the doors, what is fixed;
- **the scene's goal**: a cell, a layer, the chests, a door reached (as a level's goal now).

The place's intent already carries a `seed` (`levels/the_blasting.json`: `"seed": 53`).  The same
seed picks, for each slot, a part whose footprint and ports fit, and for each role a kind the
part accepts.  The editor shows the part chosen in each slot; one pinned by hand is kept apart.

## The check: three levels

**1. Each part alone**: the part's checks (above), once, when the part is written.

**2. The scene in the abstract.**  A search over a small state:
- the regions reached (the frame cut at the slots' ports),
- the roles held, counted (two bombs, one torch),
- the facts (a junction's setting, the water's level, a nest emptied, a socket lit).

Each part is a rule: from this port, holding these roles, given these facts, it opens these
ports, gives these roles, uses up these, sets these facts, and has these side effects on the
regions within reach.  The turns between roles (a blast in a dark room gives a light) are rules
too, written once.  A breadth-first search answers:
- **solvable?**  The goal reached, and the shortest plan: which part, in which order, with which
  roles;
- **every part needed?**  The search again with each part made a plain open way: when the goal
  is still reached, the part is decoration (allowed, but counted);
- **interactions used**: how many of the plan's steps take a role another part gave, or rely on a
  side effect from across an edge — the measure of the owner's "complexity in how the parts
  interact";
- **traps**: the states reached from which the goal is not, each with the step that led there,
  and whether R or a well brings it back (DESIGN.md: a doomed save is allowed, never a bricked
  game).

**3. The scene for real.**  The plan made concrete: each part's solution from its entry port,
joined by walks between ports (`tile_world::shortest_path`), moved things taken along between
ports by the ways the slots declare, played by `rules::play` on the place as filled in, and the
goal asserted, as `tests/first_climb.loft` does.  The world's clock is deterministic, so the
playback is exact.  **The abstract says yes and the playback no**: an interaction the rules
make that the parts did not declare.  The report names the part and the move where it went
wrong.  Either a part's contract is wrong, or it is a new entry for INTERACTIONS.md — the check
finding puzzles as a side effect of finding faults.

## Trying seeds

A tool, as the drawing kit's are: `loft --lib libs tools/puzzles.loft PLACE [N]` tries the seeds
1 to N and prints a line each:

```
seed  parts (slot: part)                         solvable  plan  interactions  traps  decoration
   7  gate: dark room, cache: tar+sticks           yes        5         2          0       0
  12  gate: blast wall, cache: bomb, side: lever   yes        7         3          1       0
  13  gate: dark room, cache: bomb                 yes        4         1          0       0
  19  gate: blast wall, cache: tar+sticks          NO         -         -          -       -
```

Seeds that pass are kept in the place file, each with its concrete plan; `tests/puzzles.loft`
plays every kept seed's plan, so a rule changed elsewhere that breaks a scene fails a test.
Seed 13 above is the role turn at work: the cache gives a bomb, the gate wants a light, and the
bomb thrown into the dark room lights its sockets.

## Rules the parts keep, so they can be combined

- **A part acts within its box**, but for its stated side effects and their reach.
- **Things pass between parts only through ports**, along the ways the frame keeps open.
- **A part's figures keep to its box** (their roam, TEMPLATES.md § Shared templates), or are the
  scene's, declared in the frame.  The playback then plays them on the clock as they are.
- **Fights are outside the check.**  Whether a fight is won is TEMPLATES.md's checks, in the
  fight rooms; a part with a figure in it is checked for sneaking past or luring it, never for
  beating it.
- **Readable, never taught** (DESIGN.md § The principle).  What a part needs is seen in the world
  (a dark doorway, a barred wall, a ledge too high), never written; and a role the scene needs
  is given somewhere the hero can reach before it is needed — the search makes sure.
- **Fun, never lost.**  A trap the search finds must be seen to happen (the cart wedged, in
  sight) and undone by R or a well.

## For the owner to rule

1. **When is the seed chosen?**  (a) at authoring: the seed picked from the sweep, the place
   shipped fixed with its test; (b) at each new game, from the place's list of kept seeds, so a
   second play differs; (c) at each new game, any seed, checked on the machine.  Proposed: (b) —
   the variety of (c) with the certainty of (a), and nothing to search on the Tanmatsu.
2. **What do parts fill?**  Slots in the hand-drawn places of LOCATIONS.md, or whole dungeons
   made of parts?  Proposed: slots first; the Old Seam's levels are where they pay most.
3. **Traps allowed?**  `first_climb` has one on purpose.  Proposed: allowed when seen and undone
   by R; the sweep counts them so a scene's seed can be chosen with fewer.
4. **Does a scene ask for interaction?**  Proposed: a slot may say "takes a role from another
   slot", and a seed whose plan uses fewer interactions than the scene asks for is not kept.
5. **Story facts as roles** (a key from a person, a lever shown by a villager's hint): later,
   once the story's places are built.

## Building it (proposed, in BUILD.md's small steps)

- **P1 — the format.**  Parts as files in `levels/parts/`, a slot list in a place's intent; two
  existing rooms cut into parts (`first_climb`'s bridge and its climb; the torch room's craft) and
  their checks alone.
- **P2 — the abstract search**, with its tests: solvable, decoration, interactions, traps, on
  scenes written by hand.
- **P3 — the playback**: a plan stitched from the parts' solutions and played by the rules.
- **P4 — the sweep**: `tools/puzzles.loft`, and kept seeds played by `tests/puzzles.loft`.
- **P5 — a first scene** in the demo village: a house of three slots, its sign naming the seed.
