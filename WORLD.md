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
- **The answer is deterministic**: worked out from the creature's points, its seed and the
  clock, never from how often anyone looked.  The same moment always answers the same place,
  which keeps a save small and a replay exact.

**Every creature has points it likes to be at, at certain times.**  A point is a place and a
cell with the hours it is liked ("the smithy's anvil, 7:00 to 12:00"; "the cave's back pool,
from dusk").  One model serves NPCs and monsters:

- **An NPC's points in time order are its day**: home, the smithy, the inn at noon, the market,
  home.
- **A monster's points spread over an area are its territory**: a lair by day, hunting spots at
  night, several points for the same hours so its seed picks among them and it seems to roam.
- **Between two points a creature walks** the way along the door graph and through each place,
  leaving early enough to arrive on time, so NPCs cross the land outside and can be met there.
- **Where a creature is at a moment** is the point it is at, or the way between two points.
- **In the hero's place a creature acts** on what it sees and hears (DESIGN.md's light, sound
  and sight).  A creature the hero held up (talked to, followed, a door locked in its way, a
  monster drawn out by a torch) goes on from where it stands when the hero leaves, to its next
  point.

**No goals, a world to explore.**  A level's goal (a layer, daylight, a cell, every chest) goes.
What the player is after is in the world: a locked door and its key elsewhere, a chest behind a
river, a way down that needs a torch.  Nothing is tutorialised; the world is learnt by watching it
(DESIGN.md's principles hold).

**Saving.**  A save is the clock, the hero (place, cell, what is carried), each visited place's
changes against its file, and each creature the hero held up, with where it stood: everyone
else's whereabouts follow from the clock.  On the Tanmatsu in its FAT area, in the browser in the page's storage, on a
desktop beside the game.  The one rollback of DESIGN.md stays.

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
- **creatures' points**: placed on the map with their hours, for NPCs and monsters alike, and a
  view of where every creature is at a chosen time of day;
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
3. **Creatures' points.**  Points with hours, walked between along the door graph, arriving on
   time; several points for the same hours chosen by seed.  Checked by a scripted day: each NPC
   at its points at their hours, each monster at one of its points for the hour, never outside
   them unless drawn out.
4. **The world from what exists.**  The five levels stitched together through a stretch of land
   outside (Kenney's Tiny Town and Tiny Farm sheets), each with a second way out.  Checked by
   walking the whole world in a scripted run, and by the editor's checks finding no door to
   nowhere.
5. **Saves.**  The clock, the hero, each visited place's changes and the creatures held up,
   written and read back.  Checked by a
   save, a restart and the same state.
6. **The Tanmatsu.**  Scale 3, `audio` on the device, saves in the FAT area, one place's files
   decoded at a time.  Checked on the device with the timing log: frame time, the time a door
   takes, the PSRAM a place takes.
7. **The browser.**  `screen`, `input`, `audio` and saves in a page (@PLN209 F2, F4, F7: library
   work), the game built as a page.  Checked by a scripted run giving the same frames in the page
   as on the desktop.
8. **The editor in the page.**  The mouse in the place editor first, then doors, creatures'
   points, materials, kinds, sounds and the start; export and import.  Checked by an edit made in the page, exported, and
   played on the Tanmatsu.
9. **Fun.**  Not last in time: every step above is played, and what is not fun is changed.  The
   measure is DESIGN.md's: familiar pictures, rules that surprise a player who knows the genre,
   fair and readable, hurt rarely killed.
