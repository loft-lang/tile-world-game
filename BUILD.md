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

- **A0. The drawing kit — built (2026-11-04).**  Shapes a place file's intent holds and the game
  fills in (intent.loft): ground, trails (paths, brooks, tunnels with a rim), blobs (caves, ponds,
  groves scattering trees), rooms and corridors, houses, slopes, items; `tools/fill.loft` fills a
  place in, `tools/map.loft` pictures it whole.  The test world's seven new rooms are drawn with
  it (the armoury, the workings, the lift, the flooded hall, the lake, the smithy, the garden), a
  street of houses for them in the demo village.  Red: `tests/kit.loft` (a house's door, the
  slope onto a ledge, the ford, the grove's trees, the ramp between galleries, items, a tunnel's
  doorway and rubble kept off the rails).
- **A0b. Domes and what holds — built (2026-11-05).**  The owner's caves (DESIGN.md § Bombs): a
  natural cave has a dome, its walls moving one cell in per layer up, so a wide cave needs height;
  it can have partly straight walls, and so more height there; and it can be crossed by a bridge
  higher up.  The kit's `domes` shape draws one from a blob's floor (`straight` layers on its
  `sides`); a bridge is a trail on a higher layer (the `bridge` material, planks).
  `tools/stable.loft PLACE` lists the cells that would not hold — under a ceiling, no wall beside
  them (`rules::unsupported`): what a blast-widened tunnel leaves.  Room: the demo's *dome* house
  (a straight north wall, stairs up two layers, a bridge across, a chest beyond).  Red:
  `tests/intent.loft` (each floor cell's ceiling at its distance from the outside; the straight
  wall layer by layer), `tests/places.loft` (the dome holds, a three-wide tunnel does not, the
  bridge crossed).  The cave-in that acts on unsupported cells: B2c.

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

- **B1. The weapons — built (2026-11-04)** with the proposed numbers, to tune in play.  Damage by weapon (sword and axe 2, torch 2 against bats and 1 otherwise,
  pick and bare hands 1), recovery by weapon (the axe slower), and reach (a held item 3 tiles, bare
  hands 1).  Room: *the armoury*, a sword, an axe, a torch and bats.  Ruled first: the axe's
  recovery, what the 3 tiles are.  Red: `tests/blows.loft` extended, a blow by each weapon timed and
  counted by hand.
- **B1b. Combat moves** (@GM-Combat) — **built** (`tests/combat.loft`, `tests/fights.loft`); in the
  game J (or Space) let go is a light swing, held 500 ms a heavy one, K (or Shift) pressed with no
  direction a parry and held a block.  Planned in the fight rooms, one move a step: (1) enemies' wind-ups
  and the dodge (the blow lands on the cells aimed at); (2) the stance and the side-step; (3) the
  light chain and the heavy swing; (4) the lunge; (5) the parry and the riposte; (6) each enemy's
  own wind-ups, poise and moves.  Red: a test per move, every time by hand, both ways (the hero's
  and an enemy's), and the fight rooms played.
- **B2. The pick, rubble and the cart's load — built** (the demo's *cave-in* house;
  `tests/places.loft`).  A rock cleared into a cart beside the hero; the cart full until bumped
  from the side at an unloading place; a full cart heavier.  Answered (proposed, to tune in play):
  a full cart weighs two carts, pushed and pulled; the cart beside the hero is looked for on its
  left, its right, then behind; a cart holds one rock; an unloading place is any free floor beside
  the track, the bump coming across it; with no cart beside, the pick leaves the rock as it is.
- **B2b. Beams and cave-ins** (owner; DESIGN.md § Torches and light).  A beam lying loose is an
  item carried in the hand; a beam standing in its place holds a tunnel up, and taken out with a
  pick it collapses the tunnel — the difference on screen.  The accidental barricade (the village
  barricade's picture) taken apart with a pick gives 2 beams, or burned away by a lit tar barrel
  opens the way and gives none.  Proposed: the collapse fills the tunnel with rubble (rocks
  touching), cleared again with the pick and a cart (B2) — a cost in time, never a dead end.
  **Built** (the *cave-in* house).  Answered (proposed): a standing beam (`prop`) stands across a
  tunnel; taken out, a beam lies on its cell and rubble falls on up to 3 free cells beyond it, the
  hero's side untouched; a held flame lights a tar barrel (it burns as a torch does, then is gone);
  the lit barrel pushed against the barricade sets it burning, gone in 3 s; taken apart, its second
  beam goes beside it, else at the hero's feet.
- **B2c. The directed blast and the cave-in — built** (the demo's *blasting* house;
  `tests/blasting.loft`, `tests/places.loft`).  The owner's rules (DESIGN.md § Bombs): a full dyke
  of five stones round a bomb directs it out of its open side, further and harder, the dyke
  holding; lit over a stone with a burning torch, at once; it cracks the first wall ahead and blows
  out a door, also one held shut by a beam (lifted from the beam's side, `bars` in the level);
  a span it widens under rock keeps gathering stones.  The proposals taken are listed there.
- **B3. The cart lift.**  A platform between layers carrying its stack, sent by a lever (or an
  arrow), moving only with its shaft clear, its rails joining the track at each stop.  Room: *the
  lift*.  Ruled first: its speed; riding it with no cart.  Red: a new `tests/lift.loft`.
- **B4. The pump and its crystals — built** (the demo's *pump* house; `tests/pump.loft`).  A
  crystal cannot be harmed (an item: a blast flings it).  Put into the mine's pump, a charged one
  gives it its charge at once and is left spent; the pump drains the water joined to it, nearest
  first, each drained cell what the level lists beneath it (`beneath`; floor otherwise).  The
  village's well pump runs on a small one — the fact `well_running` — and goes off like a bomb
  with a large one; the mine's pump ignores a small one.  A spent large crystal on a lightning rod
  is charged by a strike — the place's `storm` cycle, until the weather (C1) drives it; a small
  one the alchemist fills (a talk's gift).  Answered (proposed): a cell every 1500 ms; a full
  charge 12 cells, the spare 4; the well runs 60 s on a small one.  Not yet: the daily refill
  (needs the clock, C1).
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
- **B7b. The sack** (owner).  Carried on the shoulders in the rope's slot, so never beside the
  rope, and solely for foraging: it holds what is foraged (plants, meat, ingredients) while the hand
  holds its one item.  **Built** (the *herbs* house).  Answered (proposed): it carries 6; a herb or
  a mushroom walked over goes into it, whatever the hand holds; a talk's gift is met from it too,
  every one of that kind taken (the hand left as it is); with the sack worn, the rope walked over
  stays lying, and the other way round; L with empty hands puts the shoulder thing down, the sack
  keeping what it holds; both go through doors.
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
- **C2b. A torch at night gives the hero away** (owner).  Light is seen, by the villagers as by
  the Kharn: a hero carrying a lit torch into a home at night is suspect, a fact (C2) that costs
  goodwill; the dark keeps it unseen.  Needs C1 (night) and C2.  Room: a house at night, a villager
  beside it.  **Built** (the *night* house), with the part of C2 it needs: facts known, carried
  through doors, a talk starting where a known fact says.  Answered (proposed): people see the
  hero, not a window — within 6 cells in sight when the hero is lit, within 2 in the dark, as the
  Kharn do; a door marked `home` gone through while seen sets `snooped`.  Night before C1: the
  street is covered (a wall layer over it), so only flames light it; the rule is light alone.
- **C3. People following points.**  Creatures and people at their points at their hours, walking
  between places by the doors; a person brought to a place (the alchemist to Stormcap, the people
  to the barricade).  WORLD.md step 3.  Room: a villager going to work and home.  Red: a scripted
  day, each at their points.
- **C3b. Sable in the way** (owner; STORY.md @ST-Rival).  The rival adventurer on his points: in
  the main mine by day, home by dusk; walkways he blocks so others go the long way, and his
  practical traps built from what exists — a runaway cart waiting at a lever on a slope, a bomb
  left beside the rails.  **Built** (the *Sable* house), with the part of C3 it needs: a route, its
  points by the place's clock (`routes`, a cycle and each point's moment).  Answered (proposed): he
  does not move his obstructions — two rocks in a row left in the short corridor stay, so the
  long way it is; a demo day is 60 s, the dig from 0, home from 30 s.  The trap, from what exists
  (a lever cannot hold a cart, which starts down any branch that leads downhill): a cart waiting at
  the terrace's edge, held by a rock on the rails; whoever pushes the rock off stands on the track,
  and the cart, starting a cell every 600 ms, strikes them unless they step off at once.
- **C3c. Following through a door.**  What is with the hero passes with it (WORLD.md § The
  world): a cart it pulls, an enemy close behind, a person it leads.  Room: two houses joined by a
  door.  Red: each comes through on the hero's step, on the far side.
- **C2c. Hints for a stuck player** (@ST-Hints).  The game notices a player stuck — long in one
  place, the same tries again — and the people's lines can depend on it, as on a fact.  Red: a
  scripted stay that sets it, a talk that then gives the hint.
- **C2d. Persuading and being understood.**  What convinces someone — Odran to give the
  lightning's energy, Voss of the danger — is the owner's to define; the engine's part is a talk
  that changes on what the hero has done, brought or shown (facts, gifts, things seen).  Urgha's
  broken words (@ST-Cyclopes): lines that come through in part.  Red: a talk that will not until a
  fact is known, then does.
- **C9. The Dunns' bargain** (@ST-Farm).  The hero's horse lent to the farm for a bed in the barn:
  a choice made once, kept (a fact), the horse at the farm until he needs it.  Red: the bargain
  struck, the horse moved, the barn open.
- **C4. A group acting together.**  The Kharn as a tribe: prey counted down by the clock and by
  what has happened to the monsters; nervous scouting outside the mine; the attack; turned back
  by a defended place, they leave.  Room: a small tribe, a small village, a barricade.  Red: the
  attack at the moment the prey runs out, and a retreat that holds.
- **C5. A barricade built by its people.**  People with an axe and time turning beams into a
  barricade at an entrance; more hands, faster.  Red: the barricade's size after a set time, for
  two numbers of hands.
- **C6. Saves.**  The clock, the hero, each place's changes, the facts (WORLD.md step 5).  Red: a
  save, a restart, the same state.
- **C6b. The fountains as save points** (owner; STORY.md @ST-Fountains).  The strange fountains
  in the mines and the hills save the whole situation as a well does today; their liquid cannot be
  taken (it turns to mud).  Tiny Dungeon's wall fountains (green 8, 20, 32, 44; dry 7, 19, 31, 43):
  a fountain kind using the wells' rule.  None in the village (LOCATIONS.md).  Room: a gallery with
  a fountain.  **Built** (the *fountain* house): a look of the well kind (dungeon:32), the dry
  basin a terrain of its own (`basin`, dungeon:31), the pouring spout a wall's look.  Answered
  (proposed): a dry fountain is never filled — it only stands in the way, a sign that one was
  there; coming back the game says BACK AT FOUNTAIN.
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
