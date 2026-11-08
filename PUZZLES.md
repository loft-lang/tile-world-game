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
- Parts include **the positions of enemy lairs** and **navigation challenges**, not only things
  to find and use.
- **The point is to play the game oneself**, not only to check that it still works.  The puzzles
  need not be hard, nor unique each time, but they must **feel fresh**.
- **The depth sets the challenge.**  For the deeper levels a routine measures the challenge of a
  combination, and when there is too little it picks another combination of the relevant parts
  that gives more.  The early levels get the opposite: too much challenge is prevented.
- **The challenge is how exactly the hero has to move** to solve it.
- **And how easily the puzzle is read**: when parts of it are hidden from the start, that bumps
  up the challenge too.

**Everything below is proposed**: the format, the vocabulary, the measure, the numbers and every
example, for the owner to rule.  It is built from rules that exist (DESIGN.md, INTERACTIONS.md,
TEMPLATES.md); nothing is built.

## Why parts fit this game

- **A dungeon is a puzzle** (DESIGN.md § The principle), and **every rule holds everywhere, the
  same way every time**: that consistency is what lets two parts written apart combine.  A part
  never relies on a special rule; it relies on the world's rules, so what it does to its
  neighbour is what the world does.
- **The developer depicts, the game fills in** (WORLD.md § The developer's view).  A slot in a
  place is one more thing to depict: "a gate here that wants a light", "a lair near this way".
  The seed fills it, the editor shows what it chose, and a part pinned by hand is an adjustment
  kept apart, as a cell painted by hand is.
- **Roles, not kinds**, as kinds.json does for the rules ("the rules read these, never a name")
  and templates do for figures (the body and the weapon apart).  A part that needs *a step*
  takes a chair, a crate or a derailed cart; which one the scene offers is the seed's choice.
- **Real time, deterministic** (DESIGN.md § The principle), and **outside what the hero does,
  every creature keeps its routine** (WORLD.md).  So the world a hero sneaks through is known
  in advance, moment by moment: that is what makes the challenge measurable (below).
- **A test enforces it** (DESIGN.md): every scene the game can deal out has been solved by the
  base character, by the real rules, before it is dealt.

## The kinds of part

| Kind | Its challenge | From what is built |
|---|---|---|
| **puzzle** | find the roles and bring them, in the right order | the bridge cart (`first_climb`), tar and sticks, a barred wall and a bomb, the flotilla, a junction set by a lever or an arrow |
| **navigation** | cross a stretch: when to move and where | carts rolling across the way, a bat roost over it (the swoop), a narrow ledge over a drop, a hunter's line of fire, a dark stretch between lit sockets, a cyclops round |
| **lair** | where an enemy lives, and how far its life reaches | a rat nest, a cyclops post, a spider wall, a Kharn camp, a grey rat's run: its routine by the clock, its roam, its sight |

**A lair reaches beyond its box**, by its roam and its sight; those are its side effects.  The
same lair beside a navigation part makes the crossing tighter; beside a puzzle part it makes
fetching a role a sneak; a blast heard within 8 empties it for a while (TEMPLATES.md, *scared*),
which gives the scene the fact *nest emptied*.  Where a lair lies is the seed's choice among the
spots its part allows.

**Variants**: each part has variants the seed picks between: mirrored, a lair's spot, a round's
phase on the clock, a cart's start, which socket is lit, where the role lies.  Half the
freshness is here, at no cost to the check: a variant is checked as its part is.

## How a part is written

A part has:
- **its kind and mechanic**: puzzle, navigation or lair; the `@GM-` tag or the INTERACTIONS.md
  puzzle it is built on;
- **its footprint**: a box of cells and layers, drawn as a small place (the place file format,
  EDITOR.md § 2), with its intent where it has one;
- **its ports**: the openings on its edge where the hero, and moved things, come in and go out,
  each with a side, a layer and a height;
- **what it needs** (roles in), at which port, and whether a role is used up;
- **what it gives** (roles out), at which port;
- **its figures**: for a lair, its kind, the spots it may lie at, its routine; for any part, the
  figures that keep to its box;
- **its variants** (above);
- **its challenge**: one sentence, what the player has to see or do;
- **its solution**: the moves (`rules::play`'s words) from each entry port to each port it opens,
  for each kind it accepts for a role it needs (a navigation part's solution is found by the
  search below, not written);
- **its side effects**: what its solution or its figures do beyond its edge, with the reach (a
  blast's 3 cells, a blast heard within 8, a cart along its track, a lair's roam and sight);
- **how it is spoiled**: the moves that leave it unsolvable (the cart wedged against the rock in
  `first_climb`), and whether R or a well brings it back.

**The checks every part must pass alone** (as every template set must, TEMPLATES.md):
- it is solved, from each entry, with each kind it accepts, in each variant;
- without a role it needs, it does not open (the need is real);
- each way of spoiling it that it names does spoil it;
- its side effects stay within their stated reach;
- its challenge alone, measured (below), is recorded, so a slot can ask for a gentle part.

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
| **a cart on a track** | a cart freed, a lift (@GM-CartLift) | a delivery (INTERACTIONS 7), a broom (10), a light on rails (22), a decoy (24) |
| **a fact** (a junction set, a water level, a nest emptied) | a lever, a pump, a scare | whatever reads it |

Roles can **turn into each other** through the world's rules, and this is half of where the
complexity lies: a blast thrown into a dark room gives *a light*; the same bomb dropped into
water gives *a dark*; a cart derailed gives *a step* and takes away *a cart on a track*.  The
checker knows these turns as rules of their own, written once, never per part.

## Where the complexity lies: how parts meet

1. **A supply chain.**  One part gives what another needs.  The seed picks which giver and which
   taker, so the same scene asks for a craft (tar and sticks) on one seed and a bomb into a dark
   room on another.
2. **One resource, two uses.**  A single bomb and two walls; a single step and two ledges.
3. **Side effects across an edge.**  A blast in one part lights the sockets of the next within
   its reach, so a dark sneak becomes a lit one; a lair's roam crosses into a navigation part and
   narrows its windows; a blast scares a nest empty, opening a way for a while.
4. **Shared things.**  One track through two parts, a lever throwing junctions in both
   (INTERACTIONS 8), one body of water a pump drains in both.
5. **Spoiling across an edge.**  Solving one part first spoils another: water drained takes away
   the crossing a part was built around; a cart sent down one track wedges another's bridge.

## A scene: a frame with slots

A **scene** is a place drawn by hand (LOCATIONS.md) with slots in it:
- **a slot** has a box, its ports, and what it may hold: a kind, the roles it must give or take
  ("a gate on the way from the lift to the seam, wanting a light or a blast"), the mechanics and
  the figures allowed (`@GM-Flooded` only, in the flooded tunnels; no cyclops before the Old
  Seam);
- **the frame** is the rest: the hand-drawn ground, the doors, what the story fixes;
- **the scene's goal**: a cell, a layer, the chests, a door reached (as a level's goal now);
- **its depth**, which sets its band of challenge (below).

The place's intent already carries a `seed` (`levels/the_blasting.json`: `"seed": 53`).  The same
seed picks, for each slot, a part whose footprint and ports fit, its variant, and for each role a
kind the part accepts.

## Solvable: the scene in the abstract

A search over a small state: the regions reached (the frame cut at the slots' ports), the roles
held, counted (two bombs, one torch), and the facts (a junction's setting, the water's level, a
nest emptied, a socket lit).  Each puzzle part is a rule: from this port, holding these roles,
given these facts, it opens these ports, gives these roles, uses up these, sets these facts, and
has these side effects.  A navigation or lair part is a rule too: it opens its far port when the
search below finds a way through.  A breadth-first search answers:
- **solvable?**  The goal reached, and the plan: which part, in which order, with which roles;
- **every part needed?**  The search again with each part made a plain open way: when the goal
  is still reached, the part is decoration (allowed, but counted);
- **interactions used**: how many of the plan's steps take a role another part gave, or rely on a
  side effect from across an edge;
- **traps**: the states reached from which the goal is not, each with the step that led there,
  and whether R or a well brings it back (a doomed save is allowed, never a bricked game).

Then **the plan for real**: each part's solution and each way found through, joined by walks
between ports, played by `rules::play` on the place as filled in, and the goal asserted, as
`tests/first_climb.loft` does.  When the abstract says yes and the playback no, the rules made an
interaction the parts did not declare: the report names the part and the move.  Either a part's
contract is wrong, or it is a new entry for INTERACTIONS.md.

## The challenge: how exactly the hero has to move

The owner's measure.  Proposed way to take it, on the scene as filled in, by the real rules:

- **The winning set.**  While the hero is unseen and unhurt, the figures keep to their routines,
  and carts, bats and lights do what the clock says: the world does not depend on where the hero
  is.  So a search over *(cell, moment)* finds every cell and moment from which the hero still
  reaches the goal unhurt and unseen.  That is the winning set, exact by the rules.  Moments
  are the world's due times, a step taking its 140 ms.  A figure that answers the hero before it
  sees it (a bat's swoop aimed at the hero's cell) adds its state to the search.
- **Measured on the easiest way.**  A player takes the most forgiving way, so a scene is as hard
  as its most forgiving solution, not its first: the search picks the way whose tightest moment
  is widest.
- **Along that way**, each moment has:
  - **its window**: how much earlier or later (ms) the hero may make that move and still be in
    the winning set;
  - **its margin**: how many of the cells the hero might be in instead (beside, behind) are
    winning too;
  - **whether it is a safe wait**: a cell where the hero may stand as long as it likes and stay
    winning — a breath.
- **The challenge**, from those (the numbers for play to tune):
  - **the tightest window** (ms);
  - **the tight moments**: windows under 300 ms, about two steps;
  - **the longest run without a safe wait** (s): tight moments in a row are harder than tight
    moments with a breath between;
  - **the forced moves**: moments where one of the five moves (four steps, wait) is the only
    winning one.  A puzzle part with no clock in it counts here: an order that must be kept, a
    push that must not be made.

### How easily it is read

The second axis (owner, 2026-11-08): a puzzle whose parts are hidden from the start is harder
than one laid out in sight.  Proposed way to take it, on the same easiest way:
- **What the plan uses**: each thing a step of the plan acts on or needs (the role and where it
  lies, the lever and the junction it throws, the ledge, the socket, the round of the figure to
  slip past) and each thing it is for.
- **Seen from where it is first needed**: for each, is it in sight (`tile_world::line_of_sight`,
  in light the hero can see by) from the port where the hero enters the part, or from the scene's
  start?  Each one not is **hidden**.  The ways a thing is hidden, from mild to strong:
  - **in the dark**: seen once a light is brought;
  - **round a corner, or on another layer**: seen by walking there;
  - **cause and effect apart**: a lever whose junction is out of sight from it, a blast whose
    wall is in another room — each end seen, never both at once;
  - **only by its sound**: a cart heard, a nest heard, before either is seen;
  - **in another part**: the role lies in a slot far from the one that needs it.
- **The reading measure**: the hidden things, counted, each weighted by how it is hidden, and
  the longest way (cells) between a cause and its effect.

**Hidden is never unfair.**  The principle holds as before: danger is readable before it
arrives (DESIGN.md § The principle).  What may be hidden is *how the puzzle is solved*: where a
role lies, what a lever throws, where the way goes on.  What may never be hidden is *what hurts*:
a cart is heard before it arrives, a bat chirps before it swoops, a lair's figure is seen or
heard before it can see the hero.  The check tells them apart: a danger first met with less
warning than its tell (TEMPLATES.md: 400 ms for a blow of 2 or more) fails the scene in every
band.  And a hidden thing is always findable: the search makes sure the hero can reach a place
from which it is seen before it is needed.

A slip is measured as failing, though in play it only costs (DESIGN.md: hurt, rarely killed): a
slip in the early places costs a hit and a step back; deeper, more.

## The depth sets the band

Each scene's depth gives a band.  The routine **deals** a scene:
1. draw a seed, fill the slots, check it solvable (above);
2. measure its challenge;
3. in the band: keep it.  Too little or too much: change the combination where it misses — swap
   the part holding the tightest moment for a gentler one when too hard; when too easy, swap the
   gentlest part for a harder one of the same slot, move a lair's spot towards the way, or shift
   a round's phase.  For reading: put the role in a farther slot, turn a socket's torch out,
   choose the variant whose lever stands round the corner from its junction, or the opposite.
   Then measure again;
4. after a set number of tries, keep the nearest to the band.

Proposed bands, by LOCATIONS.md's order:

| Depth | Places | Tightest window | Tight moments | Longest run without a breath | Hidden | Traps |
|---|---|---|---|---|---|---|
| first | the village, the camp, the hidden path, the hillside | 600 ms or more | 1 at most | 3 s at most | 1 at most, only in the dark or round a corner | none |
| middle | the Old Seam's first and second levels, the flooded tunnels, the lake | 300 ms or more | 4 at most | 6 s at most | 3 at most | seen, undone by R |
| deep | the High Diggings, Stormcap | 150–300 ms | 3 or more | 4 s or more | 2 or more, one with cause and effect apart | seen |

Deep scenes have a floor and a ceiling: too little challenge is dealt again, and so is a window
under 150 ms, which asks more than the hero's moves can give (a dodge takes 100).

## Fresh, not unique

- **The seed is drawn at each new game**, so the author plays places whose layout they do not
  know.  It is written to the play log (`playlog.loft`), so any game, and any bug in it, can be
  dealt again.
- **The save remembers the parts met**, and the routine prefers, among the deals that land in the
  band, parts and variants not met lately.
- **Variants carry much of it**: the same part mirrored, its lair moved, its round on another
  phase, plays fresh without a new part written.
- **The story keeps what it needs**: a place whose layout the story fixes (LOCATIONS.md) is
  drawn by hand; slots go where it does not.
- **A place stays dealt** once entered: it keeps its parts for the rest of the game, as every
  place stays as the hero left it.

**Where the routine runs**: the same code either way.  Proposed to start: on the developer's
machine, a pool of a few hundred dealt seeds per place, each with its measure, kept in the place
file; a new game draws from the pool by band and freshness, so the Tanmatsu searches nothing, and
`tests/puzzles.loft` replays every pooled plan.  Later, when the routine is fast enough, dealt on
the machine at a new game.

## The sweep

`loft --lib libs tools/puzzles.loft PLACE [N]` deals the seeds 1 to N and prints a line each:

```
seed  parts (slot: part, variant)                  solv  plan  inter  tightest  tight  run    hidden  traps  band
   7  gate: dark room, cache: tar+sticks, lair: rats   yes      5      2    900 ms      0  1.2 s       1      0  first
  12  gate: blast wall, cache: bomb, lair: cyclops    yes      7      3    350 ms      3  4.8 s       2      1  middle
  13  gate: dark room, cache: bomb, way: cart loop    yes      4      1    220 ms      4  5.5 s       3      0  deep
  19  gate: blast wall, cache: tar+sticks, ...        NO       -      -         -      -      -       -      -  -
```

Seed 13 is a role turn at work: the cache gives a bomb, the gate wants a light, and the bomb
thrown into the dark room lights its sockets; the cart loop beside it makes it deep.

## Rules the parts keep, so they can be combined

- **A part acts within its box**, but for its stated side effects and their reach.
- **Things pass between parts only through ports**, along the ways the frame keeps open.
- **A part's figures keep to its box** (their roam, TEMPLATES.md § Shared templates), or are the
  scene's, declared in the frame.
- **Fights are outside the check.**  Whether a fight is won is TEMPLATES.md's checks, in the
  fight rooms; a part with a figure in it is measured for sneaking past or luring it, never for
  beating it.
- **Readable, never taught** (DESIGN.md § The principle).  What a part needs is seen in the world
  (a dark doorway, a barred wall, a ledge too high, a cart heard before it is seen), never
  written; a role the scene needs is reachable before it is needed — the search makes sure.
- **Fun, never lost.**  A trap the search finds must be seen to happen and undone by R or a well.

## For the owner to rule

1. **Settled by the owner's aim**: the seed is drawn at each new game.  Open: from a pool dealt on
   the developer's machine (proposed, to start), or dealt on the machine itself?
2. **What do parts fill?**  Slots in the hand-drawn places of LOCATIONS.md, or whole dungeons
   made of parts?  Proposed: slots first; the Old Seam's levels are where they pay most.
3. **The bands**: the measures (tightest window, tight moments, run without a breath, forced
   moves; hidden things and how they are hidden) and the numbers in the table.
4. **Two axes or one?**  Exactness and reading kept as separate columns of the band (proposed:
   a deep scene may be tight and open, or loose and hidden, both in band), or folded into one
   number?
5. **Being seen**: does it fail the measure, as being hurt does?  Proposed: yes for the measure
   (it asks how exact a clean pass is), though in play a sighting starts a fight, not an ending.
6. **Interactions asked for**: may a deep scene require a number of interactions between parts,
   as it requires a challenge?  Proposed: yes, as a column of the band.
7. **Story facts as roles** (a key from a person, a lever shown by a villager's hint): later,
   once the story's places are built.

## Building it (proposed, in BUILD.md's small steps)

- **P1 — the format.**  Parts as files in `levels/parts/`, a slot list in a place's intent; two
  existing rooms cut into parts (`first_climb`'s bridge and its climb; the torch room's craft)
  and one lair (the rat run), with their checks alone.
- **P2 — the abstract search**, with its tests: solvable, decoration, interactions, traps.
- **P3 — the playback**: a plan stitched from the parts' solutions and played by the rules.
- **P4 — the measures**: the search over (cell, moment), the easiest way, its windows, margins and
  breaths; what the plan uses and what of it is hidden from where it is first needed; the warning
  every danger gives.  Tested on hand-made rooms whose numbers are worked out by hand.
- **P5 — dealing**: the bands, the swaps, the pool, the save's memory of parts met.
- **P6 — the sweep**: `tools/puzzles.loft`, and `tests/puzzles.loft` replaying the pool.
- **P7 — a first scene** in the demo village: a house of three slots, dealt anew each game.
