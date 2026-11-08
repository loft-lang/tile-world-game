<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The game's story: a design to write before anything is built

The rough full game is a story the hero walks through, which in time takes in every puzzle idea
in some form (owner, 2026-11-04).  It is not about presenting everything at once.  Nothing of it
is built until four things are known: the story to tell, the people in it, the monsters and their
place in the world, and how the story can end (owner).  **Status** (owner, 2026-11-04): the outline
below is enough; the questions still open (the other endings, the other adventurers' aims) are now ones to answer by building the game.
Building keeps the story in sight (owner, 2026-11-04): the individual pieces have to be part of
the overall plot.  What is still open will be decided later, or may even remain a mystery in the
story: not everything needs spelling out (owner, 2026-11-04).

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

## Story parts

Every part of the story has a tag, the way loft's formal rules do, so what is built can say which
part of the plot it serves: a place file, a talk, a piece of code or a doc cites it as
`@ST-<Name>`, and the tag links to the part's paragraph here.  This index DEFINES the parts; each
part's text is the paragraph that carries its anchor.  A piece that cannot name the part it
serves is not built (the status above).  `loft tools/tags.loft check` keeps the index, the
paragraphs and the citations in step; `sites <tag>` lists where a part is cited.

Names are working names (owner, 2026-11-04): easy to change if need be, but with them the people
are easy to pinpoint and the game has something to work with.  The tags name the role, not the
person (`@ST-Wizard`, not his name), so a renamed person keeps every citation.

| Part | What | § |
|---|---|---|
| [`@ST-Setting`](#st-setting) | Orbury, the decrepit mining town, in Sunmarch, in the Kingdom of Aldmere | § 1 |
| [`@ST-Farm`](#st-farm) | Thornfield, the Dunns' farm, which cannot feed the village | § 1 |
| [`@ST-Mine`](#st-mine) | mined out at the surface, deeper sites left | § 1 |
| [`@ST-Artefact`](#st-artefact) | the artefact in pieces, crafted again with the lake's materials | § 1 |
| [`@ST-Heart`](#st-heart) | the heart of the amulet, left in the hero's dream on his first night: the key no one else had | § 1 |
| [`@ST-Force`](#st-force) | the force in the land: an underling mage of an opposing country, spreading rumours | § 1 |
| [`@ST-Decree`](#st-decree) | the decree: the lord fallen, a weak ownership dispute | § 1 |
| [`@ST-Guards`](#st-guards) | the King's Wardens: federal guards, no sides, no protection | § 1 |
| [`@ST-Future`](#st-future) | the village will be rebuilt anyway; the villagers may not live to see it | § 1 |
| [`@ST-Hero`](#st-hero) | Bram Holt, the sergeant who failed as an officer | § 1 |
| [`@ST-Play`](#st-play) | searching places, puzzles to progress | § 1 |
| [`@ST-Hints`](#st-hints) | villagers aid a stuck player; not a tutorial | § 1 |
| [`@ST-Stealth`](#st-stealth) | reaching places unnoticed; terrain knowledge | § 1 |
| [`@ST-Order`](#st-order) | no fixed order; places presented logically | § 1 |
| [`@ST-Outside`](#st-outside) | quests of the bigger world and the drought | § 1 |
| [`@ST-Villagers`](#st-villagers) | close, dependent, disagreeing on everything | § 2 |
| [`@ST-Newcomers`](#st-newcomers) | other adventurers, a mix of NPCs | § 2 |
| [`@ST-Magic`](#st-magic) | low magic: crafted, part of puzzles | § 2 |
| [`@ST-Alchemist`](#st-alchemist) | Hollis, potions and stimulants for the miners | § 2 |
| [`@ST-Foreman`](#st-foreman) | Garth Mabley, the retired foreman: the gist of the mine, not its latest state | § 2 |
| [`@ST-Blaster`](#st-blaster) | Wick, the blaster: for blowing up the mine; an outsider, few social skills; knows where bombs lie, teaches blasting | § 2 |
| [`@ST-Innkeeper`](#st-innkeeper) | Agnes, the innkeeper of an abandoned inn: for abandoning the town | § 2 |
| [`@ST-Warden`](#st-warden) | Sergeant Voss of the King's Wardens, in their own camp, keeping the people out | § 2 |
| [`@ST-Fountains`](#st-fountains) | strange fountains in the mines and hills; their liquid turns to mud | § 2 |
| [`@ST-YoungVillager`](#st-youngvillager) | Lark, who can show the player a hidden path into the hills | § 2 |
| [`@ST-Wizard`](#st-wizard) | Master Odran, a newcomer after the mines for his own goals; not easily swayed | § 2 |
| [`@ST-Rival`](#st-rival) | Sable, the rival adventurer: a contender who blocks walkways | § 2 |
| [`@ST-Aide`](#st-aide) | Tamsin, Lady Brigid's aide: her viewpoint; arrives after the hero | § 2 |
| [`@ST-Court`](#st-court) | Lord Edric of Sunmarch, the young local lord, rejected by Princess Isolde | § 2 |
| [`@ST-Lady`](#st-lady) | Lady Brigid of Greywood, the witch: elderly, far forest lands, after the mine | § 2 |
| [`@ST-Neighbour`](#st-neighbour) | Lord Garrick of Highcrag, elderly lord of the mountains next door; sent the wizard | § 2 |
| [`@ST-NoKilling`](#st-nokilling) | killing is never the goal; monsters retreat | § 3 |
| [`@ST-Residents`](#st-residents) | monsters living there; the cyclopes' prey | § 3 |
| [`@ST-Founder`](#st-founder) | the ghost of the miner who founded the mine, in the cemetery at night | § 3 |
| [`@ST-Hunter`](#st-hunter) | Rennick, the outcast returned with a grudge; befriendable, never the village's | § 3 |
| [`@ST-Cyclopes`](#st-cyclopes) | the Kharn, the tribe in the mine, planning to take the village | § 3 |
| [`@ST-Debate`](#st-debate) | blow up the mine or abandon the town | § 3 |
| [`@ST-Climax`](#st-climax) | the invasion; a well-defended village turns it back | § 4 |
| [`@ST-BestEnding`](#st-bestending) | the cyclopes leave | § 4 |
| [`@ST-Sunset`](#st-sunset) | the hero rides into the sunset in every ending | § 4 |

## 1. The story to tell

<a id="st-setting"></a>**The setting** (owner, 2026-11-04): the story centres on a decrepit, but not totally abandoned,
mining town, Orbury, in the region of Sunmarch, in the Kingdom of Aldmere, whose king is King
Theobald (names, owner 2026-11-04).  The last remaining villagers struggle to make something of their lives.  The lands
around it are more given to farming, though desert has been creeping in lately (owner,
2026-11-04; the drought, @ST-Outside).

In the village, ore stands for wealth, and its flow has dried up (owner, 2026-11-04).  There is no
money in the game itself for the hero: there is nothing to buy with it (owner, 2026-11-08).

<a id="st-farm"></a>**The farm: Thornfield** (owner, 2026-11-04): there is a small farm at the village, kept by the
Dunns; it cannot provide enough food for the villagers, small as their number now is.

<a id="st-mine"></a>**The mine** (owner, 2026-11-04): it is mined out at the surface, which made them dig deeper and
open new dig sites in the hills to reach the valuable ore.  They had to stop, because of
political circumstances from outside and a drought (below: a decree), so there are still
valuable things to find.  Deep in the mine there are magical crafting materials, unknown to most
(owner, 2026-11-04; the witch and the wizard are after them, @ST-Lady, @ST-Wizard).

<a id="st-artefact"></a>**The artefact** (owner, 2026-11-04; still to be thought out): it lies in the mine in pieces, and has
value only when crafted together again, with the help of magical materials from around the Still
Lake (@ST-Mine, @GM-Flotilla).  Given to Tamsin for Lady Brigid, or taken to the local lord, it
pushes the story (@ST-BestEnding).  For now (owner, 2026-11-04): the chests are how its pieces are
found, and the pieces are just a number the game shows, not things held or drawn one by one
(owner, 2026-11-04).  The pieces were put in the chests on purpose after the war, and the chests hidden, though
not well enough against the changes time brought (owner, 2026-11-04).  The hero crafts it
himself, after learning how from the alchemist (owner, 2026-11-04; @ST-Alchemist), at an abandoned
smithy that can be made to work again for it (owner, 2026-11-04).  It was shattered in an old demon war, to prevent a portal being opened to the place where
the demon lord is banished.  The artefact is a key, not the portal: the portal is a specific
location outside the direct area (owner, 2026-11-04).  This is how the game alludes to a second
game with a bigger scope; that scope is not brought into this one (owner, 2026-11-04).  That war
was long before the mine; their leader deemed the place
inaccessible to people.  A force in the land influences people to gather the pieces again
(@ST-Force).  The bombs stand for the explosives that made the mine possible in the first place:
the one thing the old leaders after the war could not prepare for (owner, 2026-11-04).  They are
found throughout the mine: wherever there was work to do and extensions to build, some are left
(owner).  Modern alchemy made them easy to fabricate (owner, 2026-11-04; @ST-Alchemist).  The
demon lord is dead by now, but is still a source of power for mages (owner, 2026-11-04;
@ST-Magic, @ST-Lady, @ST-Wizard).  Read again (owner, 2026-11-08): the heart is the first piece
(@ST-Heart), and getting the found pieces together correctly may be a mini puzzle of its own.

<a id="st-heart"></a>**The heart of the amulet** (owner, 2026-11-08): the chests have been there a long time, and they are
seen; they are hard to reach, but a miner with time on their hands would have ground through
that anyway.  So there has to be a narrative reason why the hero is unique.  On his first night in
town the witch-lord (@ST-Lady) invades his dream and leaves him a special item: the heart of the
amulet, which also counts as a key.  The dream itself is kept light on details: spoken words,
perhaps an image or two, to be used again in an ending or in some backstory.  She definitely had
to prepare: Tamsin (@ST-Aide) had already brought the heart to its place before.  The heart is the
first piece of the artefact (@ST-Artefact), and it is not one held in the hands.  Getting the
found pieces together correctly may be a mini puzzle.

<a id="st-force"></a>**The force in the land** (owner, 2026-11-04): an underling mage from an opposing country, seeking to
destabilise Aldmere, and thinking the demon lord is still alive (@ST-Artefact: he is dead).  The
mine was not planned by them, but when they heard of it they started the rumours of magical
riches there.  The player can find out about the mage, but only by getting the different people
involved together, peacefully, in the same place (owner, 2026-11-04).

<a id="st-decree"></a>**Why the village has stopped mining** (owner, 2026-11-04).  Mining is rough right now, but the
hardy villagers (some of them workers) would press on regardless of that.  What stops them is a
decree from outside: digging is made unlawful or worthless.  The local lord, who got a lot of his
income from this mine, has fallen from grace, and that resulted in federal action.  The mine is
not yet claimed by others: there is an ownership dispute, but the parties in it do not have a
strong enough claim to really pull their weight.  The drought is a factor, but not everything.
The closure was more or less forced (owner, 2026-11-08): what the miners used was left where it
was useful to them.
Two lords want the ownership, the witch (@ST-Lady) and the elder lord next door (@ST-Neighbour),
who has the wizard as an adviser (@ST-Wizard); and the favour of the local lord, the young lord
(@ST-Court), might be restored (owner, 2026-11-04).

<a id="st-guards"></a>**The guards: the King's Wardens** (owner, 2026-11-04) are part of the decree: they are from the federal government
and take no sides.  They will not protect the town either; the local lord is blocked here: he
cannot benefit from the mine, and the local miners are blocked from working it (owner).

<a id="st-future"></a>**The village's future** (owner, 2026-11-04): the village has a future in all scenarios; eventually it
will be rebuilt anyway.  But that is not a very helpful future for the current villagers, who might
not survive until then.  Any of the lords in the ownership dispute (@ST-Court, @ST-Lady,
@ST-Neighbour) can eventually drive the cyclopes away (owner, 2026-11-04).

Weighed and set aside, and why (owner): no blasting powder (it would block gameplay); no buyer
(it stops the drive of the people and the hero); the workers conscripted (too much the same thing
as the hero being a soldier).

<a id="st-hero"></a>**The hero: Bram Holt, Sergeant Holt** (owner, 2026-11-04): a pre-defined hero, not a one-size-fits-all hero that tries to
fit every gamer.  He is a veteran soldier, a sergeant, who tried to become an officer and failed
spectacularly at it.  Because of that he is getting out of the army, for now: not really sacked,
but certainly not welcome there at the moment.  For now he is just searching for a place to lay
low for a while; but this adventure is also his way to grow into the person he wants to become.
Part of why he comes here is that he heard this place may offer opportunities for someone like
him, with little official education.  The army he served in was the local lord's (@ST-Court,
owner 2026-11-04).  He does not know of the court trouble at the start: it is bound up with the
state of the town, and learned together with the player (owner, 2026-11-04; Agnes tells it,
@ST-Innkeeper).  Entering the village is a bit of a let-down for him: this is
very much not the place he wants to be: he arrives riding his horse (owner, 2026-11-04; the game's
own sheet, tile 24), and cannot find a place to stay at the inn, which is abandoned (@ST-Innkeeper).
He can stay in the barn at Thornfield, if he is willing to let the Dunns use his horse on the
farm until he needs it again (owner, 2026-11-04; @ST-Farm).
But it might become the place he needs: he needs this
place to learn, perhaps to fetch useful artifacts, and to decide what to pursue in his life
(owner, 2026-11-04; so he rides on at the end, @ST-Sunset).

<a id="st-play"></a>**What the player does** (owner, 2026-11-04): search the different interesting places and get
into puzzle situations to progress.  The village is the tutorial, though never presented that way:
it holds the world's main mechanics on a smaller scale, and less interconnected (owner,
2026-11-04; LOCATIONS.md § 1).

<a id="st-hints"></a>**Hints** (owner, 2026-11-04): the villagers sometimes act as hints for the puzzles, how to get
down to the deeper tunnels for example.  This is not a tutorial: it is a system to aid players
who get stuck on a puzzle.

<a id="st-stealth"></a>**Unnoticed by the cyclopes** (owner, 2026-11-04): some of the puzzles are about getting places
without the cyclopes noticing.  Otherwise places are hard to reach, and fighting them in the open
is a bad idea, because they call for aid.  So terrain knowledge from careful exploration is part
of the game.

<a id="st-order"></a>**No fixed order** (owner, 2026-11-04): there is no final order in which the player should do
things.  The world presents the different places in a logical order.

<a id="st-outside"></a>**The bigger world and the climate** (owner, 2026-11-04): there will certainly be interactions and
quests related to the bigger world (the political circumstances outside) and the climate (the
drought).  The outside contenders for the mine (@ST-Court, @ST-Lady, @ST-Neighbour) are not
presented in the game unless an ending scene is built for it (owner, 2026-11-04); the witch's
dream (@ST-Heart) is kept light: spoken words, an image or two (owner, 2026-11-08).

**The places so far** (gathered from the owner's outline, 2026-11-04; a place marked *reading* is
the outline's implication, not the owner's words):

- **The village** (@ST-Setting): the town, the small farm with the barn where the hero stays
  (@ST-Farm), the alchemist's home
  (@ST-Alchemist), the abandoned inn and Agnes's house nearby (@ST-Innkeeper), an abandoned smithy
  that can be made to work again (@ST-Artefact); a small cemetery
  near the village, where the founder's ghost appears at night (@ST-Founder); the family that fled is missed there (@ST-Debate).
- **The main mine, the Old Seam** (@ST-Mine): mined out at the surface; roughly three levels of
  tunnels below it, the flooded ones the deepest (owner, 2026-11-04; @ST-Hints names the way down);
  more natural caves than the lake's, which is the biggest (owner); the lower tunnels, flooded, running in several directions, some
  into natural caves (@GM-Flooded; no special name, owner), one of them leading towards an
  underground lake, the Still Lake, without joining it once the pump has lowered the water
  (@GM-Flotilla); a lift for carts that joins all the levels, though the lowest cannot be reached by
  it until the pump has run several times (owner, 2026-11-04; @GM-CartLift).  Some places are
  reached only with a cart to clear rubble out of the way, and a working lift to get the cart there
  (owner).
- **The new dig sites in the hills, the High Diggings** (@ST-Mine), with a main mine of their own,
  where Sable has his spot, with his goods, rope among them (owner, 2026-11-04; @ST-Rival), and a hidden path into the hills that a younger
  villager can show the player (@ST-YoungVillager), along a brook where crabs live (@ST-Residents).
  The hidden path leads to a cave-in spot, a way into the main mine without being spotted by the
  Wardens or the Kharn (owner, 2026-11-04; @ST-Stealth).  The cave-in entrance is not directly
  connected to the Kharn's main tunnels: there is a walk before reaching them (owner).  Others have found that entrance, but not
  the hidden path: it can also be reached over the hillside itself, which is more exposed, so only
  safe at night, and then with trouble finding the way (owner, 2026-11-04).
- **The cyclopes settled in the main mine** (owner, @ST-Cyclopes), and so block the normal way to
  the lift and the pump (@GM-CartLift, @GM-Flooded).  There are more ways into it, such as a
  caved-in tunnel, reached by the hidden path (@ST-Stealth, @ST-YoungVillager).  So the cyclopes are a big part of the main puzzle (owner), and
  that matters to the theme: they must not stay unknown to the player, only heard of from the
  villagers.
- **The hill-top, Stormcap,** with the lightning installation, where bigfoot lives (@GM-Flooded,
  @ST-Residents).  Near the top of the hill is the start of a mine, where bigfoot sleeps at night
  (owner, 2026-11-04).
- **The Wardens' small camp on the main path to the mine** (@ST-Warden, owner): they guard that
  path, the one the ore is carried out by; the mine's rails run along it, through the camp, all the
  way to the village (owner, 2026-11-04).  It did not stop the Kharn.  Everybody else can get into
  the mine too, but has found a way in of their own (owner, 2026-11-04).
- **The bigger world outside** (@ST-Outside), where the decree comes from (@ST-Decree) and the
  other adventurers arrive from (@ST-Newcomers).

For a small game this is more than enough (owner, 2026-11-04); a few more small places may come,
such as where the hunter settled (@ST-Hunter): a small clump of woodland on the hillside (owner,
2026-11-04).  Each place holds a set of puzzles: a place is not
one room (owner).  Plenty of places have natural puzzles (owner, 2026-11-04): houses that are
locked but still accessible, tunnels with enough debris, broken walkways; the rival's obstructions
add to them (@ST-Rival).

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
- **Tiny Battle and Tiny City are mostly out of scope** (2026-11-04): their tiles may be used, but
  the story need not include them all.  Tiny City (Kenney's, online, not in the repo) has one
  interesting tile, a street lamp.
- **Scripted is enough.**  Scripted versus simulated is irrelevant to the player, who sees a scene
  and appreciates that it is not always exactly the same: variety from alternatives drawn from
  seed and day, no unwatched simulation; habits change only from the player's choices or authored
  scripts (2026-11-03).

## 2. The people in it (NPCs)

<a id="st-villagers"></a>**The villagers together** (owner, 2026-11-04): the villagers of the town know each other well and
depend on each other, but they have wildly different opinions on just about everything.  They do
not agree with the hero plundering the remains of the village; the smithy is the exception, and
they applaud it being opened again (owner, 2026-11-04).  It is not the things taken they mind (a
rope, say): an outsider sneaking into houses is not done, so they get far less accommodating once
they learn of it (owner, 2026-11-04).  They are grumpy about snooping, but enough aid to the village
makes them look past it (owner, 2026-11-04).  Aid the player can give from the start: helping the
farm directly, and foraging food — meat and vegetables — for the villagers (owner, 2026-11-04);
the rest comes with the detailed locations (LOCATIONS.md).

<a id="st-newcomers"></a>**Other newcomers** (owner, 2026-11-04): the hero is not the only one coming here; there are other
adventurers beside him and the hunter.  This is a world with magic in it, which makes for a mix of
NPCs.

<a id="st-magic"></a>**Magic** (owner, 2026-11-04): it is a low-magic world: mages cannot do magic at will, but have to
craft a lot to make it happen.  It can certainly exist as part of a puzzle.

<a id="st-alchemist"></a>**The alchemist: Hollis** (owner, 2026-11-04): still living in the village.  Their job was to provide the
miners with potions for small injuries, and other stimulants.  Hollis stayed when others left,
with a lab that is not easily replaced (owner, 2026-11-08).  The alchemist is paid with
resources: slightly more than the potions need (owner, 2026-11-08).

<a id="st-foreman"></a>**The old foreman: Garth Mabley** (owner, 2026-11-04): the retired foreman; the active one left.  He
knows the gist of the mine, but not its latest state (@ST-Mine, @ST-Hints).

<a id="st-blaster"></a>**The blaster: Wick** (owner, 2026-11-04): knows explosives, and is sure the mine can be blown up
with the cyclopes in it (@ST-Debate).  He was already a bit of an outsider among the miners:
keeping to himself, with few social skills.  He can tell the hero where bombs lie in the mines,
and teach him blasting (owner, 2026-11-05): that a small dyke of stones pushed up to a bomb
directs its explosion one way, instead of the general push to all sides.  Not a skill the hero gains: a rule of the
world anyone can find, but not obvious to gamers; Wick is where it can be learnt by being told
(owner, 2026-11-05; DESIGN.md § Bombs, @ST-Hints).  **His plan should back-fire spectacularly,
and that is a good thing** (owner, 2026-11-07): not by a special rule or a script, but just by how
the rules already interact (owner).

<a id="st-innkeeper"></a>**The innkeeper: Agnes** (owner, 2026-11-04): has seen too many leave, and is for abandoning the
town (@ST-Debate).  The inn itself is abandoned; she lives in a house nearby that was easier to
maintain.  She carries the state of the town and the court trouble behind it: the hero, and the
player with him, learns them from her (owner, 2026-11-04).

<a id="st-warden"></a>**The Wardens' sergeant: Sergeant Voss** (owner, 2026-11-04): a fellow sergeant for the hero to
measure himself against (@ST-Guards).  The Wardens keep to themselves in a small camp, explicitly
not mingling with the people: they are there to keep the people out.  They guard the main path to
the mine; that is enough to stop it running, because the ore is carried out that way too.  It did
not stop the Kharn (owner, 2026-11-04).  Part of the court trouble is known to him, and the hero
can talk to him about it (owner, 2026-11-04).

<a id="st-fountains"></a>**The strange fountains** (owner, 2026-11-04): found in the mines and the hills, holding a strange
liquid.  The alchemist talks about them: he cannot take the liquid from them, because it turns to
mud, and he cannot find what makes the water special, or any use for it.  They are the game's save
points (owner, 2026-11-04; DESIGN.md § Guards, doors and wells: drinking saves the whole situation),
explained in the world as a mystery nobody has solved.  Tiny Dungeon has them already: wall
fountains with a green liquid (tiles 8, 20, 32, 44), and dry ones beside them (7, 19, 31, 43).

<a id="st-youngvillager"></a>**A younger villager: Lark** (owner, 2026-11-04): of the Dunns of Thornfield (@ST-Farm); can be
asked to show the player a hidden path into the hills (@ST-Hints, @ST-Stealth).

<a id="st-wizard"></a>**The wizard: Master Odran** (owner, 2026-11-04): not a villager; one of the newcomers, interested
in the mines for his own goals: he is an adviser to one of the two lords who want the mine, the
elder lord next door (@ST-Neighbour), but certainly has goals of his own too.  He has the energy for a lightning strike, which the alchemist
cannot produce himself and the pump's crystal needs (DESIGN.md @GM-Flooded), but is not easily
swayed to chime in.  He certainly does not approve of a lord with magical knowledge who shields
that knowledge from his own lord (owner, 2026-11-04): the witch (@ST-Lady).  He is not one of
her advisers (owner, 2026-11-04).  His goals in the mine are the same as hers: the magical
crafting materials deep in it, unknown to most (@ST-Mine).  He was sent by the elder lord next door
(@ST-Neighbour, owner 2026-11-04).

<a id="st-rival"></a>**The rival adventurer: Sable** (owner, 2026-11-04): another newcomer (@ST-Newcomers), after the same
artifacts as the hero; he turns up in places as a contender.  He actively blocks walkways, so some
places hold puzzles solely because of the obstructions he made.  What does not occur naturally,
a runaway cart for one, he can have left behind as practical traps (owner, 2026-11-04).  His spot,
with his goods and rope among them, is in the High Diggings' mine; during the day he is often in
the main mine, and cannot get back home until dusk (owner, 2026-11-04).

<a id="st-aide"></a>**Lady Brigid's aide: Tamsin** (owner, 2026-11-04): one of the other adventurers, working for the
witch (@ST-Lady) and looking for the magical materials in the mine (@ST-Mine); gives her viewpoint,
so the wizard does not carry both lords (owner).  Unlike Sable, who is there before the hero,
Tamsin arrives later (owner, 2026-11-04), though Tamsin had been here before, unseen, to bring the
heart to its place for the hero's first night (owner, 2026-11-08; @ST-Heart).  Tamsin is gender-neutral, for the fun of it (owner): the
lines about Tamsin never say he or she.  Tiny Dungeon's tile 88 proposed.

<a id="st-court"></a>**Court drama** (owner, 2026-11-04): the young lord, Lord Edric of Sunmarch, proposed to a princess,
Princess Isolde, and was rejected.
The young lord is the local lord, the one who fell from grace (@ST-Decree), and the hero was in his
army (@ST-Hero).  He has a claim on the mine, but it is actively blocked (owner, 2026-11-04; the
guards block him, @ST-Guards).  The princess is the reason he fell from grace (owner, 2026-11-04).
The hero can influence this, indirectly: by giving the young lord unique treasures, and ways to show
a bit of spine (owner, 2026-11-04).  They reach him when the hero rides off to him at the end, one
of the directions he can take (@ST-Sunset, owner 2026-11-04).

<a id="st-lady"></a>**The lady: Lady Brigid of Greywood** (owner, 2026-11-04): another one interested in the mine is an older lady with quite
some magical skills, and aides.  That is not forbidden, but it is not generally approved of.  She
is the witch, an elder lord, and she has the magical knowledge (owner, 2026-11-04).  She is not the
lord of the mountains next door; both are elderly, but her lands lie further away and are mostly
forest (owner, 2026-11-04).  She is a party
in the ownership dispute (@ST-Decree), and her interest is the magical crafting materials deep in
the mine, unknown to most: the same as the wizard's (owner, 2026-11-04).

<a id="st-neighbour"></a>**The elder lord next door: Lord Garrick of Highcrag** (owner, 2026-11-04): the last of the lords, an elder male lord, and the
one who sent the wizard (@ST-Wizard).  His interest in the mine, which is adjacent to his lands, is
mostly just monetary.  His lands are mostly mountains, with little central control.  He is not
aware of the wizard's personal interest, but was pleasantly surprised by how willing the grumpy
mage was to go there (owner, 2026-11-04).  He is a party in the ownership dispute (@ST-Decree) with a
good claim on paper: only a small change of their borders would make him the owner.  In practice it
is weak, because he and his soldiers cannot travel there easily (owner, 2026-11-04).

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

<a id="st-nokilling"></a>**Killing is never the goal** (owner, 2026-11-04): in this world, killing everything that moves
(monsters included) is never the goal; that is why monsters often retreat when injured.  It is not
a strict rule: the player may have reasons to hunt them anyway, for resources or for future
safety, but that is never the presented goal of an area.

<a id="st-residents"></a>**The other monsters** (owner, 2026-11-04): most of them are just living there.  The cyclopes hunt
them too, for food, but that is hardly enough to sustain their tribe.  Green slimes live in the
natural caves beside the Still Lake (owner, 2026-11-04; Tiny Dungeon's tile 108).  Crabs live along
a brook beside the hidden path into the hills (owner, 2026-11-04; tile 110).  There can be several
kinds of rat (the brown 123, the grey 124), but only if each finds a way of behaving of its own
(owner, 2026-11-04).  Bigfoot lives on Stormcap, the storm hill (owner, 2026-11-04; Tiny Ski's
tiles 78–80); unique ways to deal with him are defined when that terrain is built (owner).

<a id="st-founder"></a>**The founder's ghost** (owner, 2026-11-04): at night a local, non-aggressive ghost haunts the small
cemetery near the village: the original miner who founded the mine (@ST-Mine; Tiny Dungeon's
tile 121).  With the help of others he dug the first main shaft, but drowned in the lake when
they struck its water; his fellow miners retrieved his body (owner, 2026-11-04; the Still Lake,
@GM-Flotilla).

<a id="st-hunter"></a>**The hunter: Rennick** (owner, 2026-11-04): someone the villagers know, and dislike already.  He was cast
out of the village in his youth, and has now returned to it.  He has a grudge against the village;
he is not evil (owner).  He might be befriended, but he will
never take the side of the village.  He already knows that the village's main income has dried up
for now: nobody is working the mine.

<a id="st-cyclopes"></a>**The cyclopes: the Kharn** (owner, 2026-11-04): a tribe that recently settled in a mine (the Old
Seam, the main mine, @ST-Setting).  The villagers
have met them only a few times, and fled, using their knowledge of the terrain to get home.  The
group plans to take over the village eventually, but for now the cyclopes are content in the mine.
They have their own language (owner, 2026-11-04).  The one who knows some broken words of the
hero's, and is difficult to communicate with, is the chief's second-in-command, Urgha, not the
chief himself: better for the tension (owner, 2026-11-04).

<a id="st-debate"></a>**The village's debate over them** (owner, 2026-11-04): the cyclopes are already a subject of
debate among the villagers.  Some want to blow up the mine with the cyclopes in it (not easy to
do); some want to just abandon the town.  The debate is not settled.  Only a single family has
fled because of it, the Fenns; they are missed, and might even return when the threat wanes.

*Further outline to come; details follow from what the player is to do.*

What the owner has already said about monsters (DESIGN.md § Enemies):

- Enemies do not fight to the death: hurt, they go home to heal; a bested one leaves only what it
  visibly held.
- Monsters roam larger territories, in bands between points of interest the developer picks
  (2026-11-03, WORLD.md).
- Cyclopes make darkness a tool: hard to fight, in groups, passed by stealth, so a torch has a cost
  too (2026-10-30).
- The hunter shoots as the hero does, hunts other monsters, prefers the hero.

## 4. How the story can end

<a id="st-climax"></a>**The climax** (owner, 2026-11-04): the cyclops situation always comes to a climax: eventually they
invade the village, and the hunter is not helping there.  The cyclopes know other places, but
because of the shared situation the village is their best choice for the moment.  The cyclopes
are not cold killers, they are tribal: presented with a well-defended village, they retreat.
**When** (owner, 2026-11-04): the attack takes place when the cyclopes have hunted the monsters
there are to hunt, and run out of food (@ST-Residents).  Between the start and the attack there is
a phase where the cyclopes get nervous and start to scout outside the mine (owner, 2026-11-04):
the clock, read from the world.
**Well defended** is enough capable people together behind a barricade that has been built, of
straight beams as for the flotilla, at the village entrance (owner, 2026-11-04).  The town can
build its barricade itself; it takes time, and an axe (owner, 2026-11-04).  When the Wardens
understand the danger they are in, and the right thing to do, the hands for the barricade more
than double (owner, 2026-11-04; @ST-Warden).  With little aid given, the hero is needed to defend
the town himself, and probably can only with enough potions lying around, whose ingredients he has
to gather (owner, 2026-11-04; the alchemist brews them, @ST-Alchemist); so the
Wardens have to move too, for they are in danger as well (owner, 2026-11-04; @ST-Warden).

<a id="st-bestending"></a>**The best ending** (owner, 2026-11-04): getting the cyclopes to leave.  Blowing up the mine is not
it: a cave-in might kill some of them, but the group as a whole would be able to dig itself out
and start an offensive.  A good ending also resolves the miners' block: they can work the mine
again (owner, 2026-11-04).  So the mine's ownership has three scenarios: one of the two lords who
want it gets it, or the local lord's favour is restored (owner).  One way to push the story: an
artefact given to Tamsin (@ST-Aide), for Lady Brigid, instead of taken to the local lord (owner,
2026-11-04).

<a id="st-sunset"></a>**The hero's ending** (owner, 2026-11-04): the hero rides into the sunset in any scenario, just leaving
a different world behind.  Riding off to the young lord is one of the directions he can take
(@ST-Court, owner 2026-11-04).  Perhaps there is an ending where he settles in the village, but that
would be quite a special, hidden ending (owner, 2026-11-04).  The ending, too, is the player
walking away: then the game gives the ending, which can hold conclusions the player did not know
before (owner, 2026-11-04).  So the player may aid one of the contenders purely by mistake: the
game does not make it unclear on purpose, it just does not hand the pieces over on a plate
(owner, 2026-11-04).  Nothing is withheld: it is only not delivered in a cutscene.  The player can
talk to everybody, and can aid anybody (owner, 2026-11-04).  After the game, probably a couple of
stills: scenes of what follows, as it reaches the hero's ears (owner, 2026-11-04).

*Further outline to come.*

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

- **Talk has a first shared memory** (BUILD.md C2b): facts, known world-wide and carried through
  doors; a talk can start at a known fact; a doing can set one (going into a home while seen sets
  `snooped`).  Gossip, a reputation, an event the village reacts to still need lines that set
  facts, and lines and barks that depend on them past the start.
- **Barks rotate in order.**  A world that should feel lived-in may want them chosen by what the
  hero carries or how hurt it is, by a recent blast, by the time of day, drawn from seed and day.
- **Overheard scenes.**  Two people talking between themselves while the hero only passes would
  need a talk that runs at listening range, apart from the hero's own talk that starts beside.
- **Routines come first for "living".**  People moving through their day (WORLD.md step 3: points,
  activities, the clock) carries more life than any talk; talk then follows the activity.  A
  first piece exists (BUILD.md C3b): a person's points by the place's clock; walking between
  places and one clock across them do not.
- **Places are the cheap part.**  A new place is a file and a door; a story with many small places
  costs little engine work.  What costs more: things that follow the hero through a door, one clock
  across places, saves (WORLD.md steps 2 and 5).
- **Outcomes are state.**  Endings that differ by what the player did need that state saved and
  read (facts, as above); endings by place reached need nothing new.
- **Wick's plan backfires by the rules, with no script** (DESIGN.md @GM-Combat, *Enemies by
  templates*; TEMPLATES.md § The area's unease, proposed): a blast raises its area two steps and its
  neighbours one, at once, and draws the Kharn to it.  So blowing up the mine with the Kharn in it
  alarms every hall it is heard in, and brings the tribe out on guard instead of burying it.  How
  spectacular, and what it sets going (the raid on the village coming early, @ST-Climax?), is the
  story's to say, within the owner's rule: nothing is added for it — what follows is what the
  general rules do with a big blast.  The engine side needs only what is general: the unease
  spreading across places, and a tribe that acts on its area's step (BUILD.md C4, still open: a
  tribe across places).  A test can hold it: Wick's blast, played, ends with the Kharn alarmed and
  out, from the rules alone.
- **The heart asks little that is new** (@ST-Heart).  A chest that opens only for the one who
  carries the heart is a chest with a condition, as a door held shut is; a thing carried that
  is not held is kept apart from the one held item, as the artefact's pieces already are (a
  count).  The
  first night is an event at a moment, as the climax is (below), and needs the hero to sleep
  (DESIGN.md: sleep is planned, not built).  The dream's words and images can be stills in the
  epilogue's form (`levels/epilogue.json`: a place, a cell, what is said, the facts it needs),
  so an ending can show one of them again.  The mini puzzle of the pieces needs the pieces as
  things, where today they are a number (@ST-Artefact): things the hero's own moves can put in
  place (Q puts down), as every other puzzle is worked.
- **The climax is a world event.**  The cyclopes invading, and retreating from a well-defended
  village, asks for: an event that comes at a moment or on a condition; a measure of "well
  defended" read at that moment — known now (§ 4): capable people gathered behind a built
  barricade, which existing rules can make of big things pushed into place (rocks, barrels,
  carts), the people brought there by their points (WORLD.md step 3); and the group marching from the
  mine to the village through doors (monster bands, WORLD.md step 3, and things following through
  a door).  The cyclopes' rules today are per place and per figure; a group decision to retreat is
  new.
- **The attack's moment follows the Kharn's prey** (§ 4).  With no unwatched simulation (scripted
  is enough), the prey running out can be a count read from the clock and from what has happened
  to the monsters: those the player, the hunter (who hunts them too) or a blast takes off it bring
  the attack nearer; those kept alive put it off.
- **Wick's blasting is a rule, not a skill** (owner, 2026-11-05): the directed blast holds for
  every player, so it needs no fact or unlock — only the rule in the blast (DESIGN.md § Bombs) and
  Wick's lines explaining it.  **Where bombs lie**, told, is a hint (@ST-Hints): lines naming the
  places, or marks on the map when there is one.
- **Hints for a stuck player** need to know the player is stuck (time spent in a place, tries,
  returns) and lines that depend on it: facts and conditions again, the same mechanism as gossip.
- **Relations that last** (the hunter befriended, a family returning, the village's mood) are facts
  too; one mechanism serves gossip, hints, relations and endings.
- **No fixed order** keeps places gated by the world (a puzzle, a tool, a way down) rather than by
  story flags where it can; the editor's checks (K, the way to a place) can then verify that every
  order still reaches the climax.
