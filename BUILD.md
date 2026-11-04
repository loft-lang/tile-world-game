<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Building the story game, in small safe steps

What STORY.md, LOCATIONS.md and DESIGN.md need built, as a sequence of steps that each change
one thing and leave the game green.  A design for the owner to steer (2026-11-04); nothing in it
is built yet.

## The rules every step keeps

1. **One thing a step.**  A step builds one mechanic, one system or one place, small enough to be
   one commit or a short series, and leaves every earlier test passing on both backends (the
   interpreter and native) and every game check (`checks/game.loft`) green.
2. **Rules first.**  A step starts from its rules in DESIGN.md.  Where they are still *proposed*
   (the numbers, the readings), the owner rules on them before the step is built; the step lists
   what it needs ruled.
3. **New mechanics live in the test world first.**  Each new mechanic gets a room of its own in
   the demo village (`levels/demo.json`, a house per room, its name on the ground), which names
   the mechanic in its `serves` (@GM-…) and carries a solution test played by the base hero
   (DESIGN.md § The principle).  The story world uses a mechanic only once its room plays.
4. **Each step says what goes red.**  The test or check that fails when the step is undone; a
   new guard is shown to fail on the code before the step (as the game's tests do today).
5. **Pictures stay the same unless the step changes them.**  A step that is not about drawing
   leaves the frames of the existing rooms byte-identical (TW_SHOT before and after).
6. **The tags stay in step.**  `loft tools/tags.loft check` passes after every step; a room or a
   place cites what it serves.

## Phase A — the tools to author with

Small editor steps that every later place needs; none changes the rules.

- **A1. Figures in the editor.**  Place and remove enemies and people (the kinds in kinds.json)
  from the palette, saved in the place file's `enemies`.  Red: `tests/editor.loft`, a figure placed,
  saved and loaded back.
- **A2. Doors and text in the editor.**  Mark a cell as a door to another place and door, and
  write text on the ground.  Red: a door made in the editor walked through in `checks/game.loft`.
- **A3. What a place serves.**  The `serves` list shown and edited in the editor's panel.  Red: a
  tag added in the editor, saved, and `tools/tags.loft check` reading it.

Talks stay written by hand in the place files for now; they read as a script already.

## Phase B — the new mechanics, each in a room of the test world

In an order where each needs only what comes before it.  Each: its rules (DESIGN.md), its room,
its tests on both backends.

- **B1. The weapons.**  Damage by weapon (sword and axe 2, torch 2 against bats and 1 otherwise,
  pick and bare hands 1), recovery by weapon (the axe slower), and reach (a held item 3 tiles, bare
  hands 1).  Room: *the armoury*, a sword, an axe, a torch and bats.  Ruled first: the axe's
  recovery, what the 3 tiles are.  Red: `tests/blows.loft` extended, a blow by each weapon timed and
  counted by hand.
- **B2. The pick, rubble and the cart's load.**  A rock cleared into a cart beside the hero; the cart
  full until bumped from the side at an unloading place; a full cart heavier.  Room: *the
  workings*.  Ruled first: a full cart's weight (proposed: two carts).  Red: a new
  `tests/rubble.loft`.
- **B3. The cart lift.**  A platform between layers carrying its stack, sent by a lever (or an
  arrow), moving only with its shaft clear, its rails joining the track at each stop.  Room: *the
  lift*.  Ruled first: its speed; riding it with no cart.  Red: a new `tests/lift.loft`.
- **B4. The pump and its crystals.**  A crystal is an item that cannot be harmed (a blast flings
  it); a charged one in a pump drains the joined water a cell at a time, the nearest first, until
  spent; a small crystal for a well's pump.  Room: *the flooded hall*, a spare crystal for a
  first charge.  The daily refill waits for the clock (C1): here it is played on a clock set by
  hand.  Ruled first: cells a charge, the draining speed.  Red: a new `tests/pump.loft`.
- **B5. The flotilla and the rope.**  Beams float where other things are lost; beams that touch
  link as carts do; beams fastened with rope are stood on, a lone beam is not; pushing off drifts
  the flotilla until it touches something.  Rope is carried on the shoulders beside what is held,
  and does not block pushing or pulling.  Room: *the lake*.  Ruled first: how rope fastens (walked
  into with rope, the beams beside), how many beams carry the hero.  Red: a new
  `tests/flotilla.loft`.
- **B6. Crafting.**  A workbench (the smithy's anvil) turns things brought to it into another: the
  blunt sword into a sword; the artefact, from its pieces (a number the game counts, owner: a chest
  adds one) and the materials.  Room: *the smithy*.  Ruled
  first: how a recipe is given (walked into with the parts beside it, or a talk with the one who
  teaches).  Red: a new `tests/crafting.loft`.
- **B7. Foraging and brewing.**  Plants that can be picked (Tiny Farm's), meat from what is hunted;
  a person who takes ingredients and gives a potion (a talk's `gifts`, `take`, `give`, built).  Room:
  *the garden*.  Red: a talk test, ingredients in, a potion out.
- **B8. The new creatures, one a step.**  The grey rat (out by day, carrying off food left lying),
  the slime, the crab, the ghost (at night, after C1), bigfoot (with Stormcap's terrain).  A room
  each.  Ruled first: each one's habits.  Red: a test per creature in `tests/enemies.loft`'s manner,
  every time by hand.

## Phase C — the world's systems

WORLD.md's order of work, steps 2, 3 and 5, with what the story adds.  Each is a system all places
share, so each is built once and shown in the test world first.

- **C1. The clock.**  A time of day, one clock across places, night and day drawn; timed states as
  moments (WORLD.md step 2).  Room: *the night room*, a lamp lit at dusk.  Red: the same moment
  asked twice and across places giving the same answer.
- **C2. Shared facts.**  A line that sets a fact, and lines, barks and talks that depend on facts;
  being seen (who saw the hero do what); goodwill, lost by snooping and won back by aid.  Room: two
  villagers, one who sees and one who is told.  Red: a talk test, a fact set by one and read by
  the other.
- **C3. People following points.**  Creatures and people at their points at their hours, walking
  between places by the doors; a person brought to a place (the alchemist to Stormcap, the people
  to the barricade).  WORLD.md step 3.  Room: a villager going to work and home.  Red: a scripted
  day, each at their points.
- **C4. A group acting together.**  The Kharn as a tribe: prey counted down by the clock and by
  what has happened to the monsters; nervous scouting outside the mine; the attack; turned back
  by a defended place, they leave.  Room: a small tribe, a small village, a barricade.  Red: the
  attack at the moment the prey runs out, and a retreat that holds.
- **C5. A barricade built by its people.**  People with an axe and time turning beams into a
  barricade at an entrance; more hands, faster.  Red: the barricade's size after a set time, for
  two numbers of hands.
- **C6. Saves.**  The clock, the hero, each place's changes, the facts (WORLD.md step 5).  Red: a
  save, a restart, the same state.
- **C7. The epilogue.**  The player walks away; stills of what follows, chosen by the facts.  Red:
  the stills for a set of facts.

## Phase D — the story world, place by place

Each place is built with mechanics and systems that already play in the test world, cites what it
serves (@ST-…, @GM-…), and carries a walk-through test.  LOCATIONS.md is each place's brief.

- **D1. Orbury** — the village as the unannounced tutorial: needs B4 (the well's small crystal),
  C1 (night), C2 (being seen, aid).
- **D2. The hidden path, the brook, the hillside, the cave-in** — needs B8 (crabs), C1 (night).
- **D3. The Old Seam, first level** — the Kharn's home: built mechanics, C3 (their points).
- **D4. The Old Seam, second level** — needs B2, B3.
- **D5. The flooded lower tunnels** — needs B4.
- **D6. The Still Lake** — needs B5, B8 (slimes).
- **D7. The High Diggings** — Sable's ground: built mechanics, C3 (his day).
- **D8. Stormcap** — needs B4 (the lightning), B8 (bigfoot), C3 (the alchemist brought up).
- **D9. The climax and the endings** — needs C4, C5, C7; the whole world played through.

## Where to begin

A1–A3 and B1 change little and unlock the rest.  The first step with a picture worth playing is
**B4** (the pump, with the spare crystal's first charge), because D1, the village, needs its small
form; with C2 it makes D1 the first story place.
