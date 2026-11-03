# One world, on the Tanmatsu and in a browser

The game is one world the hero walks through: out of a dungeon or a building, across the land
outside, into the next.  No levels, no screens between places, no order to play them in.  It runs
on the Tanmatsu and as a web page, and the page carries a full editor for every part of the game's
data.

## The world

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
  and sight).  A creature the hero held up (talked to, followed, a door locked in its way, a
  monster drawn out by a torch) goes on from where it stands when the hero leaves, to its next
  point.

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
changes against its file, and each creature the hero held up, with where it stood: everyone
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
- **Creatures off their period** (held up by the hero) are a short list beside the table: where
  each was last, and since when.  Entering asks it as it asks the table; a creature leaves it
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

Everything the game reads is editable there, in the same loft code the game runs (one model, so
an edit is played the moment it is made):

- **places**: terrain and materials, layers and rises, edges and doors, the things standing in a
  cell — the in-game editor (EDITOR.md) with the mouse added;
- **doors**: link a door to a door in another place, follow a link, list the doors that lead
  nowhere;
- **materials and sheets** (theme.json): which tile a material draws, its variants and autotiles;
- **kinds** (kinds.json): what each thing and creature is and does;
- **sounds**: which sound an event plays;
- **creatures' points**: placed on the map with their hours and activities, for NPCs and monsters
  alike, and a view of where every creature is, and what it is doing, at a chosen time of day;
- **scripts**: what a creature does with what it comes upon, and after how long;
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
   state as left.
2. **The clock and whereabouts.**  A time of day; timed states as moments; whereabouts asked on
   entering a place; a held-up creature going on from where it stands.  Checked by asking the
   same moment twice and across a save and reload: the same answers; and by the hero following a
   creature through a door and finding it on its way.
3. **Creatures' points and who can be near the hero.**  Points with hours over a period, walked
   between along the door graph, arriving on time; activities at the points; regions, the region graph and the coverage
   table; arrivals during a visit; the list of creatures off their period; recalculation when the
   hero changes the world.  Checked by a scripted period: each NPC at its points at their hours, doing their
   activities, each monster at one of its points for the hour; a shopkeeper who trades at the
   stall and not asleep; by entering a region and finding exactly the
   creatures a search over every creature finds there; and by any run of random player actions
   leaving the recalculated table equal to one built from scratch; by a recalculation during a
   visit that changes no creature in the hero's region, the new ones walking in afterwards; and
   by a letter the hero sends on, whose change happens at the moment it arrives and not before;
   by a letter left beside the road, taken by the scripted passer-by at the moment the table
   gives and acted on after its scripted delay, or left alone when the hero took it first; the
   same in every run and across a save and reload.
4. **The world from what exists.**  The five levels stitched together through a stretch of land
   outside (Kenney's Tiny Town and Tiny Farm sheets), each with a second way out.  Checked by
   walking the whole world in a scripted run, and by the editor's checks finding no door to
   nowhere.
5. **Saves.**  The clock, the hero, each visited place's changes, the creatures held up and the
   pending consequences, written and read back.  Checked by a save, a restart and the same state.
6. **The Tanmatsu.**  Scale 3, `audio` on the device, saves in the FAT area, one place's files
   decoded at a time.  Checked on the device with the timing log: frame time, the time a door
   takes, the PSRAM a place takes.
7. **The browser.**  `screen`, `input`, `audio` and saves in a page (@PLN209 F2, F4, F7: library
   work), the game built as a page.  Checked by a scripted run giving the same frames in the page
   as on the desktop.
8. **The editor in the page.**  The mouse in the place editor first, then doors, creatures'
   points, scripts, materials, kinds, sounds and the start; export and import.  Checked by an edit made
   in the page, exported, and played on the Tanmatsu.
9. **Fun.**  Not last in time: every step above is played, and what is not fun is changed.  The
   measure is DESIGN.md's: familiar pictures, rules that surprise a player who knows the genre,
   fair and readable, hurt rarely killed.
