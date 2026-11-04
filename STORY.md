<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The game's story: a design to write before anything is built

The rough full game is a story the hero walks through, which in time takes in every puzzle idea
in some form (owner, 2026-11-04).  It is not about presenting everything at once.  Nothing of it
is built until four things are known: the story to tell, the people in it, the monsters and their
place in the world, and how the story can end (owner).

**An outline, not a world bible** (owner, 2026-11-04): this is foremost a game, and the world is
the backdrop for it.  The sections below are an outline; their details come from what the player
is meant to do in the story, not from working each subject out on its own.

**Whose ideas lead.**  The story is the owner's.  This file keeps three kinds of content apart:

1. **The owner's design** (§ 1–4): only what the owner has said or written.  A section with nothing
   yet says so; it is not filled in by anyone else.
2. **What exists** (§ 5): facts about the engine and the art as they stand, as material the story
   can draw on.  Not suggestions.
3. **Implementation notes** (§ 6): the engine side's thoughts on what a story choice would need.
   They follow the story; they never lead it.

The demo village (`levels/demo.json`, a house per mechanic) stays as it is, a world of its own
beside the story's, startable on its own in a browser and on the Tanmatsu, for anyone curious and
for our own tests (owner, 2026-11-04).

## 1. The story to tell

**The setting** (owner, 2026-11-04): the story centres on a decrepit, but not totally abandoned,
mining town.  The last remaining villagers struggle to make something of their lives.

What the owner has already said that bounds it:

- **Familiar look, surprising world.**  Trope-familiar pictures (Kenney's Tiny packs, a few extras
  in their style) over a world engine whose rules interact so that players who know the genre
  inside out are surprised.  The surprise stays fair: readable from the screen, consistent
  everywhere, never a dead end (2026-10-30).
- **One world, no levels.**  Places joined by doors, several doors a place, seamless; it runs on
  the Tanmatsu and in a browser, the browser carrying a full editor (2026-11-03).
- **Nothing is tutorialized.**  Places in their natural state, learnt by watching and by dying, as
  in Spelunky; danger readable from the world (sound, sight), never from a message (2026-10-30).
- **Hurt, rarely killed.**  No set-ups that easily kill; the hero gets hit and searches for
  potions (2026-10-30).
- **Puzzles first, growth never necessary.**  A dungeon is one big puzzle, not a grind; growth
  comes later and is never needed (DESIGN.md § The principle).
- **Light is a bonus, never a wall**; the semi-dark is always playable (2026-10-30).
- **Simple assets, a capable engine**: the engine carries the show, never richer art (2026-11-03).
- **Scripted is enough.**  Scripted versus simulated is irrelevant to the player, who sees a scene
  and appreciates that it is not always exactly the same: variety from alternatives drawn from
  seed and day, no unwatched simulation; habits change only from the player's choices or authored
  scripts (2026-11-03).

## 2. The people in it (NPCs)

**The villagers together** (owner, 2026-11-04): the villagers of the town know each other well and
depend on each other, but they have wildly different opinions on just about everything.

*Who each is follows from what the player is to do.*

What the owner has already said about NPCs:

- NPCs move between houses through the world; every creature has points it likes to be at at
  certain times; only the hero's place runs live (2026-11-03, WORLD.md).
- **Talking is moving** (2026-11-04, WORLD.md § Talking is moving): no dialogue menus.  Coming
  beside someone is to have a casual talk; otherwise one walks on, and they may bark ("hi", "what
  brings you here").  While a line is said there are always three choices: stay (let it play
  out), bump (get something out of them; holding a thing, bring it), walk away (get out).  Two
  arguing are answered by whom the hero goes to.  Neither choice is a big deal by accident.

## 3. The monsters and their place in this world

*The owner's outline to come; details follow from what the player is to do.*

What the owner has already said about monsters (DESIGN.md § Enemies):

- Enemies do not fight to the death: hurt, they go home to heal; a bested one leaves only what it
  visibly held.
- Monsters roam larger territories, in bands between points of interest the developer picks
  (2026-11-03, WORLD.md).
- Cyclopes make darkness a tool: hard to fight, in groups, passed by stealth, so a torch has a cost
  too (2026-10-30).
- The hunter is an evil hunter: it shoots as the hero does, hunts other monsters, prefers the hero.

## 4. How the story can end

*The owner's outline to come.*

## 5. What exists: material the story can draw on

Facts only, as of 2026-11-04.  Each is built and tested unless marked designed.

**The hero's world.**  Four-step layers, slopes, ceilings, falls (1/3/6 health for 1/2/3 layers),
wells as safe points (one rollback), health and spirit potions, money in chests, signposts,
doors, stairs, fences, trees felled by an axe for sticks, places joined by doors, text on the
ground.

**Moving things.**  Push and pull (any held item blocks both), one item a cell (light items pushed
aside), big things items rest on, rocks, barrels, chairs on tables, carts on tracks: rolling,
linking into trains, riding and leaning, junctions and levers, derailing into a step, pulling a
cart over a layer's edge.

**Light and fire.**  Sticks dipped in tar become torches, lit from a flame, mounted in sockets;
they burn out after ten minutes, fading as a warning; rats keep out of light, bats drift to it,
cyclopes see by it.

**Bombs and the bow.**  Bombs thrown, rolling, chaining in a ripple, pushing, flinging items,
lighting or dousing torches, scaring ordinary monsters, drawing cyclopes; arrows that throw levers
and set off bombs from across a chasm.

**Monsters built.**  Rats, bats, spiders (climb walls, flee to other layers), guards (block, step
aside when the area is clear), cyclopes (sight by light, shouts, groups, search, axes, bombs,
grabbing and pulling the hero home), the hunter.

**People built.**  Figures that talk (villager, elder, maid kinds), barks, the three choices,
gifts, give and take, a talk remembering its last choice.

**Puzzle ideas recorded** (INTERACTIONS.md, by number): the track hop (4), a bomb on a cart (7),
coupled junctions (8), a cart as a broom (10), a derailed cart as a step (12), water and stepping
stones (18), arrows as remote hands (20), a light on a cart (22), stealth against a cart (24),
sticks from above (25), lighting a room from afar (27), a blast as scare and decoy (28), a
cyclops's bomb used (29), the hunter's arrows (31); and the open rulings beside them (fire
spreading 21, darkness and the clear area 23, blasts through layers 16, …).

**Pictures available for people and creatures** (no new art needed; described as they read at a glance):

| Sheet | People | Creatures |
|---|---|---|
| Tiny Dungeon | a robed wizard (84), young man (85), bald man (86), horned-helmet warrior (87), a long-haired figure (88), a figure in a dress (99), knight (96), the hero (97), bearded man (98), grey-haired figure (100), hooded figure (111), green-hooded hunter (112) | slime (108), cyclops (109), red crab-demon (110), bat (120), ghost (121), spider (122), rat (123), grey creature (124) |
| Tiny Farm | two farmers (108, 109) | cows (120, 121), chicken (122), a small animal (123) |
| Tiny Battle | soldiers in five colours (106–179, two poses each) | — |
| tile-world.png | — | the horse (24) |

**Places the art can show.**  Dungeon rooms, a village (red and slate roofs, wood and stone
walls), castle walls, farm fields and crops, barns, lakes and shores, paths, forests, snow and ski
slopes (Tiny Ski).

## 6. Implementation notes (the engine side — not leading)

Written to be overruled.  Each note says what a kind of story choice would ask of the engine, so
the cost of a choice is known when it is made; none proposes story content.

- **Talk carries no shared memory yet.**  A talk remembers only where its own figure's next talk
  starts.  A story where people know what happened elsewhere (gossip, a reputation, an event the
  village reacts to) needs facts a line can set and other lines can depend on.
- **Barks rotate in order.**  A world that should feel lived-in may want them chosen by what the
  hero carries or how hurt it is, by a recent blast, by the time of day, drawn from seed and day.
- **Overheard scenes.**  Two people talking between themselves while the hero only passes would
  need a talk that runs at listening range, apart from the hero's own talk that starts beside.
- **Routines come first for "living".**  People moving through their day (WORLD.md step 3: points,
  activities, the clock) carries more life than any talk; talk then follows the activity.
- **Places are the cheap part.**  A new place is a file and a door; a story with many small places
  costs little engine work.  What costs more: things that follow the hero through a door, one clock
  across places, saves (WORLD.md steps 2 and 5).
- **Outcomes are state.**  Endings that differ by what the player did need that state saved and
  read (facts, as above); endings by place reached need nothing new.
