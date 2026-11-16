# One world, on the Tanmatsu and in a browser

The game is one world the hero walks through: out of a dungeon or a building, across the land
outside, into the next.  No levels, no screens between places, no order to play them in.  It runs
on the Tanmatsu and as a web page, and the page carries a full editor for every part of the game's
data.

## The developer's view

The developer always works in a simple view of the situation, and the game fills in the details:

- **simple to say**: a spot ("2 rats here, often"), a door between two places, a material on a
  cell, an NPC's home and work;
- **the game fills in the rest**: pools and bands, hours and activities, routes and timetables,
  a material's autotiles, a door's other side;
- **every detail can be adjusted**: the editor shows each detail the game derived and lets the
  developer set it by hand (a band of exactly three, a stay of an hour, a tile other than the
  autotile's).  An adjustment is kept apart from the intent and survives when the game derives
  the rest again; taking it away gives back what the game derives.  The editor marks what is
  derived and what was adjusted, so the simple view stays readable however many details were
  set by hand.

The same holds for the world itself.  A developer depicts a room with a bit of rubble, not
forty cells of floor, wall and stone:

| The developer depicts | The game fills in |
|---|---|
| a room: its outline, "a bit of rubble" | its walls autotiled, its floor's variants, the rubble scattered by the place's seed |
| a corridor between two rooms | its floor and walls, and the doorways at both ends |
| "an old cellar: damp, crates along the walls" | moss on the stones near water, crates against the walls, the cellar's dark |
| a yard outside a house, "overgrown" | grass and weeds over the path's edges, the fence's corners |

What the game fills in keeps the place playable: nothing scattered stands in a doorway, cuts off
a part of the room or covers a spot, and the editor's checks say when an intent cannot be met
(more rubble than the room holds with its ways kept open).  A cell painted by hand is an
adjustment like any other: kept apart, surviving when the room is filled in again, and the
filled-in cell back when the adjustment is taken away.  The materials already work this way: a
cell's tile follows from its material, and the rare tile set by hand is listed apart (EDITOR.md
§ 2).

## The world

**Layers are a 3D model; the grid is a choice** (owner, 2026-11-05).  The current engine models
little, but the layers already carry a full shape — a dome cave is its walls stepping in one cell
per layer up (intent.loft's `domes`, DESIGN.md § Bombs).  It is capable of rendering 3D; the
difference the enhanced engine makes is a hex pattern instead of cubes.  So what is built on
layers (heights, ceilings, domes, what holds) carries over; what assumes four square neighbours
(the eight-neighbour checks, the facing, the reach across a swing) is what a hex grid revisits.
The current engine's limits are felt already: tile_world keeps a terrain kind in four bits, so
sixteen kinds at most (a door held shut is one kind, the side of its beam kept in the level), and
all sixteen are taken since shallow water (2026-11-08).  How one game could play on either grid:
§ One game, square or hex.

**Places joined by doors.**  The world is a set of places: a dungeon, a building, a stretch of
land outside.  Each is a map of its own (a place file, the chunked format of EDITOR.md § 2).  A
door is a link from one place to another, both ways:

- a door has a cell, a side and a destination: another place and a door there;
- a place has as many doors as it needs — a front door, a back door, a cellar hatch, a gap in a
  wall — so a building or a dungeon can be left another way than it was entered;
- the hero steps through and stands at the destination door, facing out, moving as before.  The
  view moves to the new place in that step: no fade, no screen, no wait;
- what passes through with the hero passes through: what the hero carries, a pulled cart, an
  enemy close behind (the same rule on both sides, readable from the screen).

**A house in a town is not a place of its own** (owner, 2026-11-08).  Its inside is part of
the town's map, on the town's layers (its basement below the street, its floors and its roof
above), so climbing up outside lands the hero at an exact spot on the house.  The houses get
bigger for it; the owner likes the sense of scale.  A place of its own is for what lies apart: a
dungeon, a mine, a stretch of land outside.  The demo village's houses stay doors to rooms of
their own: they are test rooms, not houses.  How a house is built: PUZZLES.md § Houses.

**A place you leave keeps its state.**  The rock you pushed, the chest you opened, the rat you
lit away from the door: all are where they were when you come back, unless a creature's day
(below) moved it.

**One clock that always runs.**  The world has a time of day: the clock runs in real time,
whether the hero walks or waits, and a day is a fixed number of ticks.  Away from the hero
nothing is moved tick by tick (crawler's method, `crawler/src/sim.loft`): where something is, or
what state it is in, follows from the clock when it is asked for.  A timed state stores the
moment it ends (`until = now + duration`) and is what it is until the clock passes it.

- **The hero's place runs live**, in real time and deterministically, as now: creatures there
  see, hear and act every tick.
- **Elsewhere, whereabouts are a question asked at a moment**: entering a place asks, for every
  creature with a point there, whether it is here now and where.  One halfway through a door
  when the hero arrives is halfway through that door.
- **While the hero is in a place**, a creature whose points bring it there walks in at its
  moment: entering the place works out when each such creature reaches one of its doors, and the
  game waits for that tick, as a timed state waits for its `until`.  Nothing is computed for it
  before then.
- **The answer is deterministic**: worked out from the creature's points, its seed and the
  clock, never from how often anyone looked.  The same moment always answers the same place,
  which keeps a save small and a replay exact.

**Every creature has points it likes to be at, at certain times.**  A point is a place, a cell,
the hours it is liked and what the creature does there ("the smithy's anvil, 7:00 to 12:00,
working the forge"; "the cave's back pool, from dusk, drinking").  A creature's points repeat over its period: a day for a smith, a week or more for
a merchant whose trade route crosses the map (crawler's merchants alternate markets day by day,
`npc_route` in `crawler/src/sim.loft`).  One model serves NPCs and monsters:

- **An NPC's points in time order are its day**: home, the smithy, the inn at noon, the market,
  home.
- **A monster's points spread over an area are its territory**: a lair by day, hunting spots at
  night, several points for the same hours so its seed picks among them and it seems to roam.
- **Between two points a creature walks** the way along the door graph and through each place,
  leaving early enough to arrive on time, so NPCs cross the land outside and can be met there.
- **Where a creature is at a moment** is the point it is at, or the way between two points.
- **An activity is what a creature does at a point**: sleep, eat, work (the forge, a stall, a
  field), serve, sit, loiter, patrol a few cells, guard, keep watch, drink, hunt.  In the
  hero's place the activity is the live behaviour and the picture (a smith hammering, a guard
  pacing, a sleeper lying still), and it says what the creature is open to: a shopkeeper at the
  stall trades, the same one asleep does not; a sleeping guard does not see.  Away from the hero
  an activity costs nothing; it is only data until someone is there to see it.  An activity
  with more than one form (a loiterer's few spots, a patrol's two turns) takes the one drawn
  from the creature's seed and the day, as points do.
- **In the hero's place a creature acts** on what it sees and hears (DESIGN.md's light, sound
  and sight), and can be interrupted (below).

**The developer says where and how often; the game works out the rules.**  A game developer does
not write pools, routes or timetables.  They place **spots** in the editor, each a sentence of
intent, and the game derives the detailed rules behind it:

| The developer places | The game derives |
|---|---|
| a spot: "2 rats here, often" | a point of interest, visited by a band of two on many of the pool's roams |
| "bats round this pool, at dusk" | a point with hours from "at dusk", and the bat's own habits (out from dusk, back by dawn) |
| "a cyclops sleeps here by day" | its lair, with the activity sleep for the daylight hours |
| several rat spots in one dungeon | one pool: the spots of a kind that its kind can walk between, within its reach |
| "a smith lives here, works there" | a timetable from the smith's habits: home at night, the forge by day, the inn at noon |

A spot is a place and a cell with a kind, how many (2 rats), how often (always, often,
sometimes, rarely) and, when it matters, when (by day, at night, at dusk).  Everything else
comes from the game's rules for the kind and the spot's surroundings: the activity (water: drink;
a larder: feed; a lair: sleep), how long a band stays, how far a kind roams, how it reacts and
how it comes back.  The editor shows what the game made of the spots: the pools, the bands of a
chosen roam, where every creature is at a chosen hour; so the developer sees the result of an
intent, and adjusts a detail where the derived one is not what they want (§ The developer's
view).

**What the game derives for monsters: pools, bands and points of interest.**  The spots of a kind
that its members can walk between form a **pool**: its **points of interest** are the spots
(across the dungeon's rooms, layers and places), its **members** the monsters the spots ask for,
and the counts at the spots give how many band together on a roam.  For each roam the pool
splits into bands, and each band picks its next point of interest, the "often" ones more often;
the split and the picks are drawn from the pool's seed and the roam's number, so the rats met in
the cellar are not always the same band, and a replay is the same.  A band travels together, one
leading the way and the others close behind it; its whereabouts at a moment are worked out for
the band, not for each member, and the coverage table holds the pool once, in the regions of its
points of interest.

A band is interrupted together: a rat struck alerts its band, which chases or flees as one.  A
member split from its band (fled to another layer, chased off) takes the band's next point of
interest as its way back and joins it there.  A member killed leaves the pool for good, a
change the player made; the bands of a smaller pool are smaller, and an empty pool roams no
more.

**The monsters as they are**, read as this model (`kinds.json`, `rules.loft` `enemy_act`):

| Monster | Today | In the model |
|---|---|---|
| rat, bat, spider | one nest, wandering within `roam` cells of it, chosen by its id and steps | a pool with points of interest; the nest becomes its lair |
| guard | stands at its post, steps aside for the hero | one point, the activity "keep watch" |
| cyclops | one nest; seeing the hero it shouts and those within `calls` cells search the spot until `search_until` | a band; asleep in its lair by day, the stealthy way past |
| hunter | one nest; hunts other monsters, follows a hurt one home | a pool whose points of interest are other pools' haunts |

What it keeps: timed states are moments already (`scared_until`, `search_until`, `heal_due`),
and every reaction is a way back — a hurt monster's way home to heal and a scared one's flight
are detours, the walk back to its nest a rejoin.  What it gains: hours, more points than one,
activities as data, bands, and a way back across layers: a spider that fled to another layer
today stays there, because walking home and wandering both ask for its nest's layer.

**Talking is moving (owner, 2026-11-04; built: `talk.loft`, `tests/talk.loft`).**  No dialogue
menus: what the hero says is where it walks.  Getting close to someone is to have a casual talk;
otherwise one walks on.

- **Coming beside** one who talks starts their talk.  **Walking on by** within three steps gets a
  short bark ("HI THERE.", "WHAT BRINGS YOU HERE?"), one each time by, taken in turn.
- A talk is a script of lines, each said by someone taking part (the figure, the hero, others
  `with` it) and shown for a time derived from its length.  While a line is said the player has
  three choices, always the same three:
  - **stay**: let it play out, the next line follows;
  - **bump** one of them: get something out of them, the line's `bump` for whom was bumped (in an
    argument, taking a side); bumping while holding a thing it has a gift for brings that thing;
  - **walk away**, more than three steps: get out, the line's `away` (it calls after the hero), or
    the talk ends.
- Neither is a big deal by accident: a step or two off is still listening, a bump with nothing to
  get changes nothing, so a bump's line leads back into the talk or somewhere mild.
- A line can `take` what the hero holds (the figure then holds it, in sight), `give` a thing, and
  `then` set where the next talk starts: what was chosen is remembered, in the figure's own state,
  so a place left keeps it.
- In a place file a talk reads as a script: `{ "ann": "NOT YOURS!", "next": "a" }`, a bare text
  for the figure's own line, `bump` and `gifts` as small maps.  The demo village (levels/demo.json)
  has an elder who wants a stick, and Tom and Ann arguing over the well.

**A creature's routine is its plan, and the player can interrupt it in many ways**: talk to it,
trade, give or show it something, ask it to follow, block its way, wake it, scare it, attack it,
steal from it, draw it with light or noise.  Its activity says which apply and how it takes
them: a sleeper is woken before it hears anything, a smith at the forge answers briefly, a guard
on watch notices a theft another would miss.

**Every interruption carries a way back** to the routine, one of a few:

| Way back | The creature | For example |
|---|---|---|
| pause | goes on where it stopped, late by the time it lost | talked to on its way to the inn, it arrives late |
| rejoin | goes straight to where its routine has it now | a guard pulled off its round returns to its post for this hour |
| skip | drops the point it missed, and goes on to the next | a farmer kept from the field all morning goes straight to market |
| detour | goes elsewhere for a time or until a condition holds, then rejoins | follows the hero to the gate; flees home until the danger is gone; chases a thief |

Which way back an interruption takes is data, per activity and per kind of interruption, so the
author shapes it and the editor shows it.  An interruption that should outlast the scene (a
creature hired, a shopkeeper too frightened to open for a week) is a change of its habits: a
pending consequence (§ Who can be near the hero), not an interruption.

**An interrupted creature away from the hero** is in the list of creatures off their period:
where it was, since when, and its way back.  Where it is at a moment follows from those and the
clock, as its routine's whereabouts do; it leaves the list when it is back on its routine.  A
creature on a detour with the hero (following, chasing) crosses doors with the hero, by the rule
of what passes through a door.

**Outside what the hero does and what the author scripted, every creature keeps its routine.**
Its points and their hours are the whole of its life until one of those changes them (§ Who can
be near the hero): no creature changes its habits by itself.

**Why scripted is enough.**  Whether a scene was scripted or emerged from a simulation is mostly
irrelevant to the player: they see a scene, and what they appreciate is that it is not always
exactly the same.  So the world spends nothing on simulating what nobody watches, and its effort
goes into variety instead: a creature's points and scripts offer alternatives (several points
for the same hours, a script with more than one outcome), and the one taken is drawn from the
creature's seed and the day.  The market looks different from one day to the next and from one
game to the next, and the same game replayed is still the same.

**No goals, a world to explore.**  A level's goal (a layer, daylight, a cell, every chest) goes.
What the player is after is in the world: a locked door and its key elsewhere, a chest behind a
river, a way down that needs a torch.  Nothing is tutorialised; the world is learnt by watching it
(DESIGN.md's principles hold).

**Saving.**  A save is the clock, the hero (place, cell, what is carried), each visited place's
changes against its file, and each creature the hero interrupted, with where it stood and its way
back: everyone
else's whereabouts follow from the clock; and the consequences still pending (§ Who can be near
the hero).  On the Tanmatsu in its FAT area, in the browser in
the page's storage, on a desktop beside the game.  The one rollback of DESIGN.md stays.

## Who can be near the hero

A creature's period can span weeks and the whole map, so "who has a point here" would be nearly
everyone in a busy world.  The game never looks at a creature that is far away: a table, worked
out from the data, says who can be where at which hour.

**Regions.**  The world is divided into regions: a building or a dungeon is one region; the land
outside is cut into blocks of `tile_world` chunks.  The **region graph** holds which regions
connect (the doors between a building and the land, a bridge, a pass between two blocks) and
how long crossing each region takes.

**The coverage table.**  For each region and each hour of a period: the creatures that can be in
that region in that hour, at one of their points or on their way between two.  A creature's
points and the routes between them are data, so its whole period is known: a merchant crossing
ten regions in its week is in the table of those ten, each only for the hours it passes.  Its
hours carry an hour of slack each side, so a path a little longer inside a region changes
nothing.

**Asking it.**

- **Entering a region** reads that region's entries for the current hour (the clock modulo each
  creature's period) and works out where each of those creatures is now, from its points and the
  clock.  Nobody else is looked at.
- **Arrivals during a visit** come from the next hour or two of entries: each one's moment of
  reaching this region's edge goes into a small queue by time, and each tick compares the clock
  with its head.  Leaving the region drops the queue.
- **The live set** is the hero's region and the regions next to it.  Crossing into a region of
  the land outside adds its entries and drops the ones left behind, so on a large outdoor area
  only the creatures in the block around the hero run live.
- **Creatures off their period** (interrupted by the hero) are a short list beside the table:
  where each was last, since when, and its way back.  Entering asks it as it asks the table; a creature leaves it
  when it is back on its points.  No creature leaves its period by itself.

**Recalculating it when the hero changes the world.**  The table depends on two things only: the
creatures' points and the region graph.  Only the hero changes either while the game is played:
a habit changes because of something the player did, never by itself.  The change starts where
the hero is and can reach across the world (a bridge blown up re-routes every merchant whose
route crossed it, wherever they are); it reaches exactly as far as the routes the change touches.
A change inside a region (a rock pushed, a chest opened, a wall inside a room) leaves the table as
it is; the rest is recalculated where it reaches:

| What the hero did | What is recalculated |
|---|---|
| blocked a link between regions: locked a door, blew up a bridge | the routes that use the link, found from a reverse index (link → routes over it), wherever they are |
| made a new link: unlocked a door, broke through a wall, built a bridge | the routes that become shorter through it: with the regions' travel times cached (a world has hundreds of regions), each route is one comparison |
| changed what a creature's points depend on: drove a shopkeeper out, burnt a home | that creature |
| removed a creature: killed it | that creature's entries |

The editor changes points, doors and regions too, outside play: the same recalculation, or the
whole table.

**Habits change over time, never at once.**  The recalculation is background work, a few routes
a frame, with no deadline: a burnt bridge reshapes the merchants' routes over the time it
takes, as habits change in a real town.  Until a creature's new route is in, it keeps its old
one.  While it runs, nothing changes in the hero's region because of it:

- the creatures in the hero's region stay, and go on with their old routes until they leave by
  them;
- a creature the new table brings into the hero's region walks in: its arrival is a moment like
  any other, at a door or the region's edge, after the recalculation is done.  Nobody appears;
- a creature the new table takes away leaves at its next departure.  Nobody vanishes.

**Consequences that take their time.**  What the hero chooses to do can change the world later,
elsewhere and out of sight.  A letter found forgotten beside the road: the player decides what
becomes of it (send it on, deliver it, read it, keep it, burn it), and when it is sent on, what
it sets going happens when it arrives, not when it was sent.

- **A choice is the player's or the author's.**  The world never acts on the hero's behalf.  A
  passer-by may come upon the letter, but what it does with it is scripted in the game's data
  ("the miller takes a letter he finds to the castle, two days later"), never improvised: which
  creature, which things it reacts to, what it does and after how long.
- **A passer-by comes upon a thing** when its route crosses the thing's cell: the coverage table
  gives the first such creature after the thing is there, and so the moment.
- **A pending consequence** is a moment, a place, what it changes (a creature's points, a link,
  a thing), and the condition it still needs at that moment ("the letter is still beside the
  road").  The player's choice or a script makes it; its moment is how long the effect takes to
  get there (a letter's journey to its addressee, a passer-by's scripted delay); and it can make
  the next one when it applies.  A condition no longer met at its moment does nothing: the hero
  who takes the letter first has pre-empted the miller.  Only the hero's choices and the
  author's scripts start them, so there are few.
- **They wait in order of their moment**, and each tick compares the clock with the first, as
  arrivals do.  When a moment comes the consequence applies; when it changes the table, the
  recalculation starts then, under the rule above (nothing changes in the hero's region because
  of it).
- **Hidden, never unfair.**  Nothing announces a consequence; its trace is in the world, to be
  found: a shop shut a week after its keeper's letter arrived, a guard posted where the letter
  asked for one.
- **They are saved**, being what the hero set going: the moment, the place and the change each.

**The table is never saved.**  Loading rebuilds it from the data and each place's saved changes,
deterministically, so a save stays small and cannot disagree with the world.

## One game, square or hex

The owner wants both (2026-11-09): the simplicity of the tile world for evaluating and iterating
on gameplay, and the full hex world for more realistic and engaging worlds.  This section is a
proposal for how the two meet; what is the owner's is marked so, the rest is ◇.

**The model, in three parts.**

- **Figures roam free** (owner, 2026-11-09): the hero, the monsters and the people move freely,
  keeping distances like the ones they keep now; their placement on tiles is not the problem.  F3
  is the first step (DESIGN.md § F3 roams free): the rules see the body on the cell it stands in.
- **Things and terrain stay on cells** (owner, 2026-11-09): a stone, an item, a door, a wall, water
  keeps its cell, and the restrictions on movement — what blocks, what is crossed — stay as they
  are.
- **An act aims at a cell**: Q acts on the cell before the hero, never the one it stands on
  (DESIGN.md @GM-Combat), a big step toward this common model (owner, 2026-11-09).  A push, a pull,
  a door and a blow aim the same way; a thing that moves (pushed, sliding, falling, fire
  spreading) goes to the next cell along a way.

**Only one question depends on the grid** ◇: which cell is next to this one in this direction.
Squares have four answers, hex cells six.  The rules ask the world that question instead of
working it out from `(dx, dy)`, so the same rules play on either grid.  Free roaming takes the
hero's steps and facing off the grid; what still asks are the things that move from cell to cell.

**What each grid is for** ◇

- **The square tile world** is the test bench: where gameplay is evaluated and iterated, with the
  editor and the checks, and the 2D view as far as it goes (it ends with the shapes of 2026-11-14,
  DESIGN.md § What the player sees).
- **The hex world** holds the realistic places — walls at angles, a winding stream, houses at
  their own angles, roofs, roads — built with the `hex_*` packages of loft-libs-world.  A hex step
  is 1.5 m (hexbody's scale), a person's step, so a hex cell plays at a tile's scale.
- **Square into hex, never back** (owner, 2026-11-09; RENDER_3D.md § Later): a place moves one way.
  What the two share is the rules, not the places.

**What does not carry over** ◇.  A push goes one of four ways on squares and one of six on hex, so
a puzzle that depends on four-way alignment (stones in a row, a corridor blocked across) can play
differently on hex: puzzles inside right-angled rooms keep their shape, those on open hex ground
are tried there.  The notions only squares have show once the rules ask the world — the
diagonals, the 45° slants, the rails' joins, a fence's line, the autotiles — and each is a
question for the owner in hex, not a translation.

**The drawing kit draws on either grid** (owner, 2026-11-09).  The simple routines that make a
place stay: a room, a house, a road or a river, a grove, a tree (`intent.loft`'s shapes, which
the editor's tools and `tools/orbury_gen.py` write).  They are filled into the hex grid the way
moros's hex editor fills its own, so a place is made the same simple way on both.  On hex,
houses are placed at 30° angles and their walls are thinner than a cell.  What goes from square
to hex is the shapes, not the cells: each grid fills them in its own way (RENDER_3D.md § Later's
import cell by cell is replaced).

What each shape becomes on hex ◇ (the `hex_*` packages of loft-libs-world and moros's
`hex_editor`, surveyed 2026-11-09):

| Shape | On squares (today) | On hex |
|---|---|---|
| house | a ring of wall cells, the roof on the layer above, inner walls as edges | a `hex_form` `Plan` drawn by `hex_draw`: its floor cells, its outline as wall edges, doors and windows as the edges' material, the roof as two planes (moros's `RoofPlan`); turned with the Plan |
| trail (a road, a stream) | legs along x, then along y, `width` cells | a centreline with its width (`hex_way`): a road or a stream that bends |
| blob (a pond, a grove, a cave) | ragged round cells | a ragged cell set (`hex_field`) |
| ground, dome, slope | rectangles, ceiling heights, three rising cells | regions and heights (`hex_field`'s heights, `hex_terrain`) |
| item, tree | a thing on a cell | a thing on a hex cell |
| edge (a fence, a grate) | between two squares | between two hex cells |
| yard (a garden, a terrace: owner, 2026-11-14) | an edge of its kind (low wall, low hedge, hedge, fence) along its sides, but the side against its house and its gates; its cells paved when it says so | what stands along its sides as straight stretches on the ground, turned with the house it stands against (formal/hex-houses.md § Yards) |
| a house's `wings`, its `top` ("flat", "attic"), an out-house (`out`) | a wing: walls on its three free sides, a doorway through the house's wall on each floor, its own roof — a "terrace" paved behind a low wall with a door onto it from the house; an attic: a floor within the eaves on the roof's layer, stairs up, windows in the gables and a dormer at every third cell under the slopes, the roof a layer up; a flat roof paved; an out-house as small as 3 × 3 | the house in parts, each a rectangle with its eaves and its roof: pitched and hipped, an attic's gabled roof with upright dormers, flat, a terrace; the house's wall rising over a lower wing (formal/hex-houses.md HY-Parts) |
| a house's `halls` (owner, 2026-11-14: corridors planned before the rooms) | each a rectangle of its inside on one floor or on all; on a floor that has any, the rooms are laid out from them: along each side of a corridor the floor is cut into rooms of three or four cells that run back to the wall, a wall round each and one door, onto the corridor (never onto stairs); what is left past a corridor's end is a room with its door onto the end.  A floor with none is parted by the seed as before.  **Corridors that do not touch are not joined** (owner, 2026-11-14: that limit is fine): the planner lays rooms out from each corridor and adds no way between two separate ones, so rooms lying between them connect only where the layout makes the corridors meet — a house wanting one way through plans its corridors touching or crossing | its inner walls, cell by cell, turned with the house, a doorway its two posts and the wall over the way through (formal/hex-houses.md HY-Inner) |
| a house's court, its `turn`, what is laid `over` it | the court's cells paved and open to the sky, pillars on its rim, a low wall along it upstairs; the turn unseen; the roof garden's bushes and hatch after the house | the court's outline and pillars, the roof a ring round it; the house at its angle; a flat roof behind a low wall |

- **Houses at every 30°, turned or mirrored, never refused** (owner, 2026-11-09).  A turn of
  30° is not a symmetry of the hex grid, so half the angles can never be exact, but within a few
  percent a house can always be turned: those angles get the closest approximation instead of a
  refusal.  Today moros's `place_house` (`pose_footprint`) refuses six of the twelve ("no
  mitred corners; turn one step"): their walls run along the hexes' corners (`hex_shape`'s odd
  turns), for which `hex_draw` has no flat wall, because its walls come from a `Plan`, which
  turns only in 60° steps.  That is a bug to fix in those libraries.
  - **Measured (2026-11-09, `tools/hex_orbits.py`):** the floor cells (`box_fill`) and the
    thin walls on their edges already work at every angle.  On the refused angles the edges
    stray from a straight wall no more than on the accepted ones: at most 0.40–0.66 m on either
    family (a 7.5 × 6, a 4.5 × 4.5 and a 12 × 7.5 m house), about the drawn wall's thickness,
    and which family is the closer depends on the house's size, not its angle.
  - **The fix** ◇: every angle draws its walls as the rectangle's own sides, at the exact angle
    and size the author gave, mitred at the rectangle's corners, and its roof from the same
    rectangle; the cells and the edges are the closest the grid has, as on the accepted angles
    today.  The placement answers how far it is off (moros's `fp_residual` already carries a
    residual) instead of refusing.  The fix belongs to `hex_shape` and `hex_draw` (walls from a
    `Box`, not only a `Plan`) and to moros's `hex_editor` (no refusal).
- **Turning is an editor step, and the world is drawn only from the cells** (owner, 2026-11-09).
  The editor makes a house's cells from its plan (`hexhouse.loft`) and turns them
  (`hexturn.loft`: any structure, given as its cells, by any multiple of 30°; 60° steps exact,
  an odd 30° step each drawn wall moved to the nearest line a wall can stand on in the turned
  grid; not exact, owner); the world draws the walls from the cells alone (`hexwalls.loft`), with
  no plan and no angle: the axes the walls run along are read off the cells.  It works because a
  wall always lies between two hex centres, one inside the house and one outside.  The page
  `hexhouses.html` shows an L at all twelve angles and a gallery of other shapes.
- **The town ported** (owner, 2026-11-09: start porting the town): `hextown.loft` brings Orbury's
  intent onto the hex grid — its terraces as smooth hills (`hexterrain.loft`), its houses at their
  floors' heights with their doors, its trees, fences, well and bridges — and `hextown.html` shows
  it in 3D beside a hill carrying a house at every angle and four heights.  The square town has no
  angles to bring: its terraces fall due south at every house, so its houses come over at 0°
  unless the place gives a house its angle (the intent's `turn`, 2026-11-14: five of Orbury's
  stand at one); angles are the hex town's to give (RENDER_3D.md § Later).  Rules in formal/hex-houses.md.
- **The game's 3D view draws the hex version** (owner, 2026-11-09: make it the 3D version of the
  game).  Where a place has one (`levels/<place>.hex.json`, written by `tools/hextown.loft level`),
  F1 shows its smooth ground and its houses, with the theme's pictures, while the hero is out under
  the sky or in one of its houses; play stays the square game's, the hero and every figure standing
  on the hex ground (`hexscene.loft`, `render3d.loft`).  Its meshes follow the hex mesh's model:
  the cells in chunks of 8 × 8 (`gridmesh`), each chunk's mesh kept, chunks in render groups drawn
  as one GPU mesh each, only dirty chunks built again and only their groups uploaded, a group past
  the fog or behind the eye not drawn.  Cellars and the mine keep the square view.
- **A road's sides are walls with no height** (owner, 2026-11-09): a road is cells inside and out
  like a house, its sides drawn straight by the same routines; built (formal/hex-houses.md
  HR-Sides, `hexscene.loft`).
- **What matters is that every hex centre is clearly inside or outside** (owner, 2026-11-09).
  Measured over the page's 36 houses (boxes, L's, a T and a U at their angles): no centre is on
  the wrong side of the drawn walls, and none comes nearer a wall's middle than half its
  thickness, so no centre lies under a wall.
- **A structure must be big enough to be drawn this way** (owner, 2026-11-09).  Single hexes
  marked inside or outside by hand cannot be drawn correctly, and are not a case to draw: a
  thing too small for its cells to carry its walls (an outhouse, a well) is a special object
  with its own rotation attached instead.  The smallest box on the page, 3 × 3 m, shows the
  limit: its seven cells are the same at every angle and show no direction.
- **Thin walls.**  A wall is a mark on the edge between two cells (`hex_field`'s `EdgeSet`), so
  it takes no cell: the standard house, 5 × 4 hex steps (7.5 × 6 m), keeps all 27 of its floor
  cells.  It is drawn half a hex step thick (0.75 m); movement stops on the edge
  (`hex_edge::sweep_path`), and sight reads its opacity and height.  The square game already
  keeps thin walls as edges inside houses (tile_world's edges); on hex the outer walls are edges
  too.  So to the rules a hex wall is the edge they know: is the way between these two cells
  open (step 2)?

**Tile grids at small scale, hex ground between** (owner, 2026-11-16).  The rulings of one talk,
on what the two grids are to each other.  They replace "the square tile world is the test bench,
the hex world holds the realistic places" above, and "square into hex, never back" for whole
places: a place is no longer drawn on one grid and ported to the other.  What is the owner's is
given as said; what follows from it and is proposed is ◇.

- **Why these puzzles at all** (owner): "this is exactly what draws me to these (old game) type
  of puzzles.  You can rationalize about it in your head instead of finding out with trial and
  error what works and be frustrated by it if it fails to replicate a situation that earlier
  worked."  Every ruling below serves that: the same act in the same situation gives the same
  result, and a player can work it out before trying.
- **The tile grid stays, on smaller scales** (owner): "that doesn't mean I do not want a tile
  grid, it is just that this grid lives on smaller scales (rooms, corridors, roads)."  A 90° grid
  fits inside a house or a corridor because of how they are drawn, and it "will only work in
  rooms & roads because several rooms/houses can be at odd angles" to each other: no one square
  grid covers a town.  So a room, a corridor and a road each have a tile grid of their own, at
  their own angle, and the hex ground lies between them.
  - ◇ A corridor and a road are one shape: a strip of tiles along a line, so many wide.  A room
    is a rectangle of tiles.  The drawing kit's shapes are these grids.
  - ◇ Inside a grid nothing changes: tile_world, the dealt templates and the puzzle places play
    as they do, at whatever angle the grid lies.  A room turns exactly, so a house's inside no
    longer needs turning cell by cell on hex (`hex_place::stencil_turn` stays for what is drawn
    on hex cells: outdoor structures, towers).
- **The hex data is the store; the open ground is made by routines** (owner): no tile drawing of
  a whole place is needed "if we can also use the hex binary data as a store.  We can just make
  routines to place the trees, forests, even with an algorithm to create a pattern."
  - ◇ The stored data is the place; a routine is a tool that writes into it, and a seed is kept
    as a note of where the data came from, so a place can be changed by hand after a routine ran.
- **The rules can be translated to the hex grid** (owner) — for what happens on the open ground.
- **Figures move fully free, so only moving a thing is a question** (owner): "if we already
  allow full free movement it naturally allows every situation, it is just the pushing that is
  problematic."  A thing is pushed, pulled, slid, rolled, blasted or shoved by a cart; for each
  the rules answer which way it goes, where it lands and how long it takes.
- **A thing always ends on a whole tile or hex, and is steered there from the start** (owner):
  "we secretly move the things to whole tiles/hexes and we do so by steering it that way even
  before it starts moving/flying/rolling."  When its way changes while it moves, "we just need
  to pivot early when needed."
  - ◇ The rules choose the landing cell at the moment of the push and the thing is on that cell
    from then on; the view draws a smooth way to the cell's centre.  A thing rolling over
    several cells is settled cell by cell, and the drawn way turns to the new end as soon as the
    rules know it.
- **Twelve directions on the open ground** (owner): "6 directions here is not enough, we need
  the 12 to still map the in-room 4 directions.  So pushing between two hexes is allowed as long
  as you push through to the next full hex."
  - A room's four directions are always among the twelve, and two of the four always run between
    hexes whatever the room's angle (its directions are three steps of 30° apart), so a push
    between hexes is the common case at a door.  The twelve are `hex_form`'s headings: an even
    one to a neighbour, an odd one through a corner to the next hex on that line.
- **A push that is not full goes aside** (owner): "if a push is somehow not full we can still
  use the tile rule of small objects that are pushed aside" (DESIGN.md's items pushed aside,
  `rules.loft` `aside`: the side the place prefers, then the other).
  - ◇ The two hexes a push runs between are that side and the other.  So: the full hex ahead;
    else the flank the place prefers; else the other; else the push is blocked.  A fence on the
    edge between the flanks stops the push ahead (the thing ends on one side of it); a corner
    walled on both sides blocks it.
- **A push between hexes takes longer, by the distance** (owner): "the hero needs more time to
  push between hexes (not inside a room) and that should be calculated correctly."
  - ◇ Time follows the real distance at one speed: 1.5 m in 140 ms, tile to tile or hex to
    neighbour; 2.6 m (1.5 × √3) in 242 ms between hexes; a way of several steps arrives at the
    time of its whole length, not of its steps' rounded times added up.  The hero walks the same
    way behind the thing.  A speed given in cells (a blast's push, a cart's) is a distance.
- **Leaving a grid has to be worked out** (owner): "we have to recalculate what happens when we
  go outside a room (push a stone through a door, a cart along a rails with curves)."
  - ◇ Worked out once, when a grid is placed, and kept: each opening pairs its cells with the
    cells beyond, both ways, each with the direction the thing has on the other side.  The
    rules' one question then answers the next cell and the heading there.
  - ◇ A rail is a grid of its own, a cart's cell its distance along the track: a curve changes
    nothing for the rules, only where the cart is drawn (`hex_way`).  What a rail needs worked
    out is which cells of a room or of the ground each of its cells lies over, and its switches.
  - ◇ Sight and light are worked out in the world's own space, not through the openings' pairs.
- **The open ground has not the same puzzles** (owner): "the outside world will not have the
  same puzzles, unless it is on a terrace-like structure that is just a room with low walls."
  A terrace is a room: its grid, its rules, its checks; its walls are low.
  - ◇ So nothing on the open ground has to be shown solvable, and a thing that belongs to a
    puzzle does not leave its room or its terrace through an opening; loose things may.
- **What feels real, within the rules** (owner): "it is not about what is realistic but what
  feels realistic but still adheres to the game's rules."
  - ◇ The rules' side is tested; the feel is seen and tuned in play, its numbers (how early a
    thing turns, how it settles on its cell) in the settings beside the timings.
- **Stacking and what is stable are rules, not worked out from meshes** (owner): most 3D games
  "create stacks of meshes at weird angles that slide in unnatural ways.  Here we can have rules
  about stacking and about what is stable or not without a game engine trying to calculate that
  from the meshes (and fail at it constantly)."  The game has them: a stack is a list on a cell,
  and each kind says its height, what stands and rests on it, what breaks (`kinds.json`).
  - ◇ The same stacking rules on a tile and on a hex; on a slope of the open ground, whether a
    thing stays or rolls is a rule on the heights of its cell and the next.
- **The puzzles are of the old Repton's kind, in a 3D world** (owner): "that is exactly the type
  of puzzles I want to replicate in a 3d world" — its kind of rules, exact and plannable: what a
  thing rests on, when it falls, what it rolls off.  Not its places, figures or look.
  - ◇ Falling is real here: a thing with nothing under it goes to the layer below.  What rolls
    off what is a kind's property, to add beside what stands and rests on it (`kinds.json`).
- **The side a thing goes aside to may stay the place's own** (owner): "I do not mind about a
  bit of randomness involved.  And it is not even random in the sense that the same puzzle will
  behave the same every time.  But the layout of actual puzzles involving unstable structures
  should normally have enough walls/filled tiles around it so it still reads as logical."
  - ◇ So it is the layout's to make plain, not the rule's: where a puzzle's thing can fall or
    roll, the place leaves it one way to go, or two that come to the same.  A check for the
    dealt templates and the puzzle places, beside playing a deal by the rules before it is dealt.

**Tried, marbles on a hill** (owner, 2026-11-16: "a big marble/stone almost the size of a tile so
adjacent marbles have their own tile ... a landscape with a hill and several obstacles along it
... let 10 marbles drop on the hill with the unstable rules applied.  Where do the marbles end
up?  So marbles get speed and direction and bounce off each other and obstacles/ground").
`marbles.loft`, `tests/marbles.loft`; the rules are ◇, written to be tried:
- a marble has one of the twelve headings and a whole-number speed; at rest it sets off when a
  free neighbour lies clearly lower; rolling it turns a step of 30° towards a steeper way down,
  goes one hex (or between two to the next full one), its speed taking the drop and giving a unit
  to the ground; at an obstacle it goes aside as the place prefers, else comes back at half
  speed; two marbles change motion; let fall on a marble it rolls off to the lowest neighbour.
- First run, the ground taking 1 a hex and twelve obstacles: of ten let fall on and round the
  top, four stayed on its nearly flat crown and six ran nearly straight down and 13 to 16 m out
  over the flat, meeting little.
- **Ground friction and denser obstacles first** (owner, 2026-11-16).  The ground takes 2 a hex
  (`GROUND`; 3 between hexes) and rocks are scattered by a pattern over the slopes, about one
  hex in seven (`scatter`, the same for the same seed; 100-odd of them).  Now the six that roll
  end 0 to 8 m past the foot, each having met one to four things on its way — sixteen meetings
  in all — and their ways bend round the rocks (`picture` draws the trails).  Four still stay
  on the crown.  Each on a hex of its own, at rest; the same every time.
- **Time, the bounce on landing, and houses for rocks** (owner, 2026-11-16: "my original plan
  was not rocks but houses on the slope with a better chance of catching a marble").
  - Time: whichever marble is next to arrive acts next.  A hex is a marble's own from the moment
    it sets off for it, and it is there after the time its way takes at its speed — its speed
    being the height it could climb, it goes as fast as a thing fallen that far: a hex at speed
    1 in 0.87 s, at 4 in 0.44 s, at the most in 0.25 s; between hexes 1.73 times as long.
  - The bounce: a marble is in the air the time its fall takes (2 m: 0.64 s).  Landing on a
    slope it bounces off down it with a quarter of its fall as speed; on the flat it lies still.
    So the crown no longer keeps four: all ten roll.
  - Houses: eight on the slopes (`hexhouse` plans at their angles), each with its long wall
    across the slope, three an L whose corner opens uphill.  Let fall one every 0.4 s: three
    marbles are held by a house, at 4 to 5 m up the hill; seven find a way between the houses
    and rest 1 to 8 m past the foot.  All at rest after 11.3 s, 119 happenings — the same every
    time, to the millisecond.
  - ◇ What it shows: the eight houses nearly close a ring, so the seven leave by the same two
    gaps and four end in a row; where a wall stands across the slope a marble stays, where it
    slants the marble runs along it.  The landing has no bounce to be seen (it is a speed, not
    a hop), and a marble struck while on its way changes motion at once.

- **The houses spaced out, and the hill in the 3D view** (owner, 2026-11-16).  `marblehill.loft`
  builds the hill as a place the view draws: five houses 9 to 14 m from the top, each facing
  down the hill and seated on the ground (a level floor cut into the slope), two an L open
  uphill.  Three marbles are held by a house, 2 to 3 m up; seven leave between the houses in
  different directions and rest 2 to 8 m past the foot; at rest after 11.2 s.  The page
  `hextown.html` has it as a third scene, "Marbles on a hill": every marble's way is played back
  in time, over and over (`?scene=2`; `&t=MS` holds a moment still).
  - ◇ Seen there: a marble held behind a house is hidden by its roof from most sides; the
    marbles roll in straight stretches from hex to hex, with no spin and no easing yet.

- **A bigger hill; spin and easing** (owner, 2026-11-16).  The hill is 14 m high and 42 m to its
  foot, seven houses 14 to 30 m from its top; the ten are let fall 6 to 8 m out, where the slope
  begins (the crown of a hill this size is too flat to set a marble going).  Three are held by
  a house, 5 to 10 m up; seven rest 5 to 8 m past the foot; at rest after 12.8 s.  In the page a
  marble has a banded skin and rolls — turned by the way it goes over its radius — along a
  smooth line through its stops, so it eases away from rest and into rest and rounds its turns;
  it lands with a hop that dies away.  All of that is the view's: the rules' stops are as they
  were.

- **Fifty marbles** (owner, 2026-11-16: the marbles are as big as wanted; fifty of them).  Let
  fall one every 0.25 s on the hexes 6 to 10 m from the top, going round the hill in long
  strides.  Six are held by a house, two rest on the slope, forty-two on the flat past the foot;
  87 meetings; at rest after 22.9 s and 1063 happenings — the same every time.

- **Yards and fences between the houses** (owner, 2026-11-16: "add yards and fences between the
  houses to catch more marbles"; and, as the catching rose, that catching as many as possible
  is not the goal: "for the audience seeing strings of marbles appearing that run in lines is
  part of the fun" — confirmed by the owner the same day).
  - A fence stands on the edges between hexes and takes no hex (`marbles::fence`, a line in
    metres laid on the edges it parts).  A marble does not pass it: to a neighbour, a fence on
    the edge between; between two hexes, a fence along the way or a corner shut on both sides.
  - Behind every other house a yard — two low walls 5 m up the hill and outward, leading what
    comes down between them to the house; from each of the others a fence 8 m along the hill.
    Of fifty: 20 held by a house, 4 by a fence or a yard's wall, 1 on the slope, 25 out past
    the foot in their strings.  (With a yard and a longer fence at every house 38 were held and
    12 ran out.)
  - Found: a marble set off from rest towards an open way turned itself, by the steeper-way
    rule, back into the fence that had stopped it, for ever; it now turns only towards a way
    that is open.

**Later, water by the same routine** (owner, 2026-11-16).  The marble hill is "a nice addition to
the engine, but getting real realistic scenes here is not our goal either.  After our puzzles
work (and I have tested them too) we should also allow for a similar routine that uses a marble
down the hill routine to find out where water needs to flow.  So we do not calculate it all the
time, but on scene changes (like locks) we analyze where the water will go and adjust the game to
it."  Not started; it waits for the puzzles and the owner's testing of them.
- **Water has speed, and carries** (owner, 2026-11-16): "Water has speed in practice, we do want
  to incorporate that in our model.  Water slides should be in the game and potential to be
  dragged along by water is there."
- ◇ So what the routine works out on a scene change is, per wet cell: how deep, which way the
  water runs (one of the grid's ways) and how fast — its speed taken from the drop it has come
  down and given back to the ground, as a marble's is.  It is kept until the next change.
- ◇ What carries over from `marbles.loft`: the lowest free neighbour, what stops a step (a
  wall's hexes, a fence's or a lock's edges), speed from the drop, the same answer every time.
  What differs: water does not stop on a slope and does not keep to one cell — it runs on,
  spreads, and where no neighbour is lower it stands and fills until it finds a way over.
- ◇ Carrying is then a rule on the kept answer, not worked out again: a figure or a thing in a
  cell whose water runs faster than it can stand against is taken one cell along the water's
  way, in the time that speed gives; slower water is waded.  A water slide is a channel steep
  enough that its water is that fast all the way down.

- **Even a pond keeps a little speed** (owner, 2026-11-16): "It would be marvelous if even ponds
  have a tiny bit of speed left so when we model a wooden beam inside a stream it still ends up
  down river of a pond eventually."
  - ◇ So standing water is never quite still where it has a way out: every cell of a pond that
    spills gets the least speed there is, and for its way the next cell on the shortest way
    through the pond to where it spills (worked out from the spill point back, when the scene
    changes).  A beam let go upstream comes down the stream, crosses the pond slowly and goes
    on below it.  A pond with no way out is still.
  - ◇ The game has the slow end of this already: a raft pushed off drifts a cell every
    `DRIFT_MS` (`rules.loft`).  The least speed of a pond would be that drift, with a way.

◇ Open, for the owner: whether a thing on a rail's cells is only the rail's, others crossing at
marked crossings; whether loose things cross a house's outer door at all.

**Built, the first room** (2026-11-16; `gridroom.loft`, `tests/gridroom.loft`): a room — a
square place as the game has them, played by its own rules — set down at any of the twelve angles
on a field of hexes, its door paired both ways with the hex straight outside it, one tile's
length out (the room is moved by less than a hex to make it so).  At each angle a rock is pushed
two tiles inside by the room's rules, out through the door, twice on over the ground and back to
the tile it left; every step 1.5 m in 140 ms, or between hexes 2.6 m in 242 ms.  Between hexes
with the hex ahead taken, or a fence along the way, it goes aside as the place prefers, then the
other side, then not at all.  A room's four ways are four of the twelve, two of them always
between hexes.
- **The same grid serves drawn maps** (owner, 2026-11-16): "the up-side to the grid in the rooms
  and corridors is that we can use it on drawn maps too (for Moros tabletop) where we want a grid
  where people can stand presented as big tiles."  `gridroom::tile_corners` gives a tile as the
  square a map draws.  ◇ So this geometry belongs in the shared libraries once it has settled.
- Not yet: the game's play on it (the hero, the view and the clock still run on one square
  place); more than one thing on a hex; a second opening whose hex does not lie straight out
  (it goes aside; untried); a corner walled on both sides.

**Built, the terrace** (2026-11-16; `gridroom.loft`): a terrace is the same room, floor to its
rim with a low wall on the rim's edges and a gate where there is none.  Its ways out are found
from the room itself (a rim tile's side nothing stops a step on: a doorway, a gate), so a room
and a terrace are one thing to the code.  `rim` gives what stands round a room side by side — a
wall, something low, or nothing — for a map and a view to draw.  `sees` answers whether one point
sees another past a room, in the world's own space: not through its walls and hedges, on its rim
or inside it; over a low wall; through a doorway, a gate or a gap.
- **A garden maze is a single terrace, even one that spans quite a big area** (owner,
  2026-11-16).  Its hedges are edges inside it: tried at every angle with a hedge round it and one
  down its middle — a rock stops at the hedge and passes the gap, and so does sight.
- **A maze that is a real puzzle** (owner, 2026-11-16): "you want places with an overview on the
  maze and places with little sight range.  And possibly bridges that span other walkways through
  the maze to prevent left/right handing it."
  - Built: `sees` takes how high the eye is.  From the floor a hedge is not seen over; from a
    layer up (a bridge, a platform) it is.  A low wall and a low hedge are seen over from
    the floor: they stop a step, not sight (kinds.json: `low_wall`, `low_hedge`; `hedge` opaque).
  - ◇ Little sight range is the maze's own doing — high hedges and short straight stretches —
    and needs no rule of its own.
  - Built, layers and a bridge (`gridroom.loft`): a bridge is tiles a layer up over a path,
    reached by stairs, a low wall along its sides.  `walk_from` asks the room's own rules where
    a step leads, layer and all: the bridge is walked over and the walkway under it walked under,
    the same tiles on two layers.  `sees` works by height through the room: under a bridge along
    the walkway; from the ground up to one on the bridge and back; from the bridge over a hedge
    that hides the far side from the ground; not down through the bridge's own floor.
  - Built, the check: `one_hand` walks a maze with one hand on the wall, `reached` says whether
    there is a way at all.  A court with a hedge across it and a bridge whose stairs stand free
    in its middle is reached and beats both hands; the same with the stairs against the hedge is
    walked by one hand.  ◇ For a dealt maze: reached, and not by one hand.
  - In the demo village (`levels/the_maze.json`, `tests/maze.loft`): the fenced garden's gate
    leads to a maze of thirteen tiles by nine — an outer walk between high hedges with one gap
    into the garden, high hedges that close the view, low hedges seen over, and a bridge whose
    stairs stand free, over a walk and the hedge of the closed plot in the middle, where a chest
    is.  Every tile is walked to; the plot by neither hand; the garden by the right hand only
    (the left goes round the outer walk for ever).  It is played by the square game as it is.
  - ◇ Found making it: from a bridge one layer up, a hedge a layer high hides the walk behind it
    unless the hedge is nearer than half the way there — as it would.  A real lookout wants two
    layers.  And what the player sees is the camera's: the view does not yet hide what the hero
    does not see, so "little sight range" is not yet felt in play.
  - Not built: a way out of a room on a layer over its own (an upper floor's door onto a
    terrace or a hillside); a thing pushed off a bridge is the room's rules' alone.
- A thing pushed to the rim cannot be pushed back by a hero standing inside: the rules ask the
  hero to stand behind it, which is beyond the low wall.  It is pulled back, as in any room.

**Built, play on a field with rooms** (2026-11-16; `fieldplay.loft`, `tests/fieldplay.loft`): the
hero's body is a point in metres and moves any way at the walking pace; which grid it is on
follows from where it is.  As free roaming has it on one square place, the body coming onto
another cell takes the step there through the rules: onto a free hex; a thing on a hex pushed
along the hero's way; in at a room's opening from the hex outside it; in the room by the room's
own rules; out through an opening.  A wall from either side, a tree and a fence refuse it and
the body stays where it was.  One clock: the room the hero is in runs with it, a room it comes
into is brought to the field's time.  Tried at each of the twelve angles: 9 m over the ground to
the door and in (in 840 ms, nothing refused), round a rock, the rock walked out through the door
and on over the ground, back in; and from one room's door over 20 m of ground in at a terrace's
gate.
- **These algorithms are to end up in the libraries** (owner, 2026-11-16: "The algorithms need to
  end up inside libraries eventually").  Candidates, once settled: the room grid on hex ground
  (`gridroom.loft`), play across grids (`fieldplay.loft`), the marble rules (`marbles.loft`).

◇ Next: this wired to the game's own keys, clock and views (game.loft still plays one square
place at a time), and other figures crossing between grids.  The steps below were written before these rulings:
step 2 stands (the rules ask the world for the next cell, and now the heading with it); steps 3
and 4 are for the open ground only, and a place's rooms are placed, not filled into hex.

**Steps** ◇, each worth having on its own:

1. Free roaming the everyday way to move, once it plays well enough (the owner's call).
2. The rules ask the world for the next cell.  Nothing changes on squares; every test and check
   stays green.
3. A hex world answering the questions tile_world answers (paths, sight, light, the next cell).
   tile_world needs a new version anyway: its sixteen terrain kinds are taken.
4. The drawing kit filled into hex, shape by shape as in the table above; then a place's intent
   (Orbury's, as `tools/orbury_gen.py` writes it) filled into a hex place, its houses at their
   own angles.  The test: a square puzzle place's intent filled into hex and solved by the same
   rules, and the square Orbury's walk-through played in the hex one.

**No backend lost** (owner, 2026-11-09).  A hex world has no 2D pictures (Kenney's sheets are
square), so the 3D routines are ported to the Tanmatsu first (BUILD.md Phase E), and no hex area
comes into a design before the 3D view plays on the device.  Steps 2 to 4 are that phase's E7.

**Which view ships first is ruled** (owner, 2026-11-14): the 2D view ends with the shapes the
basic tiles cannot show, and the 3D view is the game's presentation (DESIGN.md § What the player
sees).  ◇ Still open against "no backend lost": the Tanmatsu draws 2D until Phase E's 3D plays
on it, so there the new shapes are plain lines.

## The two machines

| | Tanmatsu | Browser |
|---|---|---|
| frame | 208x160 at scale 3 on the 800x480 panel (624x480: scale 4 does not fit) | a canvas, scaled to the window |
| keys | the keyboard (`input`) | the page's keys |
| pointer | none: everything works from the keyboard | the mouse, for the editor |
| sound | the ES8156 through I2S (`audio`'s Tanmatsu section, not yet run) | the page's audio |
| files | packed in the app, found from what names them (`--native-esp`) | packed into the page |
| saves | the FAT area | the page's storage |

The game asks what the machine has (keys, a pointer, the screen's size), never which machine it is
(@PLN209's rule).

## The editor in the browser

How it is built and used — two passes, building a world and then placing its challenges — is
BROWSER_EDITOR.md.  Everything the game reads is editable there, in the same loft code the game runs (one model, so
an edit is played the moment it is made):

- **places**, in the developer's terms first: rooms, corridors, caves and yards drawn as
  outlines with words for what is in them ("a bit of rubble", "damp", "crates along the walls"),
  filled in by the game; then, as adjustments, terrain and materials, layers and rises, edges and
  doors and the things standing in a cell, cell by cell — the in-game editor (EDITOR.md) with the
  mouse added;
- **doors**: link a door to a door in another place, follow a link, list the doors that lead
  nowhere;
- **materials and sheets** (theme.json): which tile a material draws, its variants and autotiles;
- **kinds** (kinds.json): what each thing and creature is and does;
- **sounds**: which sound an event plays;
- **creatures' points**: placed on the map with their hours and activities, for NPCs and monsters
  alike, and a view of where every creature is, and what it is doing, at a chosen time of day;
- **spots**: where, which kind, how many, how often and when, for monsters and NPCs alike; the
  editor shows what the game derived from them (the pools, the bands of a chosen roam, an NPC's
  day), each derived detail adjustable and marked as derived or adjusted;
- **scripts**: what a creature does with what it comes upon, and after how long;
- **interruptions**: per activity, which interruptions a creature takes and its way back from each;
- **the start**: where a new game begins;
- **checks**, as the in-game editor has them (K): a door that leads nowhere, a place no door
  reaches, a chest that cannot be reached;
- **play from here**: the hero dropped at the cursor, the edit's state kept.

The editor reads and writes the game's own files (place files, theme.json, kinds.json).  A page
keeps them in its storage, and exports and imports them as one archive, so the work goes back into
this repository and onto the Tanmatsu.  Installing onto a Tanmatsu straight from the page over
WebUSB (badgelink speaks WebUSB) is a later step.

## Order of work

Each step is checked before the next starts; "checked" names what goes red.

1. **Places and doors** (the game's logic).  Level files become place files, a door edge gains a
   destination, `order.json` and goals go; a start place and cell remain.  Checked by a scripted
   walk: out of one place through its front door, back in through its back door, every place's
   state as left.  *Built for the demo village (HANDOFF.md), with what follows the hero through
   a door (BUILD.md C3c) and doors set in the editor (A2).*
2. **The clock and whereabouts.**  A time of day; timed states as moments; whereabouts asked on
   entering a place; a held-up creature going on from where it stands.  Checked by asking the
   same moment twice and across a save and reload: the same answers; and by the hero following a
   creature through a door and finding it on its way.  *Built in part: the clock (BUILD.md C1),
   the world's people asked where they are on entering (C3), a save read back the same (C6); open:
   a held-up creature going on from where it stands.*
3. **Creatures' points and who can be near the hero.**  Points with hours over a period, walked
   between along the door graph, arriving on time; activities at the points; regions, the region graph and the coverage
   table; arrivals during a visit; the list of creatures off their period; recalculation when the
   hero changes the world.  Checked by a scripted period: each NPC at its points at their hours, doing their
   activities, each monster at one of its points for the hour; a shopkeeper who trades at the
   stall and not asleep; spots turned into the pools, bands and timetables their words say
   ("2 rats here, often": bands of two at that spot on many roams), and an adjusted detail kept
   when the rest is derived again, the derived one back when the adjustment is taken away; a pool's bands of the size
   its spots give, each at one of its points of
   interest, the same bands for the same roam and different ones for the next; a spider that
   fled to another layer rejoining its band; a creature interrupted in each way back (pause, rejoin, skip, detour)
   back on its routine at the moment its way back gives, whether the hero watches or leaves; by entering a region and finding exactly the
   creatures a search over every creature finds there; and by any run of random player actions
   leaving the recalculated table equal to one built from scratch; by a recalculation during a
   visit that changes no creature in the hero's region, the new ones walking in afterwards; and
   by a letter the hero sends on, whose change happens at the moment it arrives and not before;
   by a letter left beside the road, taken by the scripted passer-by at the moment the table
   gives and acted on after its scripted delay, or left alone when the hero took it first; the
   same in every run and across a save and reload.  *Begun: a person's points by the clock
   (`routes`, BUILD.md C3b), the world's people going from place to place through the door
   straight to the next and found at their points on entering (C3), a guide, and a tribe raiding
   within one place (C4); open: routes over the door graph, regions, the region graph and the
   coverage table, pools and bands, ways back, letters, a tribe across places.*
4. **The world from what exists**, in the developer's terms.  The five levels stitched together
   through a stretch of land outside (Kenney's Tiny Town and Tiny Farm sheets), each with a
   second way out; their rooms and corridors described as outlines with their words, the game
   filling them in, today's cells kept as adjustments where the filling differs.  Checked by
   walking the whole world in a scripted run; by the editor's checks finding no door to nowhere;
   and by filling a room in again, which leaves its doorways and ways open and every hand-set
   cell as it was.
5. **Saves.**  The clock, the hero, each visited place's changes, the creatures interrupted and the
   pending consequences, written and read back.  Checked by a save, a restart and the same state.
   *Built for what exists (BUILD.md C6): the clock, the hero, the facts and each visited place,
   enemies at their homes; what is in motion is not saved, and the people are where their points
   say.*
6. **The Tanmatsu.**  Scale 3, `audio` on the device, saves in the FAT area, one place's files
   decoded at a time.  Checked on the device with the timing log: frame time, the time a door
   takes, the PSRAM a place takes.  Only the tiles the game uses go onto the device, reshuffled
   into sheets of their own (owner, 2026-11-04): measured 2026-11-04, theme.json uses 182 of the
   798 tiles in its six sheets (dungeon 39 of 132, town 51 of 132, farm 2 of 132, battle 15 of
   198, ski 36 of 132, the game's own 39 of 72).  A cell's tile number names a sheet's slot and
   an index (`1 + slot * 256 + tile`, materials.loft), so a reshuffle must carry every place file's
   numbers with it.
7. **The browser.**  `screen`, `input`, `audio` and saves in a page (@PLN209 F2, F4, F7: library
   work), the game built as a page.  Checked by a scripted run giving the same frames in the page
   as on the desktop.
8. **The editor in the page.**  The mouse in the place editor first, then doors, creatures'
   points, spots, scripts, interruptions, materials, kinds, sounds and the start; export and import.  Checked by an edit made
   in the page, exported, and played on the Tanmatsu.
9. **Fun.**  Not last in time: every step above is played, and what is not fun is changed.  The
   measure is DESIGN.md's: familiar pictures, rules that surprise a player who knows the genre,
   fair and readable, hurt rarely killed.
