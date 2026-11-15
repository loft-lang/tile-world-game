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
- **B1c. The fight, designed again** (@GM-Combat, owner 2026-11-07) — being built: step 1 and the
  start of step 6 (2026-11-07).  Each
  step is playable on its own, so the fight can be tried between steps; the numbers stay proposed
  until played.  Ruled first: what each step's *Open* in DESIGN.md asks.
  0. **One fighter**: the hero and every enemy share one fight state and one profile (today the
     hero's is fields of the level and an enemy's its own), so each rule is one function and each
     test runs both ways.  A template is a list of this fighter's moves.  No play changes.  Red:
     the built combat and fights tests, unchanged, on the shared fighter.
  1. **The keys**: Q takes and puts down (walking over a small item no longer takes it) and pulls
     held; E uses (swing let go, heavy held); Tab locks (the bow's aim moved onto it); Esc quits;
     F1 and F2.  The built block and parry keep a key of their own until step 4.  Red: the item
     tests take with Q; `checks/game.loft` presses the new keys.  **Built** (2026-11-07), with F3's
     free roaming beside it (an experiment, the owner's).  Q acts on the cell before the hero, never
     the one it stands on (owner; `rules::take`, `rules::put_down`, `rules::take_step` for scripts),
     and the hero's facing is seen (a lean, its back facing north; DESIGN.md § What the player
     sees).  Still open: Tab's lock for the sword (it is the bow's aim as built: the first target in
     line).
  2. **Locked on**: W, S, A and D against the target; the weapon's eight ways; the 3D page passes
     the locked keys on as they are; Tab's choice within the arc the hero faces (owner), the bow
     able to hit a target off the line; hit with no target, the lock turns onto the attacker
     (owner).  Red: a move each way, a diagonal target, the lock lost, a blow from behind locking
     on.
  3. **The point and the lunge**: a ready point fills its tile, stepping onto it hurts and flings;
     the lunge between tiles and its settling; no lunge beside an enemy; a walk into an enemy no
     blow.  Red: the standoff holds untouched; the distances, by hand.
  4. **The swing in flight and the clash**: outgoing and incoming, the recovery after a clash, the
     riposte; the guard on S; giving ground needs room.  The K parry and block go.  Red: each row
     of the clash table, both ways.
  5. **Balance and dodging**: out of balance after a dodge, a point's turning speed, armour's
     weight.  Red: a second dodge refused, a blow following the fighter out of balance.

     **Steps 2–5 built on whole cells** (2026-11-15, owner; `tests/arena.loft`, rules.loft § Fencing
     by moving), tried on the arena's swordsman.  Built: Tab locks a sword on the nearest enemy
     within 4, then the next, past the last letting go; the lock follows its target and is lost
     past 4; in the game a wind-up aimed at the hero or a blow that hurt it locks on to the one who
     did it.  Locked, the point turns at the sword's 100 ms a 45°; within 3, W lunges, S steps back
     with the guard up (held, it stays up and does not step again), A and D dodge, out of balance
     500 ms after (a second step aside is no dodge, and a wound-up blow follows it).  A ready point
     fills the tile before its holder: run onto it (a step into it, or from the standoff into the
     crossed blades), 1, flung back, open 400.  The lunge reaches from 2 or 3 for the weapon's +1 and
     breaks a wind-up; into a ready point it is the hero who runs onto it; beside its target, none.
     A swing meeting a fencer's swing in flight is a clash: each recovers for its draw and 200 ms
     more and the one ready first may riposte; a heavy swing met in its last two thirds goes
     through for half, the hero staggered.  The heavy swing is wider (owner, 2026-11-15).  The
     sword is carried at the side unlocked, its point raised locked on (drawing only).
     Where it differs from DESIGN.md, so far: the four ways only, no diagonals; Tab picks by
     distance, not within the arc; the lunge does not carry the body between tiles — it reaches and
     the hero stays on its cell (the settling back the design describes for an equal distance); the
     point and the clash are a fencer's (`fences`) only, so the cyclops's sweep, the rats and the
     built fight rooms play as before (a rat does run onto a ready point); E with no lock does not
     yet turn to the nearest; a walk into an enemy unlocked is still a blow; the Shift parry and
     block stay beside S until the clash has been played.
  6. **Enemies by templates** (TEMPLATES.md): the templates' file and their choosing, the rat's and a human
     swordsman's first (the two profiles), then each kind's; patience, the cornered animal.  Red:
     each template alone against a still hero, timed by hand; every opening its own; every
     aggressive template's gap fits a lunge; `tests/fights.loft` again with its players as hero
     templates (a striker, a patient fencer, a dodger), the owner's claims held.  **Begun**
     (2026-11-07; `tests/templates.loft`): the machinery — templates in kinds.json (`templates`, a
     kind naming its own), chosen by distance and seeded, run step by step, broken by a blow or a
     scare, a kind keeping its distance while one rests — and the bat's swoop.  The rat's leap
     next; this runs before steps 0 and 2–5, so a template's blow still meets the built parry and
     block.  **The swordsman built** (2026-11-15, owner; `tests/arena.loft`, TEMPLATES.md § The
     swordsman): the demo village's arena, a human opponent with the hero's moves (`in`, `flank`,
     `draw`, `lower`, `strike`, `heavy`, `lunge`, `guard`), templates that read the hero's state
     (drawn back, guarding, ready), and three phases by his health, each a test player's style —
     the striker, the patient fencer, the dodger.
  7. **The off hand and the profiles**: the knife and the shield, the spear (an enemy with one in a
     fight room), a corridor room and an open one.  Red: the base sword wins every room with
     skill (growth is never necessary).
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
  bed kept; the words are placeholders for the owner's.  **In Orbury's own map (2026-11-10)**:
  Thornfield's barn is part of the town, so its door is a `gate` of the map (`rules::Gate`, a
  place file's `gates`), shut until `bargain` holds; a line's `keeps` makes the kept figure's
  home where it stands, and the game moves its talk along (`game::step_talks`, a talk moved;
  `checks/game.loft`'s `test_thornfields_bargain`).
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
  C1 (night), C2 (being seen, aid).  **Built first, before the fight is finished** (owner,
  2026-11-08: the puzzles are the focus, the fight a base to perfect alongside), and **as a place
  without its story beats**: the town's real locations, to verify the templates there; the game's
  actual start (riding up to the town, meeting the farmer's boy), the dream and the talks come
  later.  In steps, each with its tests and a picture to judge (PUZZLES.md § Houses, LOCATIONS.md
  § 1):
  - **D1a. A house from the inside out.**  The kit's `houses` grow from an outside to a shell
    with levels: its walls on the street's layer, its floors, a basement below and a roof on the
    layer over the top floor that can be walked on (a roof material of kind floor; theme.json's
    `roof` stays the demo's).  Its rooms parted by thin walls, as edges (EDITOR.md § 3: edges for
    thin inner walls), with door and window edges; the fill writes edges as well as cells.
    Checks: every room reached, the roof over the whole footprint, every window and door landing
    in a room, the roof seen from the street and gone from inside.
    **Built (2026-11-08)**, `tests/houses.loft` (both backends): a house with `floors` (and
    `basement`) in a place's intent is a ring of its wall on each floor, its door in front, stairs
    up from one floor arriving on stairs down on the same cell of the next, `house_roof` (theme.json:
    a roof of kind floor) over the whole footprint on the layer above, and rooms parted by thin
    walls with a door in each, by the seed (`intent::house_edges`; the fill writes them,
    `editor::refill_edges`).  Every floor cell of every layer is reached from the door, over eight
    seeds.  With it, in the picture (`game.loft`): no bank under a cell with a wall below it, so the
    roof sits over the front wall and its door as Kenney's houses do; under the open sky every layer
    up to four above is drawn (a second floor's roof, a hill's terraces); thin walls are drawn
    (walls, doors, windows, fences: edges were not drawn before).  Open: the rooms are dark by day
    (no window lets the daylight in: the outer walls are cells, so a window there needs either
    thin outer walls or a wall that lets light through); an upper floor's front wall is drawn as
    dark as the rooms behind it; the edges a fill writes are not in the undo (as a paste's are
    not); the thin walls' look is plain colour for now.
  - **D1b. Households as templates.**  The room templates of PUZZLES.md § Households (a miner's
    cottage, a craftsman's, a well-off house, the inn, the alchemist's, the smithy, the lord's
    house) fitted into a fixed shell by a seed, with what the story fixes kept where it is.
    Every ordinary house is dealt from three templates (owner, 2026-11-10; PUZZLES.md § Houses):
    how it was locked, with a way in; how its owners left its inside; its cellar and the way into
    it.  No cellar is one of the third's options, the sign of a wrong house, and a cellar can be
    hidden (STORY.md @ST-Urn); D1d's closing is the first of the three.
    **The templates as files and the deal built (2026-11-10)**: 45 templates under
    `levels/templates/` (HOUSES.md § Templates as files), applied turned and mirrored by
    `stamp.loft`; Orbury's eleven ordinary houses dealt from them by the game's seed, each played by
    the rules before it is kept, Sibil's always with a cellar (`deal.loft`, HOUSES.md § The deal;
    `tests/stamps.loft`, `tests/deal.loft`).  The deal goes to the play log (D1e's first half).
    Tunnels (2026-11-10): the cellars reached only by one get it, laid from the gap in their wall
    and stretched to the town, and are played along it (HOUSES.md § 4).
  - **D1c. The town's fixed layout** (`levels/orbury.json`): the hill in terraces, the main road
    of slopes, the stair roads, the market, the houses' shells, the dried stream a layer down with
    its bridges, the loading dock across it, the farms and the cemetery.  Pictured whole with
    `tools/map.loft`, walked with `TW_WORLD=orbury`.
    **A first draft built (2026-11-08)**, a world of its own (owner: the actual first game world,
    started apart; the demo village stays, to grow when needed): `order.json`'s `worlds`, picked
    before the game (`TW_WORLD`, or the play server's page; owner: one menu, not a second in the
    game), each saved apart (`game::worlds`, `save_dir_of`; `tests/worlds.loft`).  The
    hill in five layers, the hilltop (4) down to the foot (0), earth under each terrace so its
    front shows as a retaining wall; the main road from the stone bridge up by three slopes to the
    lord's house; two stair roads; the market with the well; eleven houses with rooms (the lord's,
    the inn, Agnes's, a well-off house, Hollis's, the smithy, cottages, Thornfield's farmhouse and
    barn); the cemetery's fence and trees; the dry bed a layer down with two pools, the stone
    bridge, a footbridge and one with a plank gone; the dock with the rails and two carts.  Tested:
    the road climbs from the dock to the market by the rules, a stair road climbs its terrace.
    Every place and size is a first guess to judge in play.  Open: the houses' doors are all on
    their ground floor's front (a house against the slope needs doors on more than one level and
    side); the lord's house is not yet dug into the hilltop; the farm's fields, the gravestones,
    the market's stalls; the bridge material shows Kenney's wood wall.
    **The inn and its cellar's chest (2026-11-08)**, `tests/inn.loft` (LOCATIONS.md § 1): with it,
    a house's `front: false`, its `openings` (an outer wall's cell made a door, a niche or a window,
    with an edge on its outer side) and its roof's `holes`; a cellar is one room; a place's intent
    takes `edges` of its own (the grate's frame); and in the picture a window edge beside open
    space is a grate's iron rail.  What the 3/4 view taught, a rule for every place: a door shows
    only in a south wall (the roof covers the others), and what stands against a house's north
    wall is hidden under its roof; a thing seen through an opening is the cell north of the one
    under it (a layer is drawn a row up).
  - **D1d. How a house was left**: closed up (locked, barred, shuttered, boarded, the ladder
    taken up) and neglect (a roof come down, a stair rotted, a basement flooded), dealt as events.
  - **D1e. Dealt anew**: `TW_SEED`, the deal written to the play log, the town played with several
    seeds to judge.

  After the town (owner, 2026-11-08): the other locations, with more threats, and the fight got
  right there (B1c): pressure and skill mixed, never a show stopper, a sneak or a puzzle always
  giving the best start of a fight.
- **D2. The hidden path, the brook, the hillside, the cave-in** — needs B8 (crabs), C1 (night).
- **D3. The Old Seam, first level** — the Kharn's home: built mechanics, C3 (their points).
- **D4. The Old Seam, second level** — needs B2, B3.
- **D5. The flooded lower tunnels** — needs B4.
- **D6. The Still Lake** — needs B5, B8 (slimes).
- **D7. The High Diggings** — Sable's ground: built mechanics, C3 (his day).
- **D8. Stormcap** — needs B4 (the lightning), B8 (bigfoot), C3 (the alchemist brought up).
- **D9. The climax and the endings** — needs C4, C5, C7; the whole world played through.

## Phase E — the 3D view on the Tanmatsu, then the hex grid

**The 3D routines are ported to the Tanmatsu, so the hex grid can come into the area designs
without losing a backend** (owner, 2026-11-09).  A hex world has no 2D pictures (Kenney's sheets
are square), so until the Tanmatsu draws 3D a hex area would play only in a browser.  The grid
question itself is WORLD.md § One game, square or hex.  This phase runs beside phase D; what is
not the owner's is ◇.

**The owner's rulings for it (2026-11-09).**

- **The panel's own resolution**, 800 × 480, not a small frame scaled up.
- **Drawing is reused between frames**, or it is far too slow.
- **The same abstractions as the 2D's** ([@PLN218](https://github.com/loft-lang/plans/issues/218)):
  the world is drawn again only when it changes; the hero, the people, the monsters and the items
  in motion are drawn every frame.
- **Those are simple sprite blits**, not drawn through the 3D routines.
- **The game logic does not know how it is drawn**: OpenGL and the Tanmatsu's drawing sit behind
  one abstraction, so neither influences the game logic (E1).
- **The Tanmatsu's side lives in libraries**: the drawing, the sprites and the frames for the
  device are libraries' code, not the game's.
- **The Tanmatsu reads the hex world as any graphics backend does**, and the OpenGL version does
  the same optimisations the hex mesh code already has.  The hex world itself is made with the
  drawing kit, houses at 30° angles and walls thinner than a cell (WORLD.md § One game, square
  or hex).
- **As few triangles as possible**: a face whose back is to the eye is skipped, and sight lines
  (the rules already have some) leave out what cannot be seen.
- **Far away, triangles become dots**: cheaper to draw than a textured triangle.
- **First a test routine**: the demo world in 3D on the Tanmatsu, to learn how to make it fast.

**Measured (2026-11-09, a probe over `render3d::faces` in Orbury, 6 spots × 4 facings each).**

| | Faces built | Out of the view cone | Back to the eye | Out of sight (the hero's layer) | Left to draw |
|---|---|---|---|---|---|
| Outdoors | 982 | 67 % | 25 % | 46 % | 147 (15 %) |
| Indoors | 1446 | 66 % | 31 % | 82 % | 72 (5 %) |

The columns overlap, so they do not add up.  What it shows:

- **Culling leaves about a hundred faces**, so triangles are not the cost.  **The cost is
  pixels.**  On the 800 × 480 panel every face left is 16 pixels or more across, two-thirds of
  them over 32, and a full picture fills all 384,000 pixels.
- **No face is near a dot within today's reach** (12 cells, fogged out from 10).  A tile's 16
  texels cover fewer than 16 pixels only beyond about 21 cells.  Dots pay once the view reaches
  further: outdoors, and in the hex worlds.
- **The faces are not all wound the same way** (checked by hand on a floor's corner order and a
  wall's north side).  It never mattered with no culling, but back-face culling needs one winding.
- The sight test used here is the rules' line of sight on the hero's layer only: an estimate.
  The 3D view sees across layers (down from a terrace, up a stairwell).

**The hardware** (loft's TANMATSU.md; the BSP and ESP-IDF's display driver, read 2026-11-09).

- Two RISC-V cores with a single-precision FPU only (a `float` is software, so the 3D code runs
  under `#width 32`); 768 KB of fast SRAM, whose largest free block is only 37–38 KB, and 32 MB
  of slower PSRAM.  A 16-bit 800 × 480 frame is 768,000 bytes.
- The panel (an ST7701 over MIPI-DSI, portrait 480 × 800) has no picture memory of its own: a
  DMA reads the current framebuffer in PSRAM 60 times a second, 46 MB/s of PSRAM's bandwidth
  taken all the time.
- **The driver keeps one to three framebuffers** (`num_fbs`).  Handing it one of its own buffers
  switches to it at the next refresh, with nothing copied; any other buffer is copied in by the
  DMA2D engine in the background.  `loft_esp.h` asks for one today, and `screen` keeps a buffer
  of its own besides and copies it every frame.  @PLN218 already plans two.
- **What a full 3D picture costs there**, measured by `nullislandspace/synthengine3D` (a
  software 3D engine for the Tanmatsu; read for comparison only, it has no licence): at 800 ×
  480, 148 ms a frame drawn straight into PSRAM, 92 ms drawn in bands of 32 columns in SRAM;
  at a quarter of the pixels, 28 ms.  So a full picture is affordable now and then, never every
  frame.

**The model: the landscape and the actors, as in 2D** (owner; @PLN218 in 3D) ◇.

- **The landscape** is everything that stands still: the ground, walls, roofs, ceilings, things at
  rest.  It is drawn with the 3D routines into a kept picture **B**, its colour and its depth,
  only when it changes.
- **The actors** are the hero, the people, the monsters and the things in motion (a pushed
  cart, a thrown bomb, a carried torch).  Each frame they are drawn as **sprites**: a figure's
  picture scaled by its distance, turned and leaned as `motion.loft` poses it, and its pixels
  drawn only where they are nearer than B's depth.  So a wall in front hides an actor with no
  extra work.  This is @PLN209's `sprite` primitive with a depth added, the same call as in 2D.
- **A thing is landscape while it rests and an actor while it moves** (@PLN218): an item
  picked up leaves B (one rebuild) and is drawn with its carrier; put down, it goes back.
- **A frame** puts back from B the rectangles where actors were (in this framebuffer, drawn two
  frames ago) and draws the actors where they are now.  The rest of the frame is untouched.
- **When B is drawn again**: the rules say the landscape changed (a door, a push, a flame lit or
  out), or the camera moved too far.  How far is too far depends on the camera:
  - **the fixed camera from the south** (RENDER_3D.md § The camera, decided 2026-11-05, not yet
    built): it never turns, so walking shifts the picture as the 2D scrolls — exactly, when its
    projection is parallel (measured, E0 below).  B is shifted and only the strips that come
    into view are drawn; it is drawn whole only when the place or the layers shown change;
  - **the first-person view** (built, in the browser): every step and every turn changes the
    whole picture, so B would be drawn again every frame while the hero moves, about 100 ms at
    800 × 480.
  - **Open for the owner**: which camera the Tanmatsu shows.  The test routine measures both.

**One mesh for every backend** ◇ (surveyed 2026-11-09).  The hex mesher is moros's `hex_mesh`
(project code, not published, 3,300 lines).  It builds the hex world in chunks of 8 × 8 cells
(with a margin of 2), one mesh per surface: ground, road, wall, roof, floor, rock.  The OpenGL
view uploads those meshes; the Tanmatsu's renderer (E2) draws the same faces into B.  Neither
reads the world itself, and render3d's square faces cross the seam (E1) the same way, so the
renderers do not know which grid they draw.

- **What it already does, so both backends get it:**
  - only the chunks that changed are built again, and `graphics`' `GroupVboSet` uploads only
    those (dryopea's terrain mesher, built the same way, takes 7 ms instead of 54 ms for an
    edit of one hex);
  - a wall is one flat quad per run (`hex_draw`'s `surface_quad`), not a strip of edges;
  - a roof is two planes: 18 vertices against 189 for a staircase of cells;
  - rock sides only where the height steps.
- **What it does not do yet, added in the mesher so OpenGL gains it too:**
  - every wall is emitted twice with opposite normals and drawn with no culling: one winding,
    and back faces culled (E4);
  - no culling to the view: by chunk, from the bounds `GroupVboSet` already keeps (E4);
  - nothing simpler far away: dots (E5);
  - each hex top is its own fan of 6 triangles: a field of one material merged into a few
    polygons, from `hex_field::trace`'s exact outlines.  On a CPU every triangle has a set-up
    cost, so many tiny ones cost more than their pixels.
- **It moves into a library** (the Tanmatsu's side in libraries, owner): moros's to give,
  loft-libs-world to hold.  **Open for the owner** and moros's agent.

**Moving the camera without drawing B again** ◇.

- **Turning: a ring, exact.**  Turning about the eye has no parallax, so B is drawn all the way
  round the eye as a ring of flat slices (8 of 45°, drawn by the same renderer).  A screen column
  is then one column of one slice scaled by one factor, and turning picks other columns: no
  drawing, and the same picture a full redraw gives.  With the panel mounted portrait each such
  column is one panel row, so a turned frame is 800 scaled row copies.  The ring is about
  2,160 × 480 pixels, 2 MB of colour and 2 MB of depth in PSRAM, about 2.7 screens of drawing per
  place it is drawn from.
- **Looking up and down: a shear** (owner, 2026-11-09: expected on the Tanmatsu's hardware).  The
  picture is shifted up or down instead of drawn again, as the engines of Doom's time did.
- **Walking: approximate until the next B.**  A move of the eye has parallax (near walls slide
  faster than far ones), which no 2D trick gets exactly right.  So:
  - **the second core draws the next B** in the background, ahead of where the hero is going
    (@PLN218 names the second core for rebuilds; our C engine can pin a task to it);
  - **in between, the last B is moved**: zoomed toward the middle for a step forward and shifted
    for a step aside (the P4's PPA scales in hardware), or each pixel moved by its depth
    (reprojection, as VR headsets do between frames: closer, but small holes where something
    near uncovers what was behind it);
  - **the actors stay exact**: drawn every frame where they are, so the hero and the monsters
    never lag.  Only the background is approximate, until the next B arrives.
- **The fixed camera from the south** needs only a slide: it never turns.  **Measured
  (2026-11-15, `bench/fixedcam.c`, below): with a parallel projection the slide is exact; with the
  perspective of RENDER_3D.md's defaults it is not**, however narrow the field (near rows move
  further than far ones).

- **E0. The test routine** (owner).  The demo village in 3D, run on the desktop and on the
  Tanmatsu.  A scripted path holds still, turns on the spot, walks a street and enters a house
  while a figure moves, once with each camera.  Every frame logs its times as `@t` lines, as
  `sheet.loft` does: drawing B (and how often), putting rectangles back, drawing the actors,
  showing.  The ring, the shear, the background core and the two ways of moving B while walking
  are switches too, judged by their artifacts as well as their times.  Each step below is a
  switch in it, so each is measured on the device, alone and
  together.  Red: the routine's summary (frame time per part of the path, per camera) is missing
  a part.
  - **Measured off the device first** (owner, 2026-11-09: so we need few uploads) ◇.  The
    routine is C, so it runs under `qemu-riscv32` (the Tanmatsu's instruction set, as loft's
    `c_sweep.py --riscv32` runs it) as well as on the desktop.  Each frame counts its own work:
    pixels drawn into B, put back from B, sprite pixels tested and written, bytes read and
    written in PSRAM.  One run on the device sets a time for each kind of work; from then on
    the counts predict the device, and an upload is needed only when the memory layout changes
    or to check a prediction.  The emulator's own times rank two ways of doing the same work,
    nothing more: it does float in software (about 10 times an integer step, measured, where the
    P4's FPU makes them close) and its memory is the PC's (SRAM against PSRAM does not show).
    A QEMU built with plugins would count instructions exactly; Ubuntu's has none.
  - **The actors, measured first: `bench/actors.c`** (`bench/run.sh` runs it on the desktop and
    under the emulator) ◇.  B, its depth and the framebuffers at 800 × 480, laid out as the panel
    is, and figures scaled and depth-tested as E3 draws them, in three ways: the changed
    rectangles with two framebuffers flipped; the same in one buffer then copied whole to the
    panel's (as `screen` does today); and everything from scratch (what a frame costs whenever
    B moved).  Every frame of the first is checked against the second's picture from scratch.
    Per frame, PSRAM read and written (2026-11-09):

    | Scene | Changed rectangles, flipped | The same, copied to the panel | From scratch |
    |---|---|---|---|
    | 6 figures standing still | 0 | 1,500 KB | 1,671 KB |
    | 6 idle figures, a new picture every 12 frames | 67 KB | 1,534 KB | 1,671 KB |
    | The hero walking (200 px tall) among 5 idle | 259 KB | 1,726 KB | 1,740 KB |
    | Hero, 3 walkers, a thrown bomb, 6 idle | 451 KB | 1,911 KB | 1,821 KB |
    | 4 walkers close by (320 px tall) | 1,458 KB | 2,948 KB | 2,262 KB |

    - **A slow animation costs almost nothing, a quick one its own rectangle twice**: with two
      buffers flipped, each puts back where the figure was two frames ago and draws where it is
      now.  A walking figure 240 px tall moves about 250 KB a frame, one 60 px tall 20 KB.
    - **The copy to the panel costs 1.5 MB a frame whatever moves**, more than all the actors of
      a busy scene.  So the two framebuffers (`num_fbs = 2`) pay first, before any 3D.
    - **Figures close enough to fill the screen cost as much as drawing from scratch**, so a
      frame never costs more than that: the worst case is bounded.
    - Under the emulator, the frame's mean: 30 µs for the idle figures, 112 µs with the hero
      walking, 197 µs for the busy scene; the copy to the panel alone 230 µs.  What they take on
      the device waits for the calibration run.
    - Not yet in it: figures turned and leaned as `motion.loft` poses them, and B drawn by E2.
  - **The fixed camera's movements: `bench/fixedcam.c`** (`bench/run.sh fixedcam [PLACE]`).  A
    first E2: the landscape's faces and cards as `render3d` builds them (`tools/scene3d.loft`
    writes them out) drawn on the CPU into B, colour and 16-bit depth, at 800 × 480.  The
    rasteriser works in 1/16 pixel with integer edge functions and a fill rule that gives a shared
    edge to one triangle, so what it draws at a pixel depends only on where the pixel lies against
    the triangle: drawing a strip draws exactly what the whole picture has there.  Each camera
    movement is made by shifting B and drawing the strips that came into view, then compared
    pixel for pixel, colour and depth, with the picture drawn whole where the camera went.  Two
    projections: **parallel**, a cell 48 pixels wide, its ground 40 deep, a layer 48 tall (the
    2D's zoom 3, the ground a little shallower than a wall is tall: about the 40° of RENDER_3D.md
    § The camera), the camera's place a whole number of pixels; and the **perspective** of
    RENDER_3D.md's defaults (40° down, a 25° field).  In the demo village (994 faces), on this
    machine and under `qemu-riscv32` alike (2026-11-15):

    | Movement | Parallel: pixels that differ | Perspective: pixels that differ | Parallel: cost against drawing B whole (emulator) |
    |---|---|---|---|
    | a cell east or west | 0 | 10 % | 9–10 % |
    | a cell north or south | 0 | 17–20 % | 3–14 % |
    | a cell diagonally | 0 | 20 % | 21 % |
    | a layer up or down (the view rises with the hero's layer, as in 2D) | 0 | 12–14 % | 4–17 % |
    | ten follow steps one after another, B never drawn whole | 0 | up to 30 %, growing | 287,360 pixels drawn, 7.5 % of drawing each picture whole |
    | a smooth slide, 32 frames of 4–6 pixels | 0 | — | 1 % of the pixels |
    | the control: a cell east shifted a pixel short | 2.4 % | | |

    - **So the fixed camera is a parallel projection** (owner, 2026-11-15; the 25° perspective
      was a default, RENDER_3D.md § Defaults).  Every movement of it — a follow step, a smooth slide, a
      layer — is a shift and a strip, exactly the picture drawn whole, and stays exact over any
      number of moves.  Its texturing is affine and exact too: no division per pixel, as the
      P4 wants.  With the perspective every step leaves a tenth to a fifth of the picture wrong,
      and the errors add up.
    - Drawing B whole: 5 ms on this machine, 168 ms under the emulator (its floats are software
      calls; the P4 has an FPU); what the device takes waits for the calibration run.
    - **A rooms flip, a door, a change of the layers shown** (stepping under a roof) draw B whole:
      nothing of the old picture is in the new one.
    - Not yet in it: the hero's cut-away (a wall in front of the hero taken out: a landscape change
      round the hero at each step, so a rectangle of B drawn again, not all of it), the actors
      over B (`bench/actors.c`'s part), and a landscape change drawn into a rectangle.  All three
      are in raster3d (below).
  - **raster3d: the engine, landscape and actors together** (2026-11-15).  `local/raster3d/` (a
    library kept here until it moves to loft-libs-graphics; `fetch-libs.sh` copies it into
    `libs/`), C with loft bindings, libc only.  The rasteriser sets up each triangle once, apart
    from the camera; draws a column as an exact span (no test per pixel), stepping its depth,
    texture and light in integers (no division, no float per pixel); and finds the triangles
    under a strip through bins of 64 pixels.  **The landscape is kept in tiles** (2026-11-15, as
    moros's `hex_mesh` keeps its mesh in tiles of 8 hexes and `graphics::GroupVboSet` uploads only
    the dirty groups): a tile is the faces given under one key (`set_tile`, `drop_tile`), drawn in
    the order of the keys, and a tile given again is set up and drawn again only where it was and
    where it is.  Each tile keeps its faces' corners, so a new depth origin (`build_scene`, when the
    scene has gone far from the old one) sets them up again in C without the game giving them
    again.  **The light is data** (owner, 2026-11-15): a face says which cell it is lit from and
    by how much (`r3_face_lit`: cell, layer, factor), the engine keeps the cells' light
    (`r3_light`, `set_light`), and when a cell's light changes it lights again the triangles lit
    from it and draws B again where they lie — no face given again.  When a tile is dropped,
    the light of the cells no tile is lit from any more is forgotten (each tile knows the box of
    cells its faces name; the store keeps those, a cell round them), so the store holds what the
    kept tiles read, not every cell the walk ever lit.  B is shifted when the camera moves; a group of faces
    (one cell's walls on one layer) is hidden or shown by drawing the rectangle it covered; each
    frame a framebuffer is made from B where something changed since it was last made, and the
    actors are drawn there, depth-tested.  An actor drawn exactly as the buffer last drew it costs
    nothing.  **The hero is never hidden** is the engine's: `r3_uncover` hides every wall group
    that covers the hero's card and reaches nearer than it, and shows again the ones it no longer
    covers (RENDER_3D.md § The camera and what is seen).
    - **Checked: `bench/frame3d.c`** (`bench/run.sh frame3d [PLACE]`): a scripted walk of 600
      frames — the hero stepping, five figures pacing, wandering and standing, the walls in front
      of the hero cut away, the camera following a cell at a time or sliding — every frame
      compared with the whole scene drawn from scratch.  In the demo village and in Orbury's town
      (3,176 faces): **0 of 600 frames differ**, on this machine and under `qemu-riscv32`, with
      both cameras; the control (the hero a pixel off after the frame) differs in all 600.  The
      landscape is given in tiles of 8 cells, and in the runs with changes it changes under the
      walk too — the hero's tile given again with another light, the one beside it dropped and
      given back, the depth's origin moved, and every 23 frames the light of the cells round the
      hero — 56 changes, **0 of 600 frames differ**; built with `-DR3_SKIP_DIRTY` (a change not
      drawn again, the control) 525 do.  Every cell round the place lit, then the tiles west of
      the hero dropped: of 32,805 cells' light 4,385 kept, and the frame, and the frame after one of
      them is given back, the scene drawn from scratch.  An empty tile with the camera over the world's origin
      (where an empty box lies) is drawn round: 0 differ (it crashed the engine before
      `meets` asked whether a box is empty).
    - **Measured** (Orbury's town, 2026-11-15): a frame takes 33 µs following a cell at a time
      and 51 µs sliding on this machine, 0.25 ms and 0.67 ms under the emulator; the whole scene
      from scratch 1.2 ms here and 9 ms under the emulator (the first bench's rasteriser: 5.3 ms
      and 168 ms).  A sliding frame copies B whole into the framebuffer (152,000 pixels a frame
      on average); a following one copies 17,000.  What the device takes waits for a run there.
  - **In the game: `fixedview.loft`, F5** (2026-11-15; `TW_VIEW=fixed` starts in it;
    `tools/fixedshot.loft` takes its picture without a window).  The landscape round the hero in
    tiles of 8 x 8 cells, each `render3d::faces_box` and `cards_box` of its cells on the layers the
    fixed camera shows: all of them out to 16 cells when the hero comes to a place or a layer (or
    night falls or day breaks: every tile's light at once), from one read; as the hero walks, the
    tiles coming into view built at once and the rest ahead of it a few a frame (3 ms of the
    frame), nearest first, and those left 24 cells behind dropped.  **What changed is told by what
    changed it** (E1's counter, in `tile_world` 0.10.0: `changes`, `changed_since`), as hex_mesh's
    editor marks its dirty chunks: a tile whose cells (or the two round them) changed is built
    again at once; one within 12 cells of a change (its light, a thin wall's picture along its
    line) is suspected; suspects are looked at while the frame's budget lasts — built again, their
    light sent, and given to raster3d only when their faces differ, as `mesh_crc` tells two
    meshes apart (a tile's faces hold no light: each names the cell it is lit from).  A light that
    moved, dimmed or went out (the rules' light list, compared each frame: a torch carried,
    burning down, flickering) sends the light of the cells it reaches, its layer only: no tile is
    built again for it.  And, while the game says something changed, every tile in turn, one every
    8 frames — what the rules hold and the world does not (a door shut until something holds).
    **A thing whose picture moves (a flame: `theme::thing_moves`) is an actor** (owner,
    2026-11-15): its tile leaves it out (`render3d::cards_box`'s `moving`) and keeps its cell, and
    each frame it is drawn over B as the figures are (`render3d::thing_cards`), so its tile is not
    built again as it flickers, and an unchanged frame of it costs the engine nothing.
    **The faces seen only from below are left out** (`faces_box`'s `from_below`): a ceiling, the
    rock over a slant's open half, the underside of a floor, a wall or a roof.  The camera looks
    down, so it sees only their backs, and with no culling those were drawn: under rock (the torch
    room) the room's ceiling lay over it as a lid and hid the hero.  The first-person view keeps
    them.
    The figures (`game::figures`) as actors, a body card and what each holds, lit from their
    tile's kept light (`render3d::light_at` cost 420 µs a figure a frame); the camera sliding to
    keep the hero within 2 cells of the middle.  The frame goes to `screen::show`, as the 2D's
    does.
    - **render3d reads the cells once** (`render3d::Cells`, as hex_mesh reads its halo grid): a
      box of cells and two round it, a layer under and over, each cell's kind, what it is (solid,
      ground, whole, a pitched roof's, upright, with things), its rise, slant and ceiling, and its
      north and west edges, in one pass; a cell outside the box is read from the world the same
      way.  A world lookup costs 0.19 µs natively (a chunk's hash) and a vector read 7 ns; a
      cell's corners, banks and sides asked their neighbours from the world over and over.  The
      faces are the same: every place's faces and cards at its start, written out whole (1.2 MB,
      38 places), byte for byte before and after; a cell's rise read with one bit fewer differs.
      The light is kept the same way (`render3d::Light`, the rules' shades over a box).
    - **A tile's build, made cheaper** (2026-11-15, measured natively in Orbury's town, 125 faces
      a tile): the cells read with `tile_world::block` (tile_world 0.11.0: a box read at one chunk
      lookup a cell) 1.8-2.2 ms → 0.56 ms; the light (`tile_world::shades` reading its window once,
      0.11.0; and the layer under the lowest shown no longer read, its faces left out) 3.7-4.5 ms
      → 1.52 ms; the faces (the stairs' kinds looked up once a build, not two or three times a cell
      at 0.5 us a name; each corner height worked out once, where a bank asked its neighbour's
      again; `light_of` not making a vector at each call) 3.1-3.7 ms → 1.79 ms.  **A tile: about
      9.6 ms → 3.9 ms.**  The tiles that come into view in one frame are built together, from one
      read of their box: a tile's light is mostly its margin (one layer over one tile 0.45 ms, over
      five in a row 1.0 ms).  Every place's faces and lights, written out whole, unchanged.
    - **Checked: `tools/fixedshot.loft PLACE OUT.png MOVES [change]`** walks the moves a frame at
      a time, `change` moving a thing and raising a wall halfway, and at the end compares the
      frame with the same moment built fresh, every tile from its own read: **0 pixels differ**
      (the demo village, three walks, one with the change; Orbury).  Its controls: the tiles in
      view not built ahead, 326,496 pixels differ; a changed tile found but not given again, 7,080
      differ with the change and 0 without it; with the counter's tiles not built again (and no
      sweep), 7,080 differ.  `change` also puts a burning torch in the hero's hand: in the torch
      room 79 light changes, 0 differ; with the light not sent (and no sweep), 44,774 differ.
    - **The figures posed as the 2D poses them** (2026-11-15): a figure's card turned by its
      pose's tilt, the hero leaning the way it faces and mirrored facing west (`game::figure_turn`,
      which `draw_figure` uses too), about the bottom middle of its picture; what it holds in its
      carried picture at its hand (`game::hand`), turned by the swing about its grip and pushed out
      by its reach (`theme::held_place`, which `put_held_fine` uses too), behind it facing north;
      what is on its shoulders behind it.  The engine turns an actor about a point of its picture
      and mirrors it (`r3_actor_pose`; its sine needs no libm, within 6.3e-9 of libm's); the bench
      poses the hero and swings what it holds every frame, and every frame is still the scene
      drawn from scratch.
    - **The status line and the talk bubbles**, as the 2D draws them (`game::draw_status_on`,
      `game::talking`, `game::draw_bubble`, which the 2D's `draw` uses too): drawn at the world's
      pixels on a surface a third of the frame's size, each bubble over its speaker's card as the
      camera sees it, and laid over the frame three times larger by the engine (`r3_overlay`, NONE
      where it is clear; where it changed the frame is made again).  The bench's overlay — a
      growing status bar and a bubble following the hero — keeps every frame the scene drawn from
      scratch; not drawn again where it changed (the control), 594 of 600 differ.
    - **The scene contract** (SCENE_CONTRACT.md, a proposal, owner 2026-11-15): the view written
      once against a scene — tiles of faces lit from cells, the cells' light, groups, posed actors
      (cards or models), the overlay, a camera — drawn by raster3d on the CPU (the fixed camera
      only, the Tanmatsu) or by an implementation on `gpu` (the free camera too); what raster3d's
      binding changes to meet it.
    - Not yet: `screen` showing
      raster3d's own two framebuffers on the Tanmatsu (today the frame is handed over as pixels);
      a run on the device.
- **E1. One seam between the game and its drawing** (owner, 2026-11-09: OpenGL and the new code
  must not influence the game logic) ◇.
  - **From the game to a view: what is there, never how to draw it.**  The landscape (the cells,
    with a counter raised whenever something in it changes: a door, a push, a flame lit — built
    2026-11-15 in `tile_world` 0.10.0, owner: every change goes through it, counted by blocks of
    8 x 8 cells, so a view asks `changed_since` of the box it read; the fixed view uses it), the actors (who, where, facing, the pose `motion.loft` gives, what they carry) and the
    overlays (speech, the cursor, the intent marks).  The landscape and the actors are two lists
    (today the cards are faces drawn with the rest).  A view builds its faces or sprites from
    them on its own side.
  - **From a view to the game: only acts**, the ones keys give: a step, a turn, an act on a
    cell.  Today the 3D view turns the hero by writing `g.world.face_x` and `face_y` itself
    (`render3d::steer`, `after`).  Turning becomes an act the rules perform, so it is in the
    play log like any other.
  - **Behind the seam, the views**: the 2D tiles (desktop, browser, Tanmatsu), the 3D on the
    GPU (OpenGL; WebGL in the browser) and the 3D on the CPU (the Tanmatsu, E2 and E3).  Each
    keeps its own caches (B, the GPU's meshes, the atlas) and decides from the counter when to
    build them again.  How long a view takes never changes what the rules do: they tick on their
    own clock.  The Tanmatsu's views are libraries (owner; E2, E3); the game keeps only what
    is there and which view shows it.
  - **Today**: the rules import no drawing already.  `game.loft` draws the 2D itself through
    `tiles` (about 40 calls; `Game` holds the surface), and `render3d.loft` mixes what is seen
    with the GPU's calls.  Both move behind the seam: the 2D drawing out of `game.loft`, and
    `render3d.loft` split into the faces of what is seen and the GPU's drawing of them, so E2
    draws the same faces on the CPU.
  - Red: (a) a check that no game-logic module (`rules`, `motion`, `game`, `talk`, `intent`)
    uses `tiles`, `graphics`, `screen` or `render3d`; (b) one walk-through played with the 2D
    view, with the 3D view and with none gives the same play log; (c) the frames in the browser
    do not change (`checks/view3d.loft` and a TW_SHOT before and after, byte-identical).
- **E2. Drawing B on the CPU** ◇.  The landscape's triangles drawn in software into B: textured
  spans with the atlas's texels sampled nearest, each vertex's light from the rules, a depth
  test.  **In a library** (owner), as its C part, the way `tiles` draws its pictures: a new one in
  loft-libs-graphics (working name `raster3d`) ◇ that takes faces, an atlas and a camera and
  draws a colour and a depth picture, so another game (crawler, moros) can use it too.
  `graphics::Canvas` is not it: it has only flat triangles and draws pixel by pixel in loft.  Drawn in bands of 32 columns in the
  fast SRAM; with the panel mounted portrait, a band of the picture's columns is a run of the
  panel's rows, one block of memory.  Red: B drawn by this renderer and by the GPU differ beyond
  a stated number of pixels (the tolerance names what affine texturing bends).
- **E3. The actors as sprites, and the frame** (@PLN218) ◇.  Two framebuffers, flipped at the
  refresh; the rectangles put back from B; the actors blitted, depth-tested against B.  **In
  libraries** (owner) ◇: the depth test added to `tiles::sprite` (it already stretches and turns
  a tile); the changed rectangles and the flip with @PLN218's engine; two framebuffers in
  `screen` and loft's `loft_esp.h`.  `bench/actors.c` moves to that library's bench then, as
  `tiles` has `bench/frame.c`.  Red: at every frame of the routine, the picture differs from
  drawing everything from scratch.
- **E4. Culling, so B is drawn faster** ◇.
  - **Back faces**: every face wound one way, its front toward its open side; a thin wall is
    two-sided (two faces).  Red: a check that every face's front points to its open side.
  - **The view cone and the distance**: faces outside the field or past the fog are skipped,
    a block of cells at a time.
  - **Sight across layers**: which cells can be seen from the hero's cell, layer by layer through
    the openings between layers, worked out when the hero changes cell.  A face belongs to the
    open cell it faces.  It must never leave out anything visible: a gap is worse than a spare
    triangle.
  - Red: on the desktop GPU, a picture drawn with culling differs from one drawn without it, at
    each spot of the routine's path.
- **E5. Dots far away** (owner) ◇.  Each tile in the atlas gets smaller copies (16, 8, 4, 2 and
  1 texels).  A face uses the copy that fits its size on the screen, and the 1-texel copy, the
  tile's mean colour, is the dot.  It also stops the pictures shimmering at a distance.  It
  matters only when the view reaches beyond about 20 cells.  Red: the faces drawn as dots
  against their distance in the routine's log.
- **E6. On the device, in the game.**  The 3D view on the Tanmatsu, F1 switching it as in the
  browser; the 2D stays.  Checked on the device with the timing log: frame time, B's rebuilds
  per second, and the PSRAM the atlas, B and the faces take.
- **E7. Then the hex grid**, WORLD.md § One game, square or hex, steps 2 to 4 (the rules asking
  for the next cell, a hex world, the drawing kit filled into hex).  The hex world is drawn
  through the hex mesher, on every backend.  **No hex area in a design before E6 plays on the
  device** (owner, 2026-11-09: no backend lost).  Red: a hex place played through by its
  walk-through test, and drawn on the desktop, in the browser and on the Tanmatsu.

◇ Open: the loft repo plans a column renderer for hex worlds on the Tanmatsu (loft's TANMATSU.md
§ One world, drawn at more than one fidelity): walls as upright spans and floors through a
division table, cheaper than triangles.  It draws upright walls and flat floors, which suits a
hex world, but not this game's slopes, slants, pitched roofs or bent floors.  E2 draws triangles
so that everything the browser shows is kept; whether the column renderer takes over on hex
ground, where it fits, is for the owner once E0 has measured the device.

## Where to begin

A1–A3 and B1 change little and unlock the rest.  The first step with a picture worth playing is
**B4** (the pump, with the spare crystal's first charge), because D1, the village, needs its small
form; with C2 it makes D1 the first story place.
