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
- **One number** (owner, ruled 2026-11-08): the measures are dimensions of one challenge, folded
  into a single number, so that when one of them gets deep it weighs heavily on the whole.

The owner on houses (2026-11-08): § Houses: where the parts start.

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

## Houses: where the parts start

The owner (2026-11-08):
- **Start with the town.**
- **The inside of a house is not a place of its own.**  It is part of the town's map, so climbing
  up outside gets the hero to an exact spot on the house: the roof over the room below it, the
  upper window of the room behind it.
- **The houses get bigger** on the starting map for it, and the owner likes the sense of scale
  they bring.
- **Parts of a house are templates**, and a house can have several levels: floors, a basement, a
  roof.  Some have only one (a poor miner's family); some have more elaborate floors with more
  distinct rooms.

**One model, inside and out** (proposed).  A house is its rooms on layers, and its outside is that
same house seen from the street: its walls are the rooms' outer walls, its roof is the layer over
its top floor, its windows are where its rooms have them.  Nothing is drawn twice, so nothing can
disagree: a roof walked on lies over the room it covers, a window climbed through lands in the
room behind it.  The rules already show it this way (DESIGN.md § What the player sees): under the
floor above, only the hero's layer is drawn, so the roof and the upper floor vanish as the hero
steps in; outside, with no ceiling, the layer above is drawn, so the roof is there.  It is the
house EDITOR.md § 3 drew (walls, door edges, the roof on the layer above, cut away while the hero
is inside); the kit's `houses` (intent.loft), today a house's outside only, grows into it.

**On a hill** (owner, 2026-11-08: the town is a bit like a medieval Italian one, LOCATIONS.md
§ 1).  A house against the slope has its floors on more than one street: its ground floor on the
lower street, its upper floor's door on the street above, and to the lower street it is a floor
taller than to the upper one.  So a house template states which of its levels meet the ground on
which side, and its ports are on more than one layer: a house is a way up the hill as well as a
house.

**Its levels**, a layer each (a layer is a wall high, 4 steps):

| Level | Layer | Reached by |
|---|---|---|
| basement | below the street | stairs down or a hatch inside; a cellar hatch from the street |
| ground floor | the street's | the front door, a back door |
| upper floor(s) | one up, and more | stairs or a ladder inside; from outside a window or a balcony, climbed to (a chair on a table, a cart, a stack) |
| roof | over the top floor, walked on | from a neighbour's roof, a tree, the hillside's ledge; down by a hatch or a chimney |

**Templates at two sizes.**
- **A room** is a part as in § How a part is written: a footprint on one layer, its ports
  (doorways, stairs, a hatch, a window), its furniture as a variant, what it may hold (a chest, an
  axe, rope), and its figures: who sleeps, works or eats there, and when (WORLD.md's points).
- **A house** is a frame of rooms on its levels, by its household; to the town it is one part,
  its ports the doors, windows, hatches and roof edges.  Two roofs that meet are two ports joined.

**Households** (proposed, from LOCATIONS.md § 1; the sizes are the outside, walls included):

| Household | Levels | Rooms | Size | In Orbury |
|---|---|---|---|---|
| a poor miner's family | ground floor | one room to live and sleep in, perhaps a lean-to | about 5 × 4 | the miners' cottages, some shut |
| a craftsman's house | ground and upper floor, perhaps a basement | the workshop or shop on the street, a kitchen behind, sleeping rooms above | about 7 × 6 | the craftsmen who served the mine, most gone |
| the alchemist's house | ground and upper floor, a vaulted basement | the shop where potions were sold to the miners, the lab (furnace, still, glass, the bench where small crystals are filled), the store of ingredients, living rooms above | about 8 × 7 | Hollis's (@ST-Alchemist): lived in, kept up |
| a well-off house (money from the ore) | ground and upper floor, basement, roof hatch | a front room, a kitchen, a store, several rooms above, a strong room or a cellar for what is worth keeping | about 8 × 7 | Agnes's, beside the inn (@ST-Innkeeper); the ore's other well-off ◇ |
| the lord's house: his representative's, and the lord's when he comes | basement, ground floor, upper floor, roof, a walled yard | a hall to receive in, the representative's office (the ore's tallies, the lord's share), a kitchen and the servants' rooms; above, the lord's chamber kept ready and the representative's rooms; below, a strong room; a stable in the yard | about 12 × 10, the yard besides | Lord Edric's (@ST-Court), the finest in the town |
| the inn | basement, ground floor, upper floor, roof | the taproom, the kitchen, a store, guest rooms above, the basement | about 12 × 9 | abandoned; the chest and a grey rat in its basement |
| the smithy | ground floor and a loft | the forge, open to the street, a store | about 8 × 6 | abandoned, to be made to work (@ST-Artefact) |

An abandoned or closed house is any of these, shut: dust, its furniture moved, a way in that is
not the door.

**Most houses are not spectacular** (owner, 2026-11-08): the village has a variety of houses,
some elaborate, most plain.  **It is a town where most people were lowly miners, but there was
money too** (owner): the ore was its wealth (STORY.md § 1), and some hands it passed through kept
some.  Proposed, for Orbury's dozen or so:
- **most are miners' cottages**, a floor and a room or two, close together: in rows low on the
  hill, along the stream, facing the dock across it, sharing walls, their roofs joined, so a row of roofs is a way
  of its own (two roofs that meet are ports joined, above);
- **a few are two floors**: a craftsman's;
- **two or three are elaborate**: the lord's house (below), the inn, which served the ore's
  traders, and the houses of
  those the ore made well-off ◇, of better stuff (stone, where the cottages are wood).  The
  notable houses stand round **a small market place beside the inn**, already higher up the hill
  (owner, 2026-11-08), so Agnes, beside the inn, has an eye on what the hero does there; the
  biggest mansion, the lord's, stands towards the top, partly built into the hilltop (owner).

**The lord's house** (owner, 2026-11-08): the house of the lord's representative, which doubles
as a place for the lord to stay if need be.  The lord is Lord Edric of Sunmarch, the local lord
who got much of his income from the mine (STORY.md @ST-Decree, @ST-Court).  Proposed:
- **Partly built into the hilltop** (owner): its back rooms are dug into the rock, the strong room
  deepest; its front faces down over the town, the market below it.  Its upper floor, or its roof,
  meets the hilltop behind at ground level ◇, so a way in from above lands at a known spot, and
  its tower, if it has one ◇, is the landmark seen from the dark hillside (LOCATIONS.md § 4's landmark
  to steer by).
- **It shows both uses**: the representative's rooms lived in, an office of ledgers and tallies;
  the lord's chamber kept ready though rarely used, the best bed under a cover, his arms over the
  door.  The stable in the yard is for the lord's horses and his escort's.
- **It is the town's most closed-up house** ◇: the lord fell from grace, the decree cut off his
  share of the ore, and his representative left with nothing left to collect.  The lord means to
  come back more than anyone, so it is closed with the most care: barred, shuttered, the strong
  room locked; and it has been empty since the decree, so its roof has had the longest to come
  down.  Whether the Wardens keep it sealed under the decree, so that entering it is more than
  snooping, is for the owner ◇.
- **It ties to the hero** ◇: the hero served in Lord Edric's army (STORY.md @ST-Hero), so he may
  know the house's ways (where the servants came in, where a key was kept), and what the hero
  finds there (a ledger, a letter, a treasure for the lord) bears on the ending where the young
  lord's favour is restored.  When it is, the house opens again: the lord's arms cleaned, the
  chamber in use ◇.

**The alchemist stayed** (owner, 2026-11-08), with a lab that is not easily replaced.  Proposed:
- **The lab is the house's heart**, and what makes it elaborate: a furnace, a still, glass, the
  ingredients, the bench where Hollis fills the small crystals (LOCATIONS.md § 1).  It is fixed
  and heavy, which is why it could not be taken along; set in a vaulted basement or a stone room
  of its own, as a fire and its fumes want.
- **A house lived in is a different puzzle**: no decay, nothing closed up, but someone home.  Its
  part is navigation by the clock (WORLD.md's points: Hollis at the lab bench, in the shop, asleep
  above), and the lab is a place the hero is shown, not one to break into (the crafting lessons).
  Coming in uninvited is snooping, as anywhere.
- **It changes with the story** ◇: the potions and stimulants it makes, the crystals it fills,
  what the hero brings it (ingredients, @ST-Alchemist); a lab that matters to the village's
  survival is one more reason the house stays kept.

**Money shapes how a house was left** (proposed).  The well-off could afford to leave first, so
their houses have stood empty longest, and they had the most to come back for, so they were closed
with the most care: barred, shuttered, the valuables (ore, STORY.md: ore stands for wealth) put
away in the cellar or a strong room.  The most care and the most neglect meet in the same houses,
and that is where the elaborate puzzles are.  The miners who left to find work closed their
cottages simply, with less to guard; the miners who stayed live in theirs, kept up, as do Hollis,
who stayed for the lab, and Agnes.

The plain ones vary in small things rather than in plan: their size (4 × 4 to 6 × 5), the side
their door is on, a lean-to, a porch, a shed, a yard or a garden, the furniture.  Plain is not
dull: a cottage's puzzle comes from how it was closed and what has come down, not from its plan,
so most closed cottages are a short way in and out, and the elaborate houses are where the town's
bigger puzzles sit (the inn's basement chest).  The seed deals the plain ones freely; the
elaborate ones are few, placed by the story, and dealt only in their rooms and their decay.

**The story fixes what, the templates deal how.**  The story fixes a house's household, where it
stands and what it must hold: the chest and the grey rat in the inn's basement, rope in an
abandoned house, axes in the closed ones, the smithy's anvil.  The seed deals its rooms, their
order and its ways in; the check makes sure what the story needs is there and can be reached.

**Where the town's puzzles come from** (owner, 2026-11-08): two things done to a house, one on
purpose and one by time.
- **Closed up, to come back to.**  Families often mean to go back to their homes later (the Fenns
  fled and might return, STORY.md § 3), so a house is closed the way a family closes it when it
  means to come back.  Proposed, as variants dealt by the household and the seed:
  - the front door locked, or barred from inside and the family gone by the back door, locked
    behind them;
  - the shutters closed, a window boarded (a window edge made a wall);
  - the ladder to the loft taken up, the hatch to the basement shut and weighted with stones
    (LOCATIONS.md's stones off a hatch);
  - the furniture pushed to the walls and stacked: a chair on a table, a step already there but
    in the wrong place;
  - what is worth something put away: down in the basement, up in the loft, in a chest.

  It reads as care: tidy, square, on purpose.  A closed house says someone means to come back,
  which is why the villagers mind the hero's snooping (LOCATIONS.md § 1).
- **Neglect.**  Too little maintenance, and time did the rest.  Proposed, laid over the house as
  it was closed:
  - **a part of the roof came down** (the owner's example): those roof cells open to the sky, and
    what fell lies under where it fell, rocks and beams on the floor below; heavy enough, it went
    through the upper floor to the ground floor as well.  Daylight comes in through the hole (a
    lit patch by day), and so does the hero, from the roof;
  - a beam fallen across a doorway; a stair rotted through (the stairs gone, a layer to climb);
  - a hole in an upper floor: a way down, a fall, a place to look down through (DESIGN.md § What
    the player sees);
  - a wall slumped outward, its stones a slope up to the window above; a tree grown against a
    wall, a climb;
  - a basement flooded by the brook: what was put away there lost, its beams afloat (kinds.json:
    a beam floats).

  It reads as time: ragged, uneven, drawn by the seed.  Fallen stones follow the rules that are
  there: a loose rock is pushed, rocks touching are rubble that pushing cannot move (DESIGN.md
  § @GM-CartLift), a heap one high is a step.
- **The puzzle is where the two meet.**  The family closed every way in, and time opened one they
  did not mean: the door is barred, the roof came down.  And neglect closes what the family left
  open: the rubble lies on the stairs they left clear, the fallen beam blocks the room their
  things were put away in.  The same house dealt twice is another puzzle: closed with care and its
  roof fallen over the kitchen, or closed in haste and a wall slumped.

**Neglect is dealt as events, not cells** (proposed; WORLD.md: the developer depicts, the game
fills in): "the roof over these cells came down", and the game derives the hole, what fell, where
it lies and what it broke on the way.  A decay needs its cause: a roof comes down where it spans
widest, a stair rots in a damp house, a basement floods near the brook.  How much has come down
follows from how long the family has been gone, a number the seed draws per house; a house still
lived in is kept up and has none.

**Open, for the owner** ◇:
- the beams of a fallen roof are beams, wanted for the barricade and the flotilla (LOCATIONS.md
  § 1): may the hero take them, and is that the snooping the villagers mind, or the aid they
  thank (the village's own wood, put to the village's defence)?
- when a family comes back (the Fenns, LOCATIONS.md § Aid), it finds its house as the hero left
  it: what was taken missed, what was moved noticed?
- the same two sources serve the mine: closed up (the seam shut by the decree, its gates barred)
  and neglect (tunnels fallen in, beams rotted), so its parts could be written the same way.

**The town's puzzles as parts** (LOCATIONS.md § 1, ◇):
- a closed house entered by its upper floor: the window wants *a step* (a chair on a table), or
  its fallen roof is reached from the house beside;
- one whose way in needs a crate pulled out of a corner first;
- stones pushed off a cellar hatch, from the street;
- the chest on a roof ledge, seen from the street, reached from the house beside: across two
  houses' roofs, ports joined;
- the market under Agnes's eye (owner): the notable houses round it are entered within her sight
  from the house beside the inn, so getting into the lord's house or a well-off one unseen is a
  navigation part by her routine (at her window, out in the market, asleep), as is taking the
  bow from the inn (LOCATIONS.md: she had better not see it);
- the two ways up (LOCATIONS.md § 1, owner): the main road, long, open and seen from the market at
  its top, the only way a thing can be moved up the hill (a step, a crate, a cart); the stair
  roads, short and hidden among the poorer quarters, for the hero alone.  Which is taken, and what
  has to be brought up by the long way, is a choice a scene can turn on;
- the dried stream's bed (LOCATIONS.md § 1, owner): a sunken way through town, a navigation part
  under the bridges, out of the street's sight; its bridges neglected as the houses are;
- at night a lived-in house is a navigation part: the sleepers in their rooms see what is lit,
  and a torch gives the hero away (LOCATIONS.md § 1), so crossing it is measured as any other.

The town is the first depth: its challenge is judged in play (below).

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
- **The exactness measures**, from those (each a dimension of the one number, below):
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

### One number

The owner's ruling: the measures are dimensions, folded into one challenge, where a dimension
that gets deep weighs heavily.  Proposed:

1. **Each dimension on a scale of 0 to 10**, 10 being as far as the hero's moves can go:

   | Dimension | 0 | 10 | Between |
   |---|---|---|---|
   | timing: the tightest window | 1000 ms or more | 150 ms | by the ratio, not the difference: 10 × ln(1000 / w) / ln(1000 / 150) — 600 ms is 2.7, 300 ms is 6.3 |
   | density: the tight moments | none | 8 | 1.25 a moment |
   | breath: the longest run without a safe wait | none | 10 s | 1 a second |
   | precision: the forced moves | none | 10 | 1 a move |
   | reading: the hidden things, weighted | none in sight-lines | 10 | in the dark 1, round a corner or on another layer 1, only heard 2, cause and effect apart 3, in another part 3 |

2. **Folded as a cube norm**: *challenge* = ∛(d₁³ + d₂³ + … + d₅³).  The cube makes the deepest
   dimension lead: one dimension at 8 and the others at 0 is 8, deep on its own; all five at 3
   is 5.1, a middle scene.  Many mild demands add up, but never to as much as one sharp one.
   The power (3) is the knob: higher, and the deepest dimension counts almost alone; 2, and
   breadth counts more.
3. **The leading dimension is named** with the number (the one with the largest cube), so the
   dealing routine knows what to swap (below).

Two things stay outside the number, as limits in every band: a window under 150 ms (more than the
hero's moves can give: a dodge takes 100), and a danger met with less warning than its tell.
They are never dealt, however the rest adds up.

A slip is measured as failing, though in play it only costs (DESIGN.md: hurt, rarely killed): a
slip in the early places costs a hit and a step back; deeper, more.

## The depth sets the band

Each scene's depth gives a band of the one number.  The routine **deals** a scene:
1. draw a seed, fill the slots, check it solvable (above);
2. measure its challenge, and its leading dimension;
3. in the band: keep it.  Too much: soften the leading dimension — swap the part that holds it for
   a gentler one, or its variant (a wider round, the lever in sight of its junction, the role in
   a nearer slot).  Too little: sharpen the leading dimension, or the one with most room to
   grow — a harder part in the same slot, a lair's spot nearer the way, a round's phase shifted,
   a socket's torch out, the role in a farther slot.  Then measure again;
4. after a set number of tries, keep the nearest to the band.

Sharpening the dimension already leading gives a scene with one clear character (a timing
scene, a reading scene); sharpening the one with most room gives a scene of many mild demands.
Proposed: the first, mostly, as a scene that is about one thing reads better; the second now and
then, drawn by the seed, for freshness.

Proposed bands, by LOCATIONS.md's order:

| Depth | Places | Challenge | Traps |
|---|---|---|---|
| town | Orbury, this side of the stream (LOCATIONS.md § 1: the stream is the line) | 2 at most | none |
| first | across the stream: the dock, the camp, the hidden path, the hillside | 3.5 at most | none |
| middle | the Old Seam's first and second levels, the flooded tunnels, the lake | 3.5 to 6.5 | seen, undone by R |
| deep | the High Diggings, Stormcap | 6.5 to 9 | seen |

The town and the first places have a ceiling only; the deep ones a floor and a ceiling, the ceiling kept
below 10 so a deep scene leaves the player some room to be wrong.

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
seed  parts (slot: part, variant)                  solv  plan  inter  timing  density  breath  precision  reading  challenge  leads    traps  band
   7  gate: dark room, cache: tar+sticks, lair: rats   yes      5      2     0.6      0.0     1.2        1.0      1.0        1.6  breath       0  first
  12  gate: blast wall, cache: bomb, lair: cyclops    yes      7      3     3.8      2.5     3.0        1.0      4.0        5.5  reading      1  middle
  13  gate: dark room, cache: bomb, way: cart loop    yes      4      1     7.3      3.8     4.2        2.0      3.0        8.2  timing       0  deep
  19  gate: blast wall, cache: tar+sticks, ...        NO       -      -       -        -       -          -        -          -  -            -  -
```

Seed 13 is a role turn at work: the cache gives a bomb, the gate wants a light, and the bomb
thrown into the dark room lights its sockets; the cart loop beside it, with its 250 ms window,
leads the number and makes it deep.  Seed 12 is middle on reading: its bomb lies in another part,
and its barred wall is round a corner.

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
2. **What do parts fill?**  Started (owner): the town's houses, in Orbury's map.  Open: later,
   slots in the other places of LOCATIONS.md, or whole dungeons made of parts?
3. **Judged in play** (owner, 2026-11-08): the bands, and which detail gets which level, are
   decided by playing the result, not here.  Every number above (the scales, the weights, the
   fold's power, the bands) is a starting point.  For the judging, proposed: `TW_SEED=<n>`
   deals a place with that seed (as `TW_WORLD` picks a world); the play log writes each place's
   seed, its parts and its measures as the hero enters it; the editor deals the place it shows
   again with the next seed, to compare two deals of the same place.
4. **Settled** (owner): one number, the dimensions folded so a deep one weighs heavily.  Open:
   does the routine sharpen the leading dimension (a scene about one thing) or the one with most
   room (many mild demands)?  Proposed: mostly the first.
5. **Being seen**: does it fail the measure, as being hurt does?  Proposed: yes for the measure
   (it asks how exact a clean pass is), though in play a sighting starts a fight, not an ending.
6. **Interactions asked for**: may a deep scene require a number of interactions between parts,
   as it requires a challenge?  Proposed: yes, as a column of the band.
7. **Story facts as roles** (a key from a person, a lever shown by a villager's hint): later,
   once the story's places are built.

## Building it (proposed, in BUILD.md's small steps)

- **P1 — houses.**  Rooms and households as templates in `levels/parts/`; the kit's `houses`
  grown from a house's outside to its rooms on their levels (basement, floors, roof); a test
  street of three houses (a miner's, a craftsman's, the inn) dealt by a seed.  Its checks: every
  room reached, every roof cell over a room or a wall, every window landing in a room, a roof
  climbed to from outside landing at the spot over the room below.
- **P1b — the format for every part.**  The house's puzzles (the upper window, the crate, the
  hatch, the roof ledge) as parts with roles; one lair (the inn's grey rat).
- **P2 — the abstract search**, with its tests: solvable, decoration, interactions, traps.
- **P3 — the playback**: a plan stitched from the parts' solutions and played by the rules.
- **P4 — the measures**: the search over (cell, moment), the easiest way, its windows, margins and
  breaths; what the plan uses and what of it is hidden from where it is first needed; the warning
  every danger gives.  Tested on hand-made rooms whose numbers are worked out by hand.
- **P5 — dealing**: the scales and the fold, the bands, the swaps, the pool, the save's memory of parts met.
- **P6 — the sweep**: `tools/puzzles.loft`, and `tests/puzzles.loft` replaying the pool.
- **P7 — a first scene** in the demo village: a house of three slots, dealt anew each game.
