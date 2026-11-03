# The editor in a browser

A developer opens a page, builds a world in an afternoon, plays it at once, and comes back later
to place the challenges inside it.  The page is the game's own loft code with the editor around
it, built for the browser; the world it makes runs unchanged on the Tanmatsu.  WORLD.md is the
world it edits; EDITOR.md is the in-game editor this one grows from.

## Two passes

1. **Build the world, fast.**  Land outside, the places on it, the rooms inside them, the life
   in them: all in the developer's terms (WORLD.md § The developer's view), filled in by the
   game, played the moment it is drawn.  A dungeon of five rooms with its rats is minutes.
2. **Place the challenges.**  The developer walks the world they built and drops challenges into
   it: a locked door and its key, a guarded way, a dark crossing.  A challenge is a template with
   a few choices; the game wires it into the world and checks that it can be solved.

The first pass never waits on the second: a world without a single challenge is playable, and a
challenge is placed into a world that already works.

## How it runs in a browser

| Part | What it is | State |
|---|---|---|
| the page | the game and editor as one loft program, built through the C backend as a page (`loft --native-c --html`): no Rust | the C page build exists (@PLN203 C6) |
| the canvas | `screen`'s browser section: the frame drawn into a canvas, scaled to the window (@PLN209 F2) | to build |
| keys, pointer, text | one `input` over the page's events (@PLN209 F4), with text, IME and the clipboard (BROWSER_INTEROP.md § Step 4) | to build |
| the files | the page filesystem (WASM.md § The page filesystem): the game's assets and data as the base tree, every write a delta kept in the page's storage | built |
| the widgets | panels, lists, buttons, text fields and an undo stack: a platform-blind loft library drawn with `tiles` and the theme's font, so the same widgets serve the in-game editor on the desktop and the Tanmatsu (BROWSER_INTEROP.md § The four tiers) | to build |
| export, import | the world's files out of the page as one archive and back in: a small browser-only library over the engine's byte channel | to build |
| onto a Tanmatsu | the archive installed over WebUSB (badgelink speaks WebUSB) | later |

The editor's frame is larger than the game's: the page asks the screen for its size (@PLN209 F5)
and lays out a map view with panels beside it; "play" shows the game's 208x160 frame scaled up,
as the Tanmatsu shows it.  Everything is drawn by loft into the frame; the engine only moves
pixels, events and bytes (BROWSER_INTEROP.md § The one invariant).

## Three layers of data

| Layer | Holds | Stored |
|---|---|---|
| **intent** | what the developer said: places and their doors, outlines with words, spots, challenges | `world.json` and a file per place |
| **adjustments** | each detail the developer set by hand: a cell, a band of exactly three, a tile | beside the intent, in the same files, listed apart |
| **derived** | what the game filled in: cells, autotiles, pools and bands, timetables, a challenge's wiring | never: worked out from the other two, deterministically, whenever it is needed |

The editor shows all three, the derived ones marked as such; an adjustment is made by editing a
derived detail, and taken away to get the derived one back.  Because the derived layer is never
stored, a change to the game's rules reaches every world the next time it is opened.

## What the editor draws

**The world as it looks.**  The map view is drawn with the game's own tiles, sheets and lights,
so a place in the editor is the place in the game, and editing it is editing what the player
will see.

**What the world means, over it.**  The editor draws more than the game's pictures: overlays in
its own lines, arrows, outlines and colours, shown or hidden one by one:

- **routes**: an NPC's day as a line through the doors it takes, a band's roams between its
  points of interest, the hour along each;
- **layers**: the layer above and below, seen through, and the stairs and openings between them;
- **connections**: doors and where they lead, the region graph, the challenge graph (what opens
  what), a consequence and the place it reaches;
- **reach**: what the hero can get to from here, the cells a key may go in, a monster's
  territory.

The drawing tools work on the overlays too: a route drawn by hand becomes an NPC's way between
two points (an adjustment of the derived route), a line between two doors links them, an
outline drawn over the land marks a region.  The overlays are the editor's, never the game's:
nothing of them reaches the player.

## Pass 1 — building a world

**The land outside.**  A map of the outdoors painted with the materials (grass, path, lake, the
castle wall: autotiled from theme.json), at a scale where a cell is a chunk; then zoomed into for
detail.  Places are dropped onto it as **stamps**: a house, a tower, a cave mouth, a cellar hatch.
A stamp makes the place behind it and the door pair between them; dragging another door onto a
place's wall makes a back door, linked to the land where it is dropped.

**A place.**  Rooms drawn as outlines (drag a rectangle; a cave is drawn freehand), corridors as
lines between them; where two rooms meet the game makes a doorway, and a stair dropped on a cell
joins two layers.  Each room takes **words** from a palette: "a bit of rubble", "damp", "crates
along the walls", "overgrown", "lit by torches".  The palette's words are data in theme.json: a
word names the things and material variants it scatters and how densely, so a new word is a few
lines of data over the sheets the game already has.

**Life.**  Spots dropped from a palette of kinds: "2 rats here, often", "bats round this pool, at
dusk", "a cyclops sleeps here by day", "a smith lives here, works there" (WORLD.md).  The editor
shows what the game made of them: the pools and the bands of a chosen roam, an NPC's day,
everyone's whereabouts at a chosen hour on a clock the developer drags.

**Starting points.**  The five places the game has (the first climb, the cellar, the torch room,
the old mine, the cyclops hall) as **templates**: a crypt, a cellar, a mine, stamped and then
changed.  A template is a place's intent, so a stamped one is filled in for where it lands.

**Playing it.**  "Play from here" drops the hero at the cursor with the clock where the editor
has it; stepping out of play returns to the editor where it was, the edits kept.  The world is
always playable: what pass 1 makes has no goal to finish, only places to walk.

## Pass 2 — placing challenges

A challenge is a **template** placed onto the built world: the developer points at where it goes
and answers a few choices; the game wires the rest from the mechanics the game has (kinds.json:
carts and rails, rocks to push, levers, torches and tar, bombs, water, doors and keys).

| Challenge | The developer picks | The game wires and checks |
|---|---|---|
| locked door | the door | a key, placed where the developer picks among the cells the game shades: reachable without passing this door |
| guarded way | the way | a spot of guards at its narrowest; asleep at night, so the way is passable by stealth |
| dark crossing | the stretch | the dark it needs, and a light to carry reachable before it |
| water to cross | the water | a way over from what is at hand: a rock to push in, a cart on rails, a bridge to lower |
| a wall to break | the wall | a bomb or a tar barrel reachable from this side |
| a lever and a gate | the gate | the lever, somewhere the gate cannot be seen from, and the rail or door it moves |
| a chest worth reaching | the chest | what is in it, and which of the other challenges stands in its way |
| the letter | the thing and the one it is for | the delayed consequence and what it changes (WORLD.md § Consequences) |

**The challenge view** lists every challenge with what it needs and what it opens, as a graph:
which key opens which door, which door leads to which chest.  The checks run as the developer
works:

- every challenge can be solved from the start, in some order;
- no choice leaves the player stuck: a key cannot be lost behind its own door, a rock pushed into
  the only corner it fits cannot block the only way (WORLD.md's rule: never a dead end);
- a challenge left unwired (a lock with no key yet) is marked, not refused, so pass 2 can be done
  over several sittings.

"Play this challenge" drops the hero just before it, with what the player would carry by then.

## What the Tanmatsu can carry

The editor shows the world against the device's budget, from the numbers `loft --native-esp`
reports (TANMATSU.md): the packed size against the app's room, the PSRAM one place needs decoded,
and the creatures the busiest region can hold live.  A world that fits is green; a place that
does not says what to take out.

## Order of work

Each step goes red on its own; the earlier ones are what the later ones are built on.

1. **The game as a page.**  `screen` and `input` in the browser (@PLN209 F2, F4), the game built
   as a page and played there.  Red when: a scripted run's frames in the page differ from the
   desktop's.
2. **The widget library.**  Panels, lists, buttons, text fields, undo, drawn in the frame, on the
   desktop and in the page.  Red when: a scripted edit through the widgets (CDP-typed text, a
   click) does not leave the model it says.
3. **Intent, adjustments, derived** in the game's code, with rooms, corridors and words filled in.
   Red when: filling a room in again changes a hand-set cell or closes a doorway; the same intent
   fills in the same cells on every machine.
4. **The land, the places, the stamps.**  Painting outside, stamps that make a place and its door
   pair, back doors, templates from the five places.  Red when: a stamped place's doors do not
   lead both ways; a template stamped twice fills in differently for the same spot.
5. **Life.**  Spots, the derived pools, bands and days, the clock the developer drags.  Red when:
   a spot's words and what the game derived from them disagree (WORLD.md step 3's checks).
6. **Challenges.**  The templates, the wiring, the graph and its checks.  Red when: a world the
   checks call solvable is not solved by the scripted solver; one they call stuck is solved.
7. **Export, import, the budget.**  The archive out and in, the Tanmatsu's budget on screen.  Red
   when: a world exported and imported differs in one byte; a world the budget calls fitting does
   not build or does not fit on the device.
