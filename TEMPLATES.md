<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Templates: how each figure acts

Every enemy, creature and person in the game, written out as its templates (DESIGN.md @GM-Combat,
*Enemies by templates*).  The owner ruled the system (2026-11-07):
- An enemy fights by templates: a set of specific actions, as a Dark Souls boss has, each with the
  conditions under which it may be chosen; some are aggressive, some defensive.
- The variety of templates gives figures, animals above all, qualities of their own, so how each
  behaves can be learned.
- Templates, with their timings in them, are a better way to make figures differ than statistics.
- Small enemies hit hard but wind up long.
- The body and the weapon are apart: a cyclops with another weapon, or none, still acts as a
  cyclops.
- Out of a fight, the same templates let people and creatures walk, talk and work in a manner of
  their own.

**Everything below is proposed**, but for what is marked *built*: every set, every condition and
every number, for the owner to rule kind by kind and for play to tune in the fight rooms.  Built
templates live in kinds.json (`templates`), each kind naming its own (`"templates": "swoop"`).  Where a template is a behaviour built
today, it says so: the template is then that behaviour written down, to be moved onto the template
machinery (BUILD.md B1c, steps 0 and 6) without changing it.

## How a template is written

A template has:
- **a type**: *aggressive* or *defensive* in a fight, *peace* out of one;
- **its conditions** (*when*): what must hold for it to be chosen;
- **its steps**: a fixed sequence, each with its time in ms;
- **its tell**: how its first moment is seen and heard, different from every other template of
  the same figure;
- **how it ends**: the gap it leaves, which for an aggressive template must fit the hero's lunge;
- **what breaks it** off, or branches it.

Steps are written `move time`, joined by `→`.  A blow names its damage (`bite 2`).

**The moves** are the hero's: `step in`, `step back`, `step aside`, `dodge`, `draw N` (a swing drawn
back for N ms; heavy from 500), `strike` (light flight 100 ms, heavy 200), `lunge`, `guard`,
`wait`, `turn`.  A kind's profile adds its own:
- `crouch`, `leap` (to beside, no reach), `hop` (an animal's dodge);
- `drop` (from a wall), `swoop` (a flier's pass), `sidle` (sideways only);
- `swell` and `engulf`, `charge`, `roar`;
- `grab` and `pull` (built), `roll bomb` (built), `call` (built), `draw bow`, `loose` (built);
- `swallow` and `carry` (built).

Peace moves: `walk to` (a point by the clock, built), `look round`, `turn to`, `gesture`, `work`,
`sit`, `doze`.

**What a condition reads:**
- distance (`apart N`) and line (in the hero's point, beside it, behind it);
- the hero's state: point ready, drawn back, out of balance, guarding, locked on this figure or
  another, turned away;
- light (the hero lit, this figure lit);
- its own health (*hurt*: a third or less);
- cornered (DESIGN.md § Enemies: no step away, no way home);
- a wall behind it or behind the hero;
- others of its kind near, and what they do (one holds the front);
- what it holds;
- the time of day, the place, facts known, where a talk stands.

**Choosing.**  Among the templates whose conditions hold, the choice is seeded by the figure's id
and its count of choices, so a fight is the same each time.  Defensive templates are kept to
while *patience* lasts (per kind, below).  When patience runs out, or the figure is cornered, the
aggressive ones may be chosen.  A template's *rest* (default 3000 ms) keeps it from coming twice
in a row while another is allowed.  A template begun runs to its end unless broken: by a hit, by
being out of balance, or by a clash.

**The hero's numbers these are measured against:**
- a step glides in 140 ms, a dodge in 100;
- a light swing flies 100 ms, a heavy one is drawn 500 ms or more and flies 200;
- the sword recovers in 400 ms, the axe in 600;
- out of balance lasts 500 ms;
- the hero has 6 health.

**The checks every set must pass** (BUILD.md B1c step 6):
- each template alone against a still hero, its timings by hand;
- each tell differs from the others of its figure;
- every aggressive template ends in a gap of 400 ms or more, so the hero's lunge fits;
- a blow of 2 or more has a tell of 400 ms or more;
- the base sword wins the figure's fight room with skill.

## Shared templates

Behaviours several kinds share, written once and named in each kind's set.

| Template | Type | When | Steps | Built |
|---|---|---|---|---|
| **wander** | peace | it does not see the hero | `step` at its wandering pace, the way chosen by its id and how far it walked, within its roam | yes |
| **home hurt** | defensive | hurt, not cornered | `step` at its chasing pace home by a way passing nowhere beside the hero; with none, a step away | yes |
| **heal** | peace | at its nest, hurt, nothing to search | `wait`, a point every 60000 (owner; built at 4000) | yes |
| **lie up wounded** | defensive | hurt, at its nest | stays there, off the road, until healed; drawn wounded (built: it sags and sways); comes out for nothing but **cornered** | no |
| **scared** | defensive | a blast heard within 8 (ordinary enemies) | flee the place 5000, then walk home | yes |
| **back off the blade** | defensive | the hero swings with nothing in reach, this figure 2 ahead, coming on | `step back 1` → `wait` a chase step | yes |
| **cornered** | aggressive | cornered | its first aggressive template, chosen at once (no patience left), never `home hurt` | yes (it fights) |

## Living there: scrounging, loitering, searching

**Healing is slow** (owner, 2026-11-07): about a point a minute at its home, nest or camp — not a
quick reset.  A wounded enemy is off the road for a while, and the hero can find it there wounded.
Proposed: a minute of the world's clock (60000 ms; a day is 20 minutes of play, so 20 points a
day), and a place left catches up its healing on entering, as it catches up the days (BUILD.md C1).
So a fight leaves its mark on a place: a cyclops beaten back leaves its post for minutes, the way
it held is open meanwhile, and the hero who comes back finds it lying up, sagging, still hurt —
where, at its nest, it is likely cornered and fights.  Ending it then or leaving it is the
player's choice (STORY.md @ST-NoKilling).

Enemies also scrounge for food, loiter around, or search for the hero when they have a reason to
(owner, 2026-11-07).  An enemy that lives in its place, and does not only wait for the hero, can
be watched, slipped past and lured: the stealth the story asks for (STORY.md @ST-Stealth) and the
puzzle a dungeon should be (DESIGN.md § The principle).  When an enemy does what is by the clock,
as people's points are (WORLD.md), never by a hunger meter: a routine to learn, not a number to
watch.

**Scared and wounded have templates of their own** (owner, 2026-11-07), with a different pace or
line, so the state of an enemy is read from how it moves, as everything else is.  Proposed: an
enemy's state chooses its set as what it holds does: *calm*, *searching*, *scared*, *wounded*
(a third of its health or less), *cornered*.  The shared **home hurt** and **scared** (built) are
each kind's default until its own are written.

| Kind | Scared (pace, line) | Wounded (pace, line) |
|---|---|---|
| rat | scurries at 250 a cell, along the walls, never across the open; into the dark first | limps at 700 a cell, stopping every 3 cells to look back; keeps to the dark |
| grey rat | drops what it carries and darts home at 200 a cell, the straightest line | creeps home by the long way round, under cover |
| bat | flies high and erratic, 150 a cell, out of the hero's reach and away from light | flutters low and slow, 400 a cell, settling often: the bow's chance |
| spider | up the nearest wall at once (built: through an opening) | climbs slowly, 900 a cell, and stays on the wall |
| crab | sidles straight into the water | sidles at 1200 a cell, claws up, its front always to the hero |
| slime | — (never scared) | shrinks and slides back to the water, 2500 a cell |
| bigfoot | — (brave) | walks home heavily, 1400 a cell, roaring back at whoever follows |
| Kharn | — (brave; a blast draws them, built) | walks home slowly, 900 a cell, calling its kind as it goes, its weapon dragged |
| hunter | breaks line of sight first, then runs home | limps to camp by covered ways, turning to loose an arrow at a follower |

- **A wounded enemy leaves a trail** (proposed): spots on the ground along its way for a minute,
  so the hero can follow it home and find it lying up (above).  Its line home is its kind's:
  learned, and so followed or cut off.
- **A scared enemy runs blind** (proposed): its line is the quickest away, not the safest, and it
  can run onto what it would avoid calm (a lit cell, past another enemy), never over a ledge.
  A grey rat scared drops what it carries: a theft undone.

### The area's unease

**A general anxiety level of the dungeon, per area, combined with all of this: what the player
does can make everyone more restless and on guard** (owner, 2026-11-07).  Proposed:

- **Per area**: a place, or an area a place marks (a part of the mine, the Kharn's halls, the
  village), set in its file and the editor's panel.  It is world state, kept through doors and in
  a save, like the clock.
- **Four steps, not a number to watch**: *calm*, *uneasy*, *on guard*, *alarmed*.  Nothing on screen
  shows it (DESIGN.md § What the player sees): the figures do.
- **It changes behaviour only, never health or damage** (templates over statistics): which
  templates are chosen, how patient a figure is, how soundly it sleeps, how far it searches.

| Step | What the figures do |
|---|---|
| calm | loiter, scrounge, sleep at night; a search is short, a sleeper wakes only for a blast |
| uneasy | loiter less and look round more; a noise wakes a sleeper; a search widens a step further |
| on guard | sleepers few, a watch posted at the area's ways in; groups keep together; patience halved; every reason searched |
| alarmed | no loitering: the area's fighters come to where the hero was last known; the Kharn call from further; the weak hide at their nests |

**Throwing bombs around is not without consequence** (owner, 2026-11-07).  Proposed: a blast
raises its own area two steps at once and its neighbours a step at once, heard through the rock,
not after a minute as other alarm spreads; a chain of blasts counts as one.  So a bomb that opens a
way also wakes what lives beyond it: where a puzzle can be solved loud or quiet, the loud way
costs the next stretch of the dungeon, and the Kharn, whom a blast draws (built), come to it on
guard.

**What raises it**, a step each besides a blast (proposed): a fight seen or heard by one who lives
there; one of its kind found ended, or coming home wounded; a theft found; a light seen moving
where none should be; a cyclops's call.  Two at once can raise it two.  **What lowers it**: quiet,
a step for every 3 minutes of the clock with nothing raising it, and a place left catches up on
entering, as with healing.

**It spreads**: an area alarmed raises its neighbours (through their doors) a step after a minute;
word gets round.  So a loud fight in the mine's upper level is felt below.  The village has an
unease of its own, raised by the Kharn's: people stay in at night, a watch stands at the
barricade, talk turns to the debate (STORY.md @ST-Debate).  The built tribe (BUILD.md C4: the
Kharn scouting and raiding) is a step the unease can drive later.

**Why it fits**: the quiet player meets a sleeping dungeon and passes through it, a puzzle solved
(@ST-Stealth); the player who fights everything makes each next fight harder, never easier, so
grinding is its own punishment (DESIGN.md § The principle).  And the area remembers: come back
an hour later and it has settled; come back at once and it is still on guard.

**Scrounge** (peace): goes to food it knows of, eats it there or carries it home.
- Its food is the world's: meat that hunted animals leave (built), forage lying (herbs, mushrooms;
  the grey rat carries it home, built), the farm's crops, what lies at the water's edge, and
  other residents for the Kharn (STORY.md @ST-Residents).
- So food is bait.  Meat put down with Q draws rats from their dark, and draws a cyclops off its
  watch.  Feeding, an enemy is busy and turned to its food: the moment to pass, or to strike its
  back.  What the hero carries in the sack can be smelled by some (below).

**Loiter** (peace): what it does at its nest or its spot, between other things.
- Each kind has its own, varied by its id: a rat grooms and sniffs round its nest, a bat hangs by
  day, a spider mends its web, a cyclops sits by a fire, sharpens its axe, or sleeps by night.
- **A sleeping enemy** wakes only for a reason (below): the dark and quiet are a way past it.

**Search** (defensive): goes to where its reason points, looks round, widens, gives up, goes home.
The built cyclops's search is this template (`walk to` the place → `look round 6000` → home).
- `walk to` the reason's place → `look round 3000` → `walk to` the next cell the hero could have
  gone to, along the way it would take → `look round 2000` → back home.
- How far it widens, and how long it looks, is its kind's: what makes a searcher learnable.
- Found the hero: it reacts as on seeing it (a cyclops shouts and calls, built).

**The reasons**, and who has which:

| Reason | What it is | Rat | Grey rat | Bat | Spider | Crab | Slime | Bigfoot | Kharn | Hunter |
|---|---|---|---|---|---|---|---|---|---|---|
| **a blast** | heard within 8 (built: the ordinary flee, the Kharn search) | flees | flees | flees | flees | flees | — | searches | searches | flees |
| **a noise** | a cart rumbling, a fight, a bump, a chop at a tree, within 6 | — | flees | — | wakes | — | — | wakes | searches | searches |
| **light** | a torch seen moving, or a flame set down where none was | avoids | — | comes | — | — | — | — | searches | searches |
| **a trace** | something changed in its place: a door left open, a thing moved, its food gone | — | searches | — | — | — | — | — | searches | searches |
| **a scent** | meat lying, or in the hero's sack, within 4 | comes | comes | — | — | comes | — | comes | comes | — |
| **a call** | one of its kind calling (built: the Kharn within 6) | — | — | — | — | — | — | — | comes | — |
| **a feeling** | the hero walks on the wall's foot it waits on | — | — | — | drops | — | — | — | — | — |
| **a theft** | its forage, or its kill, taken by the hero | — | searches | — | — | — | — | — | searches | searches |

So each kind has a few reasons and a manner of searching, and the player learns what gives the
hero away to whom.  Sound is what the dark does not hide: the Kharn see by light but hear in the
dark.

**Each kind's day** (by the clock; peace templates):

| Kind | Scrounges | Loiters | Searches |
|---|---|---|---|
| rat | meat and crops, by night, never in light | grooms, sniffs round its nest | — (comes to a scent) |
| grey rat | forage by day, carried home (built) | sits up, whiskers going | its nest's stores, when taken |
| bat | insects round light, at dusk | hangs from the ceiling by day, sleeping | — |
| spider | what its web holds | mends its web, still for long | — (drops on a feeling) |
| crab | the water's edge | sidles up and down its shore | — |
| slime | swallows what it comes onto (built) | sits still, wobbling | — |
| bigfoot | berries and roots by day | sleeps at night in its mine (STORY.md) | a noise that wakes it |
| Kharn | hunts the residents; raids the farm's crops later (@ST-Climax's lead) | sits by the fire, sharpens, sleeps by night; a watch stays awake | a blast, a noise, light, a trace (built in part) |
| hunter | hunts other enemies (built) | sits at its camp, fletching | a trace or a noise near its camp |

## The animals

Small animals have the knife's profile: no point, light, a lunge that is a leap to beside.  Their
danger is numbers, the dark and the sides, never one alone seen in light.

### Rat (brown; dungeon:123)

The pack animal.  Health 3, sees 5, shuns light; patience 1500.  It hits hard for its size with a
long, clear crouch, and comes in packs that hold the front and flank.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **leap** | aggr. | `apart 2`, not into a ready point (the hero's point turned off it, drawn back, or out of balance) | `crouch 450` → `leap 150` (aimed at the hero's cell when the crouch began) → `bite 2` → `recover 800` | flattens, a squeak | gap 800 |
| **flank** | aggr. | another rat holds the front; a side cell at `apart 2`, off the point, can be reached | `circle` at chase pace (350 a cell) to the side → **leap** | circles wide, outside the point | as leap |
| **hold the front** | def. | two or more rats, none holding the front | `step` to `apart 3` in the hero's point → `crouch 200` (a feint) → `hop back` → `wait 600`, again | a short crouch that never leaps | — |
| **hop** | def. | the hero's point turns onto it at `apart 2`, or the hero draws back | `hop` sideways 100 → out of balance 500 (a leap may follow) | a sideways skip | — |
| **back off the blade** | def. | shared | | | |
| **to the dark** | def. | lit by a flame, a dark cell within 4 (built) | `step` to the dark at chase pace | — | — |
| **home hurt**, **cornered**, **wander**, **heal**, **scared** | | shared | | | |

Breaks: a hit while crouched breaks the leap (no poise); a leap into a ready point is hurt 1 and
flung back, out of balance.  Learned: a rat seen in light is easy; three in the dark, or one
behind the hero, are not.  A torch keeps them off (built), and a corridor keeps them in front.

### Grey rat (dungeon:124)

The thief: a second rat look, kept only for a way of its own (owner).  Health 2, no bite, out by
day; patience: none needed, it never fights.  Its templates pair with Q: what the hero puts down
can be stolen.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **forage home** | peace | by day, forage lying in sight (built) | `walk to` it → `carry` → home at chase pace | — | — |
| **snatch** | aggr. | an item put down by the hero lies within 3, the hero 2 or more from it | `freeze 600` → dash to it (`step` 250 a cell) → `carry` → dash home | freezes, whiskers twitch | it carries the item home; struck, it drops it |
| **dart past** | def. | the hero comes beside it, or cornered | `hop` past the hero's side 100 → run home | a squeal | — |
| **home by night** | peace | night | `walk to` its nest | — | — |

Learned: never leave the sword lying where a grey rat lives.

### Bat (dungeon:120)

The swooper.  Health 1, flies, likes light, quick; patience 800.  Its line is hard to read, so
its tell is heard more than seen.

**Built: the swoop** (2026-11-07; `tests/templates.loft`; kinds.json `templates.swoop`): chosen 3–6
steps from the hero in sight; a chirp (heard), a zig across the line and a zag back (120 ms each),
the aim on the hero's cell, a dive at 100 ms a cell to beside it (up to 5), the nip for 1 a 100 ms
later — on the aimed cell only — and 3 cells away at 120 ms each; it rests 2000 ms, keeping 3 or
more away meanwhile.  Struck as it comes in, it is ended.  The rest of the bat's set below is
still proposed.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **swoop** | aggr. | `apart 3` or more, in flight | zig 120 → zag 120 → `swoop` (dive to beside) 120 → `nip 1` → away 3 cells, 120 a cell | chirps, wingbeats quicken | gap 360 flying away; the bow's moment |
| **circle the flame** | def. | the hero holds a flame | circle at `apart 2`, the brightest cells (built drift), turning with the hero | — | — |
| **behind the flame** | aggr. | circling the flame, the hero turned away | **swoop**, from behind | the wingbeats behind | as swoop |
| **rise** | def. | a swing drawn at it | `hop` up and back 100 (out of a sword's reach for 400) | a flutter up | — |
| **drift to light** | peace | wandering (built) | the brightest step, never back, never onto a flame | — | — |

Learned: a torch draws bats and burns them (2, built); the bow takes them as they fly off.

### Spider (dungeon:122)

The one from above.  Health 2, climbs walls, flees through openings to another layer (built);
patience 2500, the most patient animal, waiting on its wall.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **wait on the wall** | def. | on a wall face, the hero not beneath it | `wait`, turned to the hero; out of the sword's reach (built), the bow's target | — | — |
| **drop** | aggr. | on a wall, the hero passes the cell below or beside it | `tense 500` → `drop` beside the hero 150 → `bite 2` → `recover 900` → climb back 600 | legs drawn up, a click | gap 900 on the floor; climbing back, struck from below |
| **leap** | aggr. | on the floor, `apart 2` | `crouch 400` → `leap 150` → `bite 1` → `recover 900` | the rat's crouch, higher | gap 900 |
| **flee up** | def. | hurt or scared, an opening near (built) | climb through it to another layer | — | — |

A point held level does not meet a drop from above: the hero looks up, steps out from under the
wall, or shoots.

### Crab (dungeon:110)

The shelled one: its front is armour.  Health 3, within a cell of water, walks only sideways, east
and west (built); patience 2000.  Struck from in front by a light blow, its shell turns it aside;
from its side or back it is open.  So the crab teaches the backstab without a rule of its own.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **sidle** | def. | sees the hero | `sidle` east or west at 600, keeping its front to the hero | — | — |
| **claws up** | def. | the hero `apart 2` in front, its point ready | `guard` (claws raised, the shell turned) until the hero moves | claws raised | — |
| **pinch** | aggr. | the hero `apart 1` in front | `claws back 500` → `pinch 2` (strike 100) → `recover 700` | claws drawn wide, a clack | gap 700 |
| **turn and pinch** | aggr. | the hero beside its east or west side | `turn 300` (its front to the hero) → **pinch** | the turn itself | as pinch |
| **to the water** | def. | hurt | `sidle` to the water | — | — |

Learned: go round it; strike its side while it turns.

### Slime (dungeon:108)

The slow one.  Health 3, within 2 cells of water, swallows an item it comes onto and lets it go when
ended (built); patience none (it simply comes on), and it never dodges.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **creep** | aggr. | sees the hero | `step in` at 1500 | — | — |
| **engulf** | aggr. | the hero beside it, still for 1000 | `swell 900` → `engulf 2` → `recover 1500` | swells and wobbles, a wet sound | gap 1500 |
| **swallow** | peace | on an item lying (built) | `swallow` it (it holds it in sight) | — | — |
| **draw back** | def. | struck | `shrink 300` → `step back` | — | — |

Learned: never stand still beside it.  What it swallowed is taken back by ending it.  An item put
down near water may be swallowed.

### Bigfoot (Tiny Ski's 78)

The big one of Stormcap.  Health 12, heavy, poise, out by day; patience 3000.  The owner defines
the unique ways to deal with him when Stormcap's terrain is built: these are placeholders that
use only the general rules.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **stand tall** | def. | `apart 3` or more, the hero's point ready | beats its chest 1200 → `step` 900 | the chest beat | — |
| **charge** | aggr. | `apart 4–6`, in a row or column, the way clear | `roar 800` → `charge` 3 cells at 100 a cell → `slam 3` | the roar, head lowered | missed (dodged aside), it runs on until it meets something: against a wall stunned 2000; gap 1500 otherwise |
| **sweep** | aggr. | `apart 1–2` | `draw 900` → `strike` heavy over 3 cells (250) → 3 → `recover 1200` | arms drawn far back, level | gap 1200 |
| **home by night** | peace | night (built) | `walk to` its den | — | — |

Learned: a charge is dodged aside; it is put against the rocks, never met head on.

## The Kharn (the cyclopes)

**The body** (dungeon:109): health 8, poise, sees by light (8 lit, 2 in the dark), brave, calls
its kind within 6, searches.  Patience 1200: the Kharn are eager.  The body's templates are every
cyclops's; what it holds adds its fighting set.  What a cyclops holds is set where the level
places it: an axe by default, `"bomb"`, `"none"` (built), and later others.

| Template (every cyclops) | Type | When | Steps | Tell | Built |
|---|---|---|---|---|---|
| **shout and call** | aggr. | sees the hero anew | `shout` → `call` its kind within 6 → comes at the hero (450 a cell) | the shout | yes |
| **search** | def. | lost the hero; or a blast within 8 | `walk to` the place → `look round` 6000 → home | stands looking, head turning | yes |
| **hunt** | peace | sees a rat, crab or other resident (STORY.md @ST-Residents: their food) | comes at it, strikes; carries it home | — | no |
| **heal at the nest** | peace | hurt, nothing left to search | `wait`, a point every 60000 (owner; built at 4000): 8 minutes from one point to full | — | yes |

The fighting sets, by what it holds:

| Holding | Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|---|
| **an axe** | **sweep** | aggr. | `apart 1–2` | `draw 700` (built) → `strike` heavy over 3 cells (250) → 2 → `recover 1200` | axe swung far back, level | gap 1200; cannot be parried incoming |
| | **chop** | aggr. | `apart 1`, the hero guarding | `draw 900` (raised overhead) → `strike` heavy, one cell (200) → 3 → `recover 1400` | axe raised high | gap 1400; breaks a guard |
| | **advance** | aggr. | `apart 3–4`, the hero's point ready | `step in` with the axe raised, 450 a cell | the axe raised as it walks | — |
| | **shove** | def. | the hero inside (`apart 1`) after its swing | `shove 300`: the hero back a cell (DESIGN.md *Position*) → **sweep** | a shoulder dropped | — |
| **bombs** | **roll bomb** | aggr. | `apart 5–8` in a row or column, the way clear (built) | `roll bomb` | the bomb drawn back, heard | — |
| | **grab** | aggr. | beside the hero, bombs in hand (built) | as below | | |
| **nothing** | **grab** | aggr. | beside the hero | `arms wide 600` → `grab` (built) → `pull` towards the nest, a cell every 800 | arms spread wide | the hero breaks free in 3 presses (built); a dodge in the 600 avoids it |
| | **rush** | aggr. | `apart 2–3` | `step in` 300 a cell → **grab** | head down | — |
| **a spear** (later) | **thrust** | aggr. | `apart 3`, its point ready | `draw 250` → `strike` light at 2 cells (100) → 2 → `recover 600` | the spear drawn back | gap 600 |
| | **keep off** | def. | the hero comes inside (`apart 1–2`) | `step back` → `step back` | — | in trouble when it cannot |

A cyclops disarmed (open in DESIGN.md: a clash or a heavy blow knocking its weapon away) takes the
*nothing* set at once: a boss's second phase without a rule of its own.

### Urgha (kind `urgha`)

The chief's second, who knows broken words (STORY.md @ST-Cyclopes; TALKS.md: `clear`).  The body
of a cyclops, but not hostile; health 8, no blows.

| Template | Type | When | Steps |
|---|---|---|---|
| **stand watch** | peace | no talk | stands tall at its point, `look round` 3000, never its back to the hero |
| **broken words** | peace | a line said | `gesture` (points at itself, at the mine, at the hero) on each line; a grunt between words |
| **unclear** | peace | a line said before `clear` holds | `turn` away 400 → back; a shake of the head |
| **warns off** | peace | the hero comes past it towards the mine | `step` into the way → arms wide 600 (the grab's tell, never the grab) |

## People with weapons

### The hunter: Rennick (kind `hunter`; dungeon:112)

A bow: no point, so close up he is in trouble, as a spearman is.  Health 3, sees as a cyclops does,
flees hurt, scared by blasts, hunts other enemies when no hero is seen (built); patience 2000.

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **shoot** | aggr. | the hero in a row or column, `apart 4–8`, the way clear (built) | `draw bow 600` → `loose` (30 ms a cell, built) → `recover 1500` (built) | the bow drawn, a creak | gap 1500 |
| **keep range** | def. | `apart 2–3` | `step back` → `step back` | — | — |
| **out of line** | def. | the hero holds a bow in his row or column | `step aside` out of the line | — | — |
| **shove** | def. | the hero beside him | `shove 300` (the hero back a cell) → **keep range** | the bow raised across | — |
| **hunt** | peace | no hero seen, another enemy seen (built) | **shoot** it, or go after it | — | — |
| **keeps apart** | peace | in the village | stands at the edge of things, leaning, watching; turns his back on villagers | — | — |

### The swordsman (no kind yet)

A human swordsman, the profile the fight rooms are tuned on (BUILD.md B1c step 6: the rat and a
swordsman first).  No one in the story fights so yet; Sergeant Voss's Wardens, Sable, or the Kharn
with blades may.  Health 4, a sword, no shield; patience 3000 (trained).

| Template | Type | When | Steps | Tell | Ends |
|---|---|---|---|---|---|
| **hold the standoff** | def. | `apart 3`, the hero's point ready | `guard`, the point in its tile; `wait` | the point held still | — |
| **punish eagerness** | aggr. | `apart 3`, the hero drawn back | `lunge` | — (it is the answer) | settles back; gap 400 |
| **step in swinging** | aggr. | patience gone, `apart 3` | `step in` with `draw 200` → `strike` light | a shift of weight forward | a clash, or the hero hit |
| **heavy** | aggr. | patience gone, the hero guarding | `draw 700` → `strike` heavy (200) → 2 → `recover 900` | sword drawn high | gap 900 |
| **feint** | aggr. | the hero holding the standoff 3000 or more | `draw 150` → let go of nothing → `guard` | a short draw | a lunge drawn, met by a clash |
| **circle** | def. | `apart 3`, the hero turning slowly (a heavy weapon) | `dodge` to the hero's side (out of balance 500) | a step aside | — |
| **back out** | def. | the hero inside | `step back` (guard up) | — | — |

## The people

People fight only at the village's defence (STORY.md @ST-Climax), so their sets are mostly peace.
What they say stays the talk's (talk.loft); where they go stays their points by the clock
(WORLD.md); what their work does stays the rules' (a barricade's units).  Templates give the manner,
so each is known by how it moves before a word is said.

**Every person's shared peace set:**

| Template | When | Steps |
|---|---|---|
| **walk to** | its point by the clock changes (built) | walks there at its kind's pace |
| **greet** | the hero passes within its bark's reach (built: a bark) | `turn to` the hero, a nod |
| **listen** | a talk is on, another speaks | `turn to` the speaker |
| **say** | its line is said | `gesture` its own way (below) |
| **called after** | the hero walks off mid-talk (built: `away`) | `turn to` the hero, a step after it |
| **rest** | evening, a bench or chair near | `sit`; at night `doze` |
| **work** | holds an axe, a barricade's pile near (built: C5) | `work` (the axe on the beam) every 2000 |

**The village's defence** (aggressive and defensive, for the climax; proposed, the owner's to rule):

| Template | Type | When | Steps |
|---|---|---|---|
| **hold the gap** | def. | the barricade attacked, holding an axe | stands in the barricade's gap, axe raised; `guard` |
| **chop at it** | aggr. | an enemy in the gap, `apart 1` | `draw 600` → `strike` heavy → `recover 1000` (no point: a villager is weak against what gets in) |
| **carry beams** | peace | the barricade attacked, holding nothing | `carry` a beam from the pile to the barricade |
| **fall back** | def. | hurt | runs home |

**Each kind's manner, and the named people the story has** (their manner from TALKS.md and STORY.md;
a kind marked ◇ is proposed, for a person no level places yet):

| Who | Kind | Walk | When saying a line | Own |
|---|---|---|---|---|
| a villager (the worker, the farmer, the helper, the wanderer, Tom) | `villager` | steady, 1400 a cell | plain gestures | the worker works at the barricade (built) |
| the elder (and the scholar, the keeper) | `elder` | slow, 2000 a cell; stops every 3 cells to look round | nods slowly, a long pause before each line | leans on the wall when resting |
| the maid | `maid` | hurried, never stops; turns quickly | glances aside, hands busy | carries things between points |
| the neighbour (the night street) | `neighbour` | slow, 1800, by night | speaks from its doorway, half turned in | goes in when the hero comes too close |
| Garth Mabley, the old foreman | `villager` ◇ | slow, a limp | points the way to tunnels as they were, sure of himself | stops at the mine's mouth and looks down it |
| Wick, the blaster | `villager` ◇ | quick, sidelong | says little: looks away, a shrug | handles a bomb when idle (unlit) |
| Agnes, the innkeeper | `villager` ◇ | brisk | counts off on the fingers who left | wipes the inn's tables, packs a bag |
| the Dunns of Thornfield | `villager` | steady, working | tired, looking at the fields | hoe the dry field; the horse's work |
| Lark | `villager` | quick, skipping; runs ahead | points to the hills | waits at the hidden path, looks back (built: a guide) |
| Hollis, the alchemist | `elder` | careful | holds up a potion to the light | mixes at the bench |
| Sergeant Voss | `guard` ◇ | stands; a sergeant's turn | stands straight, never steps closer | stands aside only when the area is clear (built) |
| Sable, the rival | `rival` | quick, sure | arms crossed, blocking the way | stands in a walkway (built); goes home at dusk |
| Tamsin | `villager` ◇ | measured | looks over the materials, notes them | arrives after the hero |
| Master Odran, the wizard | `elder` ◇ | slow, robe trailing | grumbles, waves a hand away | studies the mine's rock |
| the founder's ghost | `ghost` | drifts, through walls, by night (built) | sways; fades between lines | stands at the cemetery, looks towards the lake |
| the horse | `horse` | follows when led (built: following); grazes | snorts, turns its head | kept at the farm (built: `keeps`) |
| Urgha | `urgha` | above | above | above |

The Wardens (kind `guard`) stand, block, and step aside when the area is clear (built): their whole
set is that.  Whether they ever fight is the story's choice.

## The hero's test players

`tests/fights.loft` plays the fight rooms with scripted players; they become templates run for the
hero, on the same machinery:

| Player | Its templates |
|---|---|
| **the striker** | step in and swing whenever ready; never guards, never waits |
| **the patient fencer** | hold the standoff; lunge into eagerness; feint after 3000; back out when inside |
| **the dodger** | dodge every wind-up aside, then strike the side |

Their claims (DESIGN.md @GM-Combat): the striker is hurt in the rat run and the cyclops den; the
patient fencer and the dodger land their first blow later; the base sword wins every room.
