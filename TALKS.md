<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# What people talk about

A writer's sheet for the lines the game's people say (talk.loft, WORLD.md § Talking is moving):
what each one knows, wants and gets wrong, what the whole village argues over, and what play
changes.  Gathered from STORY.md only (2026-11-04); a cell marked *(open)* is not in the outline
yet.  The lines themselves are the owner's to write.

## Each person's material

| Who | Knows | Wants or cares about | Natural subjects |
|---|---|---|---|
| Garth Mabley, the old foreman (@ST-Foreman) | the mine's layout, outdated | the mine running again | the old tunnels as they were, the founder, the pump and lift as they used to work; where he is now wrong |
| Wick, the blaster (@ST-Blaster) | explosives; where bombs lie in the mines (owner) | blowing up the mine with the Kharn in it | the plan and how it could be done; where the bombs are; blasting, which he can teach the hero (owner); says little, and badly |
| Agnes, the innkeeper (@ST-Innkeeper) | who left, when and why; the state of the town and the court trouble behind it — the hero learns these from her, with the player (owner) | leaving | the abandoned inn, the Fenns, the people who went; the lord's fall and the decree |
| The Dunns of Thornfield (@ST-Farm) | the farm, the land drying | food; the horse's work | the drought, the creeping desert, too many mouths |
| Lark (@ST-YoungVillager) | the hidden paths, the brook, the crabs | *(open)* | the hills, places others do not know |
| Hollis, the alchemist (@ST-Alchemist) | potions; the crystal is beyond his trade | to be paid in resources, slightly more than the potions need (owner); needs convincing to go up Stormcap | the miners he used to patch up, the lightning; the strange fountains he cannot fathom (@ST-Fountains, owner); the ingredients he needs for potions (owner) |
| Sergeant Voss (@ST-Warden) | his orders; part of the court trouble (owner) | keeping the people out | little — the Wardens do not mingle; sergeant to sergeant he can talk, part of the court trouble included |
| Sable, the rival (@ST-Rival) | the mine, from his own way in | the same artifacts as the hero | rivalry; the traps he will not admit to |
| Tamsin, Lady Brigid's aide (@ST-Aide) | the witch's view; the materials | the materials, for Brigid | Lady Brigid and Greywood, the materials; arrives after the hero |
| Master Odran, the wizard (@ST-Wizard) | the magical materials deep in the mine | his own goals, hidden from his lord | grumbling; the witch he disapproves of |
| Rennick, the hunter (@ST-Hunter) | that the mine has stopped | *(a grudge)* | the village that cast him out |
| Urgha (@ST-Cyclopes) | broken words of the hero's language | *(open)* | what can be said in a few words |
| The founder's ghost, at night (@ST-Founder) | the first shaft, the lake that drowned him | *(open)* | the mine's beginning |

## What the whole village argues over

They know each other well, depend on each other, and disagree on just about everything
(@ST-Villagers): the Kharn; blowing up the mine against leaving (@ST-Debate); the decree and the
Wardens; the young lord who fell; Rennick's return; the newcomers; the drought; the farm; the
Fenns who fled; and the hero himself, a soldier of the lord who fell.

## What play changes

A talk can depend on what has happened (STORY.md § 6: shared facts; built, BUILD.md C2): the pump running,
tunnels drained, rope gone from Sable's spot, the lightning caught, the hero seen with Rennick,
the Kharn sighted.  Every villager having an opinion on each is what makes the village alive.

## What a line can do

The words are the owner's; this is what the engine does with them (`talk.loft`, a place file's
`talks`).  A line is `{ "<speaker>": "WHAT IS SAID" }`, and beside it:

| Key | Does |
|---|---|
| `next`, `end`, `ms` | the line after it (by `id`), the talk ending after it, its time set by hand |
| `bump`, `away` | the line for bumping someone taking part (one per figure: taking a side), and for walking away |
| `take`, `give` | the figure takes what the hero holds (a gift: goodwill +1), or gives it a thing |
| `then` | where the next talk starts, so a choice is remembered |
| `sets` | a fact made known when the line is said (the alchemist's lore) |
| `when` | on a bark: said only while it holds — a fact, `!fact`, `liked` or `disliked` (goodwill) |
| `clear` | broken words: until that fact holds, every other word comes through as `..` (Urgha) |
| `follows`, `stays` | the person follows the hero, through doors too, or stays where it is |
| `guides` | `[x, y, z]`: the person walks the hero there, waiting when the hero falls behind; then `guided` is known |
| `keeps` | whoever of that kind the hero leads stays here (the horse lent to the farm) |

A talk itself has `barks` (lines said in passing, in turn), `gifts` (a thing held when bumping →
its line), `facts` (a fact known → the line the talk starts at, e.g. `"stuck": "hint"`) and
`recipes` (what the sack must hold → the line, taking it: two herbs and a mushroom, a potion).
