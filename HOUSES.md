<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# House templates: the way in, the inside as left, the cellar

The catalogue of templates an ordinary house of Orbury is dealt from, made of what the game
already has.  **Everything here is proposed** (◇), for the owner to rule template by template; the
rulings it follows are the owner's:

- every ordinary house is dealt from three templates: **how it was locked, with a possibility to
  get inside**; **how its owners left its inside**; **its cellar, including the way to get into
  it** (owner, 2026-11-10; PUZZLES.md § Houses);
- not every house has a cellar: none marks a wrong house for the ghost's urn, and a cellar can be
  hidden (owner, 2026-11-10; STORY.md @ST-Urn);
- each house an opportunity: interesting enough that a player wants to go on to the next, with
  useful objects that are not critical, not there every game, never super easy to find (owner,
  2026-11-10);
- never unsolvable: hard, hidden and not straightforward are fine (owner, 2026-11-10);
- the ways in include climbing from another house, from up the hill, and an underground tunnel
  (owner, 2026-11-10); a bomb blast too, used sparingly: it may blow open an entrance the hero
  needs to go through (owner, 2026-11-10);
- Sibil's house is not predefined on the map: any ordinary house could in principle be hers
  (owner, 2026-11-10);
- the urn is a real carried item, taking the hands: the hero may get in more easily without it,
  and may need another route with it to get it there (owner, 2026-11-10);
- the town's layout and every house's shell are fixed; what is dealt is the state and the inside
  (owner, 2026-11-08).

**Built** marks a template whose every move the rules already make; **new** names what it would
need first; **verify** marks a move the rules probably make that no test shows yet.

## Templates as files

Every template in use is a file the owner can edit (owner, 2026-11-10): a name and a map part,
not necessarily a whole floor or house, so it can be applied freely, turned and mirrored.  Each
is a place file under `levels/templates/` (JSON, EDITOR.md § 2) with a `template` block:

```json
{
  "name": "cellar_weighted_hatch",
  "template": { "kind": "cellar", "turns": true, "flips": true,
                "about": "the stairs down weighted with a rock: pushed off, hands empty" },
  "legend": { ".": "floor", "#": "wall", ">": "stairs_down", "<": "stairs_up" },
  "ground": [],
  "chunks": [
    { "cx": 0, "cy": 0, "z": -1, "rows": [ "#####", "#<..#", "#...#", "#...#", "#####" ] },
    { "cx": 0, "cy": 0, "z": 0, "rows": [ "", " >" ] }
  ],
  "things": [ { "at": [1, 1, 0], "stack": ["rock"] } ]
}
```

- **`kind`**: `way_in`, `inside`, `cellar` or `tunnel` (§ 1–4), or `find` (§ 2's finds, laid where
  the deal puts them).  **`turns`**, **`flips`**: whether
  it may be laid turned by quarters and mirrored east to west; **`about`**: a line on what it is.
- **The map part** is the place file's own: rows through the legend, `things`, `edges`, `bars` (a
  door's beam side), `enemies`, `rises`, `slants`.  Layer 0 is the layer it is laid on, −1 the
  cellar below, 1 the floor or roof above.
- **A space is the house's**: a cell left unset (a space in a row) keeps whatever the house has
  there, so a template is only the cells it changes and takes the house's shape it is laid on.
  A thing on an unset cell still replaces what stands there.  Edges are added or changed, never
  taken away.
- **Drawn facing one way**: a way in is drawn with the house's inside to the north and the outside
  to the south (`way_window_step`: the wall's row 0, the yard's row 1); the deal turns it to the
  wall it is laid on.  Turned, everything turns with it: cells, edges, slants, the beam's side,
  the things and the figures.

**Edited** in the editor: type `templates/<name>` in the panel's name field and press OPEN, edit,
and SAVE writes it back to `levels/templates/<name>.json` (never into the play order); or by hand
in the JSON.  **Applied** by `stamp.loft` (`stamp::apply`: at a cell, turned, mirrored);
`tests/stamps.loft` checks every file loads with its kind and survives a save unchanged, and plays
the window's step, the weighted hatch and the barred door in every way they can lie.

**The files**, the whole catalogue below (45):

| Kind | Templates |
|---|---|
| way in | `way_barred_front`, `way_back_door`, `way_back_door_overgrown`, `way_boarded_door`, `way_shuttered`, `way_chair_behind_door`, `way_broken_sill`, `way_window_step`, `way_roof_hole`, `way_roof_hatch`, `way_roof_hatch_weighted`, `way_slumped_wall`, `way_walled_up`, `way_upper_door` |
| inside | `inside_pushed_to_walls`, `inside_stacked`, `inside_barred_room`, `inside_in_haste`, `inside_roof_fall`, `inside_stair_rotted`, `inside_floor_hole`, `inside_rat_nest`, `inside_grey_rat`, `inside_bats_upstairs`, `inside_spider_upstairs` |
| find | `find_potions`, `find_boost_potion`, `find_hearth`, `find_larder`, `find_tar_barrel` |
| cellar | `cellar_none`, `cellar_open`, `cellar_weighted_hatch`, `cellar_hidden_hatch`, `cellar_in_barred_room`, `cellar_under_fall`, `cellar_outside_door`, `cellar_tunnel_only`, `cellar_stair_rotted`, `cellar_flooded`, `cellar_dark_rats` |
| tunnel | `tunnel_drain`, `tunnel_neighbours`, `tunnel_trial_digging`, `tunnel_vault_onwards` |

Not files, and why: **from the house beside** and **from up the hill** are where a house stands
in the town (its roof against the next one's, or the terrace behind), so their templates are the
roof's (`way_roof_hole`, `way_roof_hatch`); **both doors barred** is `way_barred_front` laid on both
doors with a roof hatch; **put away** is where the deal lays the finds; **lived in** is a house's
people, not a map; **the blast** works on any wall, and `way_walled_up` is the house where nothing
else does.  **The barrel and a chair** as a step is out: a chair lifts only onto a table (`surface`),
not onto a barrel.

Played by `tests/stamps.loft` in every way they lie: the window's step, the weighted and the hidden
hatch (a chair walked into is pushed, so the middle one is pulled out and pushed aside), the
barred door, the boarded door (a pick takes it apart, bare hands do not), the barred room and the
hatch under the fall (their stones give to no push), the weighted roof hatch, the chair behind the
door, and the rotted cellar stair: a drop onto the table below costs nothing, and the way back up
is the chair pulled onto that table and the wall's top beside the hole.

## The deal

Built (2026-11-10, `deal.loft`, `tests/deal.loft`).  When a game starts, and when the hero first
enters a place, every house its intent marks `"dealt": true` is dealt from the templates by the
game's seed (`TW_SEED`, else the clock).  In Orbury those are the eleven ordinary houses: the
middle terrace's four cottages and the seven miners' rows (`tools/orbury_gen.py`, `dealt=True`).
The lord's house, the inn, Agnes's, the well-off house, Hollis's and the smithy keep their own.

**Per house, in one draw**: a closing on its front door (a template that `closes`); a way in (one
that `gives`) unless the closing is one, laid on an outer wall turned so its inside faces into the
house, or within the house; none to two of the inside as left; a cellar (`cellar_none` among them);
and one time in three a find.  One ordinary house is Sibil's, by the seed, and is never dealt
`cellar_none`.

**Played before it is kept** (never unsolvable): on a copy of the place, the hero walks from the
town (the cells reached from the place's start before the deal) to the way in's start, plays its
solution as the template lies, with what it `needs` in the hands, and must then stand inside the
house; then walks on to the cellar's start and plays its solution, and must stand in the cellar.  A
draw that fails is drawn again, up to 40 times; a house no draw passes stays as the layout has it.
The walk never pushes, pulls or drinks, so a template's things that block the way fail the draw.
A template whose solution is broken is never dealt: `way_broken_sill` with its moves reversed
appears in no house.

**What a template tells the deal**, in its `template` block (editable like the rest):

| Field | Means |
|---|---|
| `at` | `door` (on the front door), `wall` (on an outer wall, not a corner or the door), `inside` (within the floor), `under` (a cellar, within the footprint); none: not dealt |
| `wall` | `[x, y]`: the template's cell that lies on the wall or the door |
| `closes`, `gives` | it shuts the front door; the hero gets in by it |
| `weight` | how often it is drawn (1 when not said) |
| `needs` | what the hands hold for its solution (`pick`, `axe`) |
| `solution` | the moves as drawn, from the file's `start` cell (`rules::play`'s words; `~700` waits) |
| `floors` | the house it is drawn for: its layer 1 is a one-floor house's roof (`way_roof_hole`) or a two-floor house's upper floor (`inside_bats_upstairs`); 0 any |

**Not dealt yet** (`at` none), each for a reason: `way_window_step` and `way_upper_door` want a
house of two floors (the ordinary ones have one); `way_shuttered`, windows the ordinary houses do
not have; `way_walled_up`, a directed blast in its solution; `cellar_in_barred_room`, a house
bigger than the cottages; `cellar_under_fall`, `cellar_outside_door`, `cellar_tunnel_only` and
the tunnels, a second way that the deal does not lay yet.

**The play log** gets the deal: `DEAL seed 7 sibil 5`, then a line per house with its draw, its
cellar and every template where it lies, turned and mirrored.  `TW_SEED=7 loft --lib libs
tools/map.loft orbury OUT.png` pictures that deal whole.

**Sibil's cellar** is laid as the level's `rest` (its floor cells, saved with the place): the urn
brought there brings the ghost to rest (STORY.md § 6, `urn.loft`).

**The urn's own check** (2026-11-10): Sibil's house is played once more.  Once the hero alone has
opened the way (the barrel pushed off, the bushes felled, the boards taken apart), the hero walks
from the town again with the urn in the hands, which push, pull and lift nothing, and must reach
her cellar's floor, by the hero's way or another (over the roof, down the hole).  That is the
"open the way, then fetch it" pattern (§ Carrying the urn), and a house that passes it is one the
urn can always be brought to.  A draw that fails is drawn again; a house no draw passes as Sibil's
is dealt as an ordinary one, and she moves on to the next.  With the urn's walk broken on purpose,
no house is Sibil's, and every house is still dealt.

**Still to come**: tunnels and the cellars that need them; the finds laid in the cellar
rather than on the ground floor; two-floor houses.

## What the town gives to work with

What a house's templates are made of, from `kinds.json` and the rules as they stand:

| Thing | What it does at a house | Rule |
|---|---|---|
| a door | walked into, it opens | built |
| a door held shut (`barred`) | its beam lifted from the side it opens towards, the beam in the hand; from the other side only a directed blast opens it | built |
| a window edge | seen through, never walked through | built |
| a fence edge | climbed over as a step of 1, nothing pushed across: a low sill, a garden fence | built |
| an opening in an outer wall (`openings`) | a cell of the wall made floor, a door or a niche, with an edge of its own on the outside | built (the inn) |
| a roof hole (`holes`) | the roof gone over those cells, open to the sky | built (the inn) |
| a roof | walked on (a floor-kind material); two roofs that meet are one way | built |
| stairs | taken when nothing stands on them; a thing on them stops them | built |
| a chair (1, pushed and pulled, lifted onto a table) | a step; on a table, a step of 3, which climbs a wall to the next layer | built |
| a table (2, never moved) | a step for a chair; a landing that makes a drop from the layer above free | built |
| a rock (2, pushed only) | a weight on a hatch; two touching are rubble nothing pushes | built |
| a tar barrel (2, pushed) | a weight, a step of 2; dips a stick; lit, a flame and a light | built |
| a barricade (2) | a doorway boarded: taken apart with a pick (two beams), or burnt away by a lit barrel | built |
| a bush, a tree | stands in the way; an axe fells it, for sticks | built |
| a socket | holds a torch: a light where the hero's hands cannot be | built |
| shallow water | walked through; a beam floats | built |
| a directed blast | cracks the first wall ahead or blows out a door held shut: there is no wall it cannot crack | built |
| a fall | one layer costs 1 health; onto something 2 high, nothing | built |
| darkness, a torch | rooms are dark; a burning torch lights, and at night gives the hero away | built |
| being seen | going into a home while seen sets `snooped` | built |
| a grey rat | out by day, carries forage left lying to its nest | built |
| a rat, a bat, a spider | a rat keeps out of light; a bat drifts to it; a spider climbs walls and flees to another layer | built |

Two limits shape every template.  The hero steps up 1 and cannot climb up through a ceiling, so
the way up is a step of 3 against a wall or stairs.  And the hero's hands hold one thing, and
holding anything they cannot push, pull or lift (`hands_busy`): the urn above all (§ Carrying the
urn).

## 1. How it was locked, with the way in

The family closed every way in they could think of; the template names the one left open, by
time or by their oversight, and what it takes.  A house is dealt one closing and one or two ways
in.

**The closing** (the family's care, PUZZLES.md § Where the town's puzzles come from):

| Closing | How it is made | Rule |
|---|---|---|
| barred from inside, left by the back | the front door `barred`, its beam inside; the back door closed behind them | built |
| both doors barred, left by the roof hatch | `barred` front and back; a roof hatch (below) the family went out by | built |
| boarded | a barricade in the doorway, inside or out | built |
| shuttered | every window a wall edge where it was a window | built |
| a chair against the door | the door opens onto a chair, pushed in as the hero steps; onto a table, it does not give | built |

**The way in** (one is always open, PUZZLES.md § Never unsolvable):

| Way in | What the hero does | Needs | Rule |
|---|---|---|---|
| **the back door** | round the house to a door the family only closed | getting round: a garden fence climbed, bushes felled with an axe, a rock pushed off the path | built |
| **the beam from the right side** | in by another way, the front door's beam lifted from inside: the house opened for good | another way in first | built |
| **a broken window** | a window whose sill is low (a fence edge), climbed through on the ground floor | — | built |
| **the upper window, by a step** | a window opening on the upper floor, reached by a step of 3 against the wall below it: a chair pulled onto a table (not a barrel: a chair lifts only onto a table) | the step's things, found nearby | built (`way_window_step`) |
| **the fallen roof** | onto the roof, and down through the hole into the room below: onto the table under it for free, onto the floor for 1 health | a way onto the roof (below) | built |
| **from the house beside** | along the row's joined roofs to a hole or a roof hatch in another; the house beside is the way in | the neighbour's way up | built |
| **from up the hill** | from the street or terrace above, onto the roof that lies at its layer: a house against the slope is walked onto from behind | — | verify (each house's roof layer against the terrace behind, `tools/map.loft`) |
| **the upper door on the street above** | a house against the slope with its upper floor's door on the street behind, the family having barred only the lower | — | new (a house's doors on more than one level, BUILD.md D1c open) |
| **the roof hatch** | stairs down from the roof to the top floor; a weight on them from above, or none | a way onto the roof | built |
| **the boarded door** | the barricade taken apart with a pick (two beams, the hero's to keep, owner 2026-11-10) or burnt by a lit barrel | a pick, or a tar barrel and a flame | built |
| **up from the cellar** | in by a tunnel (§ 4) to the cellar, up its stairs into the house | the tunnel's way | built |
| **the blast** | a directed blast cracks a wall or blows out the barred door: always there, never quiet; used sparingly (owner, 2026-11-10), and in a house or two the entrance the hero needs is one only a blast opens | a bomb | built |

**What makes it interesting** (◇): the way in is often seen before it is reached (the hole in the
roof from the market, the upper window from the street), and the tool it asks for is often not yet
the hero's.  So a house is left and come back to: with the axe for the bushes, the pick for the
boards, a bomb for the stubborn ones.  **The blast is used sparingly** (owner, 2026-11-10): it may
be what blows open an entrance the hero needs to go through, but only in a house or two, so a bomb
brought up from the mine is a key for those and not the town's answer.  It always works where it
is used, and always costs: it is heard, and a villager's opinion of the hero follows (`snooped`,
and what the blast's noise brings).

## 2. How the owners left its inside

The family's care laid over the plan, and then time's neglect over that (PUZZLES.md § Where the
town's puzzles come from).  A house is dealt one arrangement and up to two neglects; how much
neglect follows from how long the family has been gone.

**The arrangement** (the family's care):

| Left | What it is | Rule |
|---|---|---|
| **pushed to the walls** | the chairs against the walls, the tables where they stand: a room's middle clear, its steps put away | built |
| **stacked** | a chair on a table: a step of 3 already made, in the wrong place for the hero | built |
| **barred room** | an inner door blocked from the other side by a rock with a table behind it, which no push moves, so the room is reached by another way (the hole above it, the window, the cellar) | built |
| **put away** | what was worth something carried down into the cellar or up into the upper floor; the ground floor bare | built |
| **in haste** | the arrangement half done: a chair fallen, a door left open, a light in the socket long burnt down to a stick | built |
| **lived in** | nothing closed, someone at home by the clock: the house a navigation part, not a lock (PUZZLES.md, Hollis's house) | built (people by their points) |

**The neglect** (time):

| Neglect | What it is | Rule |
|---|---|---|
| **the roof came down** | roof cells gone (`holes`), what fell below: a rock, or rocks touching that nothing pushes; daylight in, and the hero | built (the inn) |
| **a stair rotted** | the stairs to the upper floor gone, a hole above: the upper floor reached from the roof or a window | verify (a step of 3 up through a hole beside a ceiling) |
| **a hole in the upper floor** | a way down, a fall, and a place to look down through | built |
| **a wall slumped** | a gap in an outer wall at a corner, its stones a rock or two outside: a way in of its own | built (a wall cell made floor, rocks) |
| **overgrown** | bushes against the back door and the yard; a tree where the path was | built |
| **moved in** | a lair: a rat's nest in the kitchen, bats under the roof (drawn to a torch), a spider on the walls of the upper floor | built (the lairs' rules) |

**The finds** (owner, 2026-11-10: useful, not critical, not always there, never super easy) ◇:
dealt in some houses only, and put where the family would have put them, which is never the
open floor:

| Find | Where it would be | Why it helps |
|---|---|---|
| a health potion, a spirit potion | the cellar's shelf, a back room | the falls and the fights ahead |
| the boost potion (`potion`) | a miner's house, for the long shift | push strength for a while |
| sticks, a torch | the kitchen, beside the hearth | a light without crafting |
| herbs, mushrooms, dried | the larder | Hollis's recipes (TALKS.md) |
| a tar barrel | the cellar, the yard's shed | torches, and a flame for a barricade |
| a sack | the larder | the shoulder slot, for forage |

Each find lies behind the house's hardest step, not its first: on the upper floor under the fallen
roof, behind the barred room, in the cellar.  A grey rat in the house carries forage left lying
home to its nest, so a find of herbs can be found in its nest instead.  Bombs stay out: they belong
to the mine (LOCATIONS.md § 1, Wick's empty powder shed); the axes and the rope keep their fixed
places (PUZZLES.md § Chests and progression items have their place).

## 3. The cellar, and the way into it

The urn's question is answered here (STORY.md @ST-Urn), so this is the template a player keeps
coming back for.  A house is dealt one.

| Cellar | What it is | The way in | Rule |
|---|---|---|---|
| **none** | the house has no cellar: the wrong house | — | built |
| **open** | stairs down in the kitchen or the store | walk down | built |
| **the weighted hatch** | the stairs down with a rock or a barrel on them, the family's own weight | the thing pushed off, so the room has to have the space: hands empty | built |
| **the hidden hatch** | the stairs down under a chair or a barrel, in a corner, with other things like it along the wall: one of them is the hatch | each thing moved until the stairs show | built |
| **the hatch in the barred room** | the stairs in the room the family barred (§ 2): the cellar behind the arrangement | the barred room's way (the hole above, its window) | built |
| **the hatch under the fall** | the roof came down on it: rocks on the stairs, touching, which pushing cannot move | from below, by the cellar's other way; or a blast | built |
| **the outside door** | no hatch inside at all: the cellar's door is outside, at the back, under the bushes or behind the yard's fence | an axe for the bushes, or a fence climbed | built |
| **by the tunnel only** | no stairs from the house: the cellar was joined to another's, or to the stream bed (§ 4) | the tunnel | built |
| **the stair rotted** | the stairs down gone, a hole: a fall of a layer (1 health) into it, or a drop onto a table below for nothing | down; up again by the cellar's other way, or the chair pulled onto the table under the hole and the wall's top beside it | built (`cellar_stair_rotted`) |
| **flooded** | shallow water on the floor (the houses near the stream): walked through, a beam floating | wading | built |
| **dark, and lived in** | no light, and rats: they keep out of light, so a torch, held or in the cellar's socket, keeps them off | a light | built |

A cellar's own way is checked as its own part: a cellar entered by a fall needs a way out
(PUZZLES.md § Solvable: a doomed save is allowed, never a bricked game), and **a hidden hatch must
not be lost to a push**: a rock is pushed only, so a rock pushed into the corner over the hatch
cannot be pulled back.  Either the hatch is weighted only by what can be pulled (a chair, a barrel
pushed from the other side), or the cellar has a second way, or R and the well bring it back.

## 4. The tunnels

An underground way to a house is a small part of its own, between two ports: its far end, and the
cellar it opens into.  The town is a mining town, so the people dug.

| Tunnel | From | What it is | Rule |
|---|---|---|---|
| **the drain** | the dried stream's bed, a layer down (−1), beside the miners' rows | the culvert that drained the cellars into the stream; dry now, a pool in it where the bed has one | built (terrain a layer down) |
| **the neighbours' passage** | another cellar in the row | two families who knocked through, or a row whose cellars run as one under the joined roofs | built |
| **the old trial digging** | the hillside or the terrace behind, up the hill | the founder's first tries for ore under the town (@ST-Founder), found again: its mouth behind bushes, its beams standing | built |
| **the vault onwards** | the inn's vault under the market | the inn's cellar ran on under the market (LOCATIONS.md § 1); onwards to a well-off house's cellar beside it | built |

**What lies in a tunnel** (◇): a standing beam that holds it up (taken out with a pick, it
collapses the tunnel beyond: so the beam is wanted, and the way is lost for it), a barricade a
family put across its end (taken apart, or burnt), loose rocks to push, a rat run, the dark.  The
trial digging is the founder's own: in a game where Sibil's house is dealt one of the cellars it
ends in, the tunnel he dug leads to the house of his love.

## Carrying the urn

The urn takes the hands (owner, 2026-11-10).  What the rules let a hero do with it held, and what
not:

| With the urn held | Not with the urn held |
|---|---|
| walking, opening a door, stairs up and down | pushing or pulling anything: a chair, a rock, a barrel, a cart |
| a step up of 1: a chair, a table, a stack climbed | lifting a chair onto a table: building the stack |
| over a fence edge, a low sill | felling a bush with the axe, a barricade with the pick |
| along the roofs, down through a hole (the fall's cost as ever) | a torch held: the dark is the urn's, unlit |
| wading a flooded cellar | taking another thing: a find waits |
| lifting a door's beam: it falls at the hero's feet | a bomb thrown |

So every way in has **two costs, without the urn and with it**, and the deal can make them differ.
The patterns that come of it ◇:

- **Open the way, then fetch it.**  The hands' work done first and left done: the stack built
  under the window, the rock pushed off the hatch, the bushes felled, the torch set in the
  cellar's socket (a socket holds it; rats keep out of its light).  Then back for the urn, along a
  way that asks nothing of the hands.
- **Put it down on the way.**  Q puts the urn down before the hero: carried as far as the hands
  allow, set down, the hands' work done, taken up again.  Where it can be set down is the puzzle:
  not in the doorway the rock has to be pushed through, not on the cell a push needs free.
- **Another route for it.**  The hero's own way in needs the hands all through: a tunnel's rat run
  passed only with a torch held, a passage where a chair is pushed ahead the whole way, a way
  that a rock blocks behind the hero again.  So the urn goes in by another way, over the roofs or
  down the hole, the one that is longer but asks nothing.
- **The urn goes down first** ◇ verify: set down beside a roof's hole or a rotted stair, it falls
  to the floor below, and the hero follows by the way that needs both hands (or none).  It
  survives the fall (it cannot break, owner); whether a thing put down over a void falls is the
  rules' to say.

**Left behind** (owner, 2026-11-10): the urn cannot break, and it can be left in a house, but then
the ghost haunts the hero every night, until the urn is back in the graveyard or in the correct
spot; and waking from a haunted dream, the hero has 1 health less for the next day, a lower
maximum (owner, 2026-11-10).  On deep water it floats, closed over ashes that cannot spill
(owner), so it is never lost.  So setting it down to do the hands' work is free by day, and a house left unfinished
at dusk has a price, paid in the next day's falls and fights: the search can be paused, by taking
the urn back to the graveyard.

The template's check is run twice for it: the house solved by the hero alone, and the urn brought
from the street into the cellar (PUZZLES.md § Solvable, the urn a role that takes the hands).

**Sibil's house is dealt** (owner, 2026-11-10: any ordinary house could be hers).  The seed picks
one of the ordinary houses, and that house is dealt a cellar (never *none*) that the urn can be
brought into; every other house is dealt as ever, cellar or not.  Nothing on the map marks it, so a
player who knows the town still has to search it in every game.  The urn check above runs on
Sibil's house; the others need only the hero's.

**The beams are the hero's** (owner, 2026-11-10): a beam from a boarded door or a fallen roof can be
kept.  Taking one into the mine is not made hard by how the game is structured, but nothing aids
it either (owner, 2026-11-10): the hidden path and the cave-in are not inviting ways to walk with
a beam, and a player probably will not if it means leaving behind something more useful, a
weapon, for a beam takes the hands as the urn does.  So the town's beams are there for the
one who wants them, and the mine's own (LOCATIONS.md § 6) stay the ones the flotilla is built of.

## 5. A house dealt

A house is its shell (fixed), one closing with its ways in (§ 1), one arrangement and its neglects
(§ 2), perhaps a find, one cellar (§ 3), and perhaps a tunnel's end (§ 4).  Three examples of the
same miner's cottage, to show the spread:

- **closed with care, recently**: barred from inside, left by the back; the back door behind the
  garden fence; inside, pushed to the walls; the hidden hatch among three barrels; no find.  A
  short way in, a puzzle of which barrel, and with the urn a puzzle of where to set it down while
  the barrels are pushed.
- **closed in haste, long ago**: the roof come down over the kitchen; from the house beside along
  the roofs, down onto the table under the hole; a chair fallen, sticks by the hearth; the stairs
  down under the fallen rocks, so the cellar is reached by the drain from the stream bed.  The
  drain is dark and a rat's run: a torch held gets the hero through, not the urn, so the torch goes
  into a socket halfway along first, and the urn comes after through the light it keeps.
- **no cellar at all**: shuttered, boarded at the front, the back door overgrown; in with an axe;
  inside, stacked; a health potion upstairs, reached by the stacked chair moved under the rotted
  stair; and no cellar.  The wrong house, and still worth it.

## What is new

The templates ask little that is not built:

- **the urn's sign** in a cellar (STORY.md § 6), the one piece of the search itself that needs a
  rule; around it, the urn as a thing (unbreakable, floating on deep water), the ghost haunting
  the hero's nights while it is left elsewhere, and sleep that heals, a haunted dream taking 1
  off the next day's maximum;
- **doors on more than one level** of a house against the slope (BUILD.md D1c, open);
- **the deal**: the templates' files (§ Templates as files, built) drawn by the seed for each
  house and fitted into its shell, and the check that every house dealt is solvable (BUILD.md D1b, D1d, D1e; PUZZLES.md
  § Solvable);
- the **verify** rows above: a step of 3 up through the rotted stair's hole into the upper floor
  (`inside_stair_rotted`; the cellar's way up is played), and each house's roof against the
  terrace behind it.

## Open, for the owner

Nothing is open in the urn's search; every row marked ◇ above is still for the owner to rule.
