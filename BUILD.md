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
  `sides`; `shape` "dome" — the owner's: steps of 1, then 2, then 4, the top over a middle of 8 —
  or "crag", a step a layer; `height` to raise or flatten it; overlapping shapes on one floor make
  one cave, each shaped within the whole); a bridge is a trail on a higher layer (the `bridge`
  material, planks).  The fill tool sets what an intent draws and never clears a cell an earlier
  intent drew (it cannot tell it from one set by hand): a place whose intent loses cells is drawn
  again from empty.
  `tools/stable.loft PLACE` lists the cells that would not hold — under a ceiling, no wall beside
  them (`rules::unsupported`): what a blast-widened tunnel leaves.  Room: the demo's *dome* house
  (a straight north wall, stairs up two layers, a bridge across, a chest beyond).  Red:
  `tests/intent.loft` (a crag's ceiling at each cell's distance; the straight side; a dome's
  profile and a chosen height; two shapes as one cave), `tests/places.loft` (the dome holds, a three-wide tunnel does not, the
  bridge crossed).  The cave-in that acts on unsupported cells: B2c.

- **A1. Figures in the editor — built** (`tests/editor.loft`).  The palette's figures, every kind
  of enemy and person by name, after the things: painted, one is put on the cell (replacing one
  there), erased first by Backspace, undone like any edit; saved in the place file's `enemies`.
- **A2. Doors and text in the editor — built** (`tests/editor.loft`, `checks/game.loft`).  The
  panel's DOOR TO and TEXT fields set the cursor's cell's door (to a place, coming out at its door
  named after this one) and its text; the words `door=`, `text=` do the same.  A door made in the
  editor, walked through in play.
- **A3. What a place serves — built** (`tests/editor.loft`).  The panel's SERVES list: a tag
  clicked is taken away, one typed added (`serve=`); saved, `tools/tags.loft check` reads it (the
  words house now serves @ST-Cyclopes and @ST-Hints, added through it).

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
- **B3. The cart lift — built** (the demo's *lift* house; `tests/lift.loft`).  Its platform is a
  rail cell in the shaft on the layer it is at, the shaft open (void) at its other stops — so the
  track joins it at each stop, and a drop in the shaft is there where the platform is not (owner:
  fine, not from all sides); its stops in the level's `lifts`.  A lever beside the shaft calls it
  to its layer, or sends it on from there to the next stop down (from the bottom, up); it carries
  what stands on the platform and the hero riding it; a stop under water is passed by (owner: the
  lowest reached only once the pump has run); it moves only with its shaft clear.  Answered
  (proposed): a layer every 1000 ms; riding it with no cart, yes.
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
- **B5. The flotilla and the rope — built** (the demo's *lake* house, a jetty added;
  `tests/flotilla.loft`).  Beams float where other things are lost — set into water from the
  shore, or flung there by a blast; beams that touch link, as carts do; fastened with the rope worn
  on the shoulders (walked into, the rope used up), they are a raft, stood on; a lone or loose beam
  is not.  From the raft, open water walked into is pushed off into: the raft drifts that way, the
  hero on it, until it touches something.  Answered (proposed): a raft carries the hero from 3
  beams; one rope fastens every beam touching the one walked into; it drifts a cell every 500 ms;
  pushing never moves items into water (rules as they were).
- **B6. Crafting — built** (the demo's *smithy* house; `tests/crafting.loft`).  The anvil is worked
  with the swing key, a hot forge within 3 cells — the abandoned smithy made functional by lighting
  its forge with a flame (owner: it can be made functional).  The blunt sword is made whole, a
  sword; the artefact is made from 4 of its pieces (owner: a number the game counts; old chests
  give one each, `relic_chest`) and the lake's essence, once its lore is known — the fact a talk
  line `sets` (owner: learnt from the alchemist).  Answered (proposed): the swing at the anvil (a
  walk into it pushes it); 4 pieces; the forge's reach 3 cells; the pieces carried through doors,
  on the status line.
- **B7. Foraging and brewing — built** (the demo's *herbs* house; `tests/talk.loft`,
  `tests/creatures.loft`).  Plants to pick (herbs, mushrooms: forage, into the sack); meat from what
  is hunted — a rat, a crab, bigfoot leave it when ended (owner: the hero can forage meat and
  vegetables for the villagers; killing is never the goal, but hunting for food is a reason);
  a person who makes something of ingredients: a talk's `recipes`, each the ingredients it needs
  and how many — the sack holding enough, bumped, that much is taken and its line said (the
  alchemist: 2 herbs and a mushroom, a health potion); a farmer who takes meat (the fact
  `village_fed`).  Answered (proposed): which creatures leave meat; the recipe's numbers.
- **B7b. The sack** (owner).  Carried on the shoulders in the rope's slot, so never beside the
  rope, and solely for foraging: it holds what is foraged (plants, meat, ingredients) while the hand
  holds its one item.  **Built** (the *herbs* house).  Answered (proposed): it carries 6; a herb or
  a mushroom walked over goes into it, whatever the hand holds; a talk's gift is met from it too,
  every one of that kind taken (the hand left as it is); with the sack worn, the rope walked over
  stays lying, and the other way round; L with empty hands puts the shoulder thing down, the sack
  keeping what it holds; both go through doors.
- **B8. The new creatures — built** (the demo's *creatures* house; `tests/creatures.loft`), each
  with a way of its own — proposed, the owner rules each one's habits: the grey rat (dungeon:124)
  out by day, carrying forage left lying home to its nest, home at night; the slime (108) keeping
  within 2 cells of water, swallowing an item it comes onto (falling out when it is ended); the
  crab (110) within a cell of water, going only sideways, east and west; the founder's ghost (121)
  there only by night — away by day, not met, not drawn — passing through walls, harmless, a talk
  for the owner's lines; bigfoot (Tiny Ski's 78) big and strong, out by day, home by night — the
  ways to deal with him are the owner's, with Stormcap's terrain.

## Phase C — the world's systems

WORLD.md's order of work, steps 2, 3 and 5, with what the story adds.  Each is a system all places
share, so each is built once and shown in the test world first.

- **C1. The clock — built** (`tests/day.loft`, `checks/game.loft`).  One clock for the world,
  every place keeping its time; night from 20:00 to 06:00, when the open sky gives no light
  (tile_world 0.7.0's `daylight`, loft-libs-world#41) — so whoever sees by light (the Kharn, the
  people) sees by what gives light of itself; the time on the status line.  A day without the pump
  turns back drained cells (B4's refill), a place catching up the days on entering.  A bed: slept
  in, the clock moves on to the next change of light (20:00 by day, 07:00 by night) and the
  temporary health goes (owner: sleep clears it).  Answered (proposed): a day is 20 minutes of
  play; it starts at 07:00; 4 cells refill a day, the last drained first, never one the hero or a
  thing stands on.  The ghost (B8) and people's hours (C3) now follow this clock.  Not yet: night
  drawn darker in the places that were drawn covered for it (the night street keeps its cover).
- **C2. Shared facts — built** (the *night* and *words* houses; `tests/talk.loft`).  A line that
  `sets` a fact; barks said only `when` a condition holds, and talks starting where one holds
  (`facts`) — a condition is a fact, `!fact`, `liked` or `disliked` (`rules::holds`); goodwill,
  one number for the village (owner: lost by snooping, won by aid): seen going into a home −1, a
  gift taken +1.  Facts are the village's, so the one who sees tells everyone.  Answered
  (proposed): goodwill one number, not one per person; ±1 a deed; carried through doors.
- **C2b. A torch at night gives the hero away** (owner).  Light is seen, by the villagers as by
  the Kharn: a hero carrying a lit torch into a home at night is suspect, a fact (C2) that costs
  goodwill; the dark keeps it unseen.  Needs C1 (night) and C2.  Room: a house at night, a villager
  beside it.  **Built** (the *night* house), with the part of C2 it needs: facts known, carried
  through doors, a talk starting where a known fact says.  Answered (proposed): people see the
  hero, not a window — within 6 cells in sight when the hero is lit, within 2 in the dark, as the
  Kharn do; a door marked `home` gone through while seen sets `snooped`.  Night before C1: the
  street is covered (a wall layer over it), so only flames light it; the rule is light alone.
- **C3. People following points — built** (the *work* house; `tests/following.loft`,
  `checks/game.loft`).  The world's people (`levels/people.json`): each a kind, points over a
  cycle — a place, a cell, from when — and a talk that goes where it goes.  Entering a place
  (WORLD.md step 2's whereabouts), the people whose point is there are at it; during a visit, one
  whose point moves elsewhere walks to the door that leads there and goes from beside it, one whose
  point moves here comes in by the door from its last place and walks to it.  A person brought
  somewhere: led (C3c).  A guide (Lark): a line `guides` to a cell — it walks ahead while the hero
  keeps within 4 cells and waits otherwise; there, `guided` is known and it goes back home.
  Answered (proposed): only a door straight to the next place is walked to (no route over the door
  graph yet — a person with none simply goes and comes); a person's talk starts over when it comes
  back; the demo worker's cycle 60 s, the street from 0 and work from 30 s.  Still open: WORLD.md
  step 3's regions, bands and coverage table.
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
- **C3c. Following through a door — built** (the *follow* house, two rooms joined by a door;
  `tests/following.loft`).  What is with the hero passes with it (WORLD.md § The world): every
  person it leads, an enemy that saw it right beside it (one step), and the thing in front of it
  when its last step pulled one (the first car of a train only).  Set down on the door's cell
  behind the hero — the thing, then the first one, if the thing leaves it free — the others
  beside the hero, north, east, south, west, then the corners.  A person brought somewhere: a
  talk line `follows` and the person is led, keeping beside the hero; a line `stays` and it is
  not.  Answered (proposed): everyone led comes along wherever they are in the place; a person's
  home is the cell it was set down on, so one brought somewhere stays near the door it came in
  by; its talk goes with it (the game's `moved_talks`, applied whenever a place opens).
- **C2c. Hints for a stuck player — built** (@ST-Hints; the *words* house).  The fact `stuck`:
  known after 2 minutes within 3 cells of one spot, or after 3 blocked tries at one cell in a row;
  forgotten when the hero moves 3 cells on (or through a door).  A talk starting at its hint line
  once `stuck` holds (`"facts": {"stuck": "hint"}`).  Answered (proposed): the numbers; the hints'
  words are the owner's.
- **C2d. Persuading and being understood — built** (the *words* house).  Persuasion is a talk that
  starts elsewhere once a fact holds (what convinces Odran or Voss stays the owner's to define).
  Urgha's broken words (@ST-Cyclopes): a line with `clear` comes through in part — every other
  word heard as `..` — until its fact is known (proposed: the scholar teaches `cyclops_words`; who
  teaches it in the story is the owner's).
- **C9. The Dunns' bargain — built** (@ST-Farm; the *farm* house, a yard and a barn).  The hero's
  horse (our sheet's 24) follows it, spoken to; Dunn's offer is taken by bumping him while he makes
  it, refused by walking away (asked again next time); struck, it is kept — the fact `bargain`,
  his talk starting after it from then on — and a line's `keeps` leaves the horse with the farm,
  no longer following.  The barn's door `needs` the bargain: shut (SHUT) until it holds; a bed
  inside.  Answered (proposed): the horse taken back by speaking to it again, the bargain and the
  bed kept; the words are placeholders for the owner's.
- **C4. A group acting together — built** (the *raid* house; `tests/tribe.loft`).  The Kharn as
  a tribe (a place's `tribe`): its members the place's enemies of its kind; its prey counted down
  by the clock (one eaten every `eat_ms`) and by what happens to the monsters (one of its
  `prey_kinds` ended, one less); at one left, one member scouts towards the cell it raids and
  turns back within 3 cells of it; at none, all attack it.  Arriving with a barricade on that
  cell, they turn back, and once all are home they leave for good — the retreat holds.
  Answered (proposed): the numbers; a defended place is a barricade on the raided cell; not
  turned back, they fight as cyclopes do.  Still open: a tribe across places (their mine, the
  village), scouting seen from outside.
- **C5. A barricade built by its people — built** (the *raid* house; `tests/tribe.loft`).  A
  place's `works`: the cells in the order built and a pile of beams.  Every 2000 ms each person
  holding an axe within 4 cells of its first cell adds a unit; 3 units turn a beam from the pile
  into a barricade on the next free cell.  More hands, faster: after 8000 ms one hand has built
  one cell, two hands two.  The hero brings more hands by leading them (C3).  Answered
  (proposed): the numbers.
- **C6. Saves — built** (`checks/game.loft`).  Drinking at a well or a fountain also writes the
  game to `saves/` (git-ignored): `save.json` — the clock, the place and the hero's cell, what it
  carries (its things, health, money, the facts, the goodwill, the pieces), the places solved and
  the talks moved — and each place as it was left, `<place>.json` in the place files' own form.
  The game started for play goes on from it (`TW_NEW=1` starts afresh); the checks and the
  pictures never write one.  Answered (proposed): enemies are saved at their homes, so a fight is
  not saved in the middle; what is in motion (a rolling cart, a lit fuse, a running pump) is not
  saved; the world's people are not written — read back, they are where their points say.
- **C6b. The fountains as save points** (owner; STORY.md @ST-Fountains).  The strange fountains
  in the mines and the hills save the whole situation as a well does today; their liquid cannot be
  taken (it turns to mud).  Tiny Dungeon's wall fountains (green 8, 20, 32, 44; dry 7, 19, 31, 43):
  a fountain kind using the wells' rule.  None in the village (LOCATIONS.md).  Room: a gallery with
  a fountain.  **Built** (the *fountain* house): a look of the well kind (dungeon:32), the dry
  basin a terrain of its own (`basin`, dungeon:31), the pouring spout a wall's look.  Answered
  (proposed): a dry fountain is never filled — it only stands in the way, a sign that one was
  there; coming back the game says BACK AT FOUNTAIN.
- **C7. The epilogue — built** (`checks/game.loft`).  A door that `ends` (the village's LEAVE gap
  in its east edge): walking through, the hero walks away.  The stills (`levels/epilogue.json`),
  each shown when every condition in its `when` holds (`rules::holds`: facts, `!fact`, `liked`,
  `disliked`) — a place drawn as it is in its file, nobody in it, the view centred on a cell, and
  a caption; one after another, 6 s each or a key sooner, then THE END.  Answered (proposed): the
  stills show the places as drawn, not as the hero left them; the five stills there are
  placeholders — the epilogue's words are the owner's.  The small letters have no comma or
  apostrophe: captions do without.

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
