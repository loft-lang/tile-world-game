<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The game's story: a design to write before anything is built

The rough full game is a story the hero walks through, which in time takes in every puzzle idea
in some form (owner, 2026-11-04).  It is not about presenting everything at once.  Nothing of it
is built until four things are known: the story to tell, the people in it, the monsters and their
place in the world, and how the story can end (owner).  **Status** (owner, 2026-11-04): the outline
below is enough; the questions still open (what "well defended" is made of, the other endings,
the other adventurers' aims) are now ones to answer by building the game.
Building keeps the story in sight (owner, 2026-11-04): the individual pieces have to be part of
the overall plot.

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

| Part | What | § |
|---|---|---|
| [`@ST-Setting`](#st-setting) | the decrepit mining town | § 1 |
| [`@ST-Farm`](#st-farm) | the farm that cannot feed the village | § 1 |
| [`@ST-Mine`](#st-mine) | mined out at the surface, deeper sites left | § 1 |
| [`@ST-Decree`](#st-decree) | the decree: the lord fallen, a weak ownership dispute | § 1 |
| [`@ST-Guards`](#st-guards) | federal guards: no sides, no protection | § 1 |
| [`@ST-Future`](#st-future) | the village will be rebuilt anyway; the villagers may not live to see it | § 1 |
| [`@ST-Hero`](#st-hero) | the sergeant who failed as an officer | § 1 |
| [`@ST-Play`](#st-play) | searching places, puzzles to progress | § 1 |
| [`@ST-Hints`](#st-hints) | villagers aid a stuck player; not a tutorial | § 1 |
| [`@ST-Stealth`](#st-stealth) | reaching places unnoticed; terrain knowledge | § 1 |
| [`@ST-Order`](#st-order) | no fixed order; places presented logically | § 1 |
| [`@ST-Outside`](#st-outside) | quests of the bigger world and the drought | § 1 |
| [`@ST-Villagers`](#st-villagers) | close, dependent, disagreeing on everything | § 2 |
| [`@ST-Newcomers`](#st-newcomers) | other adventurers, a mix of NPCs | § 2 |
| [`@ST-Magic`](#st-magic) | low magic: crafted, part of puzzles | § 2 |
| [`@ST-Alchemist`](#st-alchemist) | potions and stimulants for the miners | § 2 |
| [`@ST-Wizard`](#st-wizard) | a newcomer after the mines for his own goals; not easily swayed | § 2 |
| [`@ST-Court`](#st-court) | the young local lord's proposal to a princess, rejected | § 2 |
| [`@ST-Lady`](#st-lady) | the witch, an elder lord after the mine; magic and aides, not approved of | § 2 |
| [`@ST-Neighbour`](#st-neighbour) | the elder lord next to the mine who sent the wizard; money | § 2 |
| [`@ST-NoKilling`](#st-nokilling) | killing is never the goal; monsters retreat | § 3 |
| [`@ST-Residents`](#st-residents) | monsters living there; the cyclopes' prey | § 3 |
| [`@ST-Hunter`](#st-hunter) | the outcast returned; befriendable, never the village's | § 3 |
| [`@ST-Cyclopes`](#st-cyclopes) | the tribe in the mine, planning to take the village | § 3 |
| [`@ST-Debate`](#st-debate) | blow up the mine or abandon the town | § 3 |
| [`@ST-Climax`](#st-climax) | the invasion; a well-defended village turns it back | § 4 |
| [`@ST-BestEnding`](#st-bestending) | the cyclopes leave | § 4 |
| [`@ST-Sunset`](#st-sunset) | the hero rides into the sunset in every ending | § 4 |

## 1. The story to tell

<a id="st-setting"></a>**The setting** (owner, 2026-11-04): the story centres on a decrepit, but not totally abandoned,
mining town.  The last remaining villagers struggle to make something of their lives.

<a id="st-farm"></a>**The farm** (owner, 2026-11-04): there is a small farm at the village; it cannot provide enough
food for the villagers, small as their number now is.

<a id="st-mine"></a>**The mine** (owner, 2026-11-04): it is mined out at the surface, which made them dig deeper and
open new dig sites in the hills to reach the valuable ore.  They had to stop, because of
political circumstances from outside and a drought (below: a decree), so there are still
valuable things to find.  Deep in the mine there are magical crafting materials, unknown to most
(owner, 2026-11-04; the witch and the wizard are after them, @ST-Lady, @ST-Wizard).

<a id="st-decree"></a>**Why the village has stopped mining** (owner, 2026-11-04).  Mining is rough right now, but the
hardy villagers (some of them workers) would press on regardless of that.  What stops them is a
decree from outside: digging is made unlawful or worthless.  The local lord, who got a lot of their
income from this mine, has fallen from grace, and that resulted in federal action.  The mine is
not yet claimed by others: there is an ownership dispute, but the parties in it do not have a
strong enough claim to really pull their weight.  The drought is a factor, but not everything.

<a id="st-guards"></a>**The guards** (owner, 2026-11-04) are part of the decree: they are from the federal government
and take no sides.  They will not protect the town either; the local lord is blocked here.

<a id="st-future"></a>**The village's future** (owner, 2026-11-04): the village has a future in all scenarios; eventually it
will be rebuilt anyway.  But that is not a very helpful future for the current villagers, who might
not survive until then.  Any of the lords in the ownership dispute (@ST-Court, @ST-Lady,
@ST-Neighbour) can eventually drive the cyclopes away (owner, 2026-11-04).

Weighed and set aside, and why (owner): no blasting powder (it would block gameplay); no buyer
(it stops the drive of the people and the hero); the workers conscripted (too much the same thing
as the hero being a soldier).

<a id="st-hero"></a>**The hero** (owner, 2026-11-04): a pre-defined hero, not a one-size-fits-all hero that tries to
fit every gamer.  He is a veteran soldier, a sergeant, who tried to become an officer and failed
spectacularly at it.  Because of that he is getting out of the army, for now: not really sacked,
but certainly not welcome there at the moment.  For now he is just searching for a place to lay
low for a while; but this adventure is also his way to grow into the person he wants to become.
Part of why he comes here is that he heard this place may offer opportunities for someone like
him, with little official education.  The army he served in was the local lord's (@ST-Court,
owner 2026-11-04).  Entering the village is a bit of a let-down for him: this is
very much not the place he wants to be.  But it might become the place he needs.

<a id="st-play"></a>**What the player does** (owner, 2026-11-04): search the different interesting places and get
into puzzle situations to progress.

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
presented in the game unless an ending scene is built for it (owner, 2026-11-04).

**The places so far** (gathered from the owner's outline, 2026-11-04; a place marked *reading* is
the outline's implication, not the owner's words):

- **The village** (@ST-Setting): the town, the small farm (@ST-Farm), the alchemist's home
  (@ST-Alchemist); the family that fled is missed there (@ST-Debate).
- **The mine** (@ST-Mine): mined out at the surface; deeper tunnels below (@ST-Hints names the
  way down); its single flooded part, running in several directions, some into natural caves
  (@GM-Flooded); an underground lake (@GM-Flotilla); a lift for carts (@GM-CartLift).
- **The new dig sites in the hills** (@ST-Mine).
- **The cyclopes settled in the main mine** (owner, @ST-Cyclopes), and so block the normal way to
  the lift and the pump (@GM-CartLift, @GM-Flooded).  There are more ways into it, such as a
  caved-in tunnel (@ST-Stealth).  So the cyclopes are a big part of the main puzzle (owner), and
  that matters to the theme: they must not stay unknown to the player, only heard of from the
  villagers.
- **The hill-top** with the lightning installation and a local monster (@GM-Flooded).
- *Reading*: where the federal guards stand (@ST-Guards), at the mine or on the way to it.
- **The bigger world outside** (@ST-Outside), where the decree comes from (@ST-Decree) and the
  other adventurers arrive from (@ST-Newcomers).

For a small game this is more than enough (owner, 2026-11-04); a few more small places may come,
such as where the hunter settled (@ST-Hunter).  Each place holds a set of puzzles: a place is not
one room (owner).

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
depend on each other, but they have wildly different opinions on just about everything.

<a id="st-newcomers"></a>**Other newcomers** (owner, 2026-11-04): the hero is not the only one coming here; there are other
adventurers beside him and the hunter.  This is a world with magic in it, which makes for a mix of
NPCs.

<a id="st-magic"></a>**Magic** (owner, 2026-11-04): it is a low-magic world: mages cannot do magic at will, but have to
craft a lot to make it happen.  It can certainly exist as part of a puzzle.

<a id="st-alchemist"></a>**The alchemist** (owner, 2026-11-04): still living in the village.  Their job was to provide the
miners with potions for small injuries, and other stimulants.

<a id="st-wizard"></a>**The wizard** (owner, 2026-11-04): not a villager; one of the newcomers, interested
in the mines for his own goals.  He has the energy for a lightning strike, which the alchemist
cannot produce himself and the pump's crystal needs (DESIGN.md @GM-Flooded), but is not easily
swayed to chime in.  He certainly does not approve of a lord with magical knowledge who shields
that knowledge from his own lord (owner, 2026-11-04): the witch (@ST-Lady).  He is not one of
her advisers (owner, 2026-11-04).  His goals in the mine are the same as hers: the magical
crafting materials deep in it, unknown to most (@ST-Mine).  He was sent by the elder lord next door
(@ST-Neighbour, owner 2026-11-04).

<a id="st-court"></a>**Court drama** (owner, 2026-11-04): the young lord proposed to a princess and was rejected.
The young lord is the local lord, the one who fell from grace (@ST-Decree), and the hero was in his
army (@ST-Hero).  He has a claim on the mine, but it is actively blocked (owner, 2026-11-04; the
guards block him, @ST-Guards).  The princess is the reason he fell from grace (owner, 2026-11-04).
The hero can influence this, indirectly: by giving the young lord unique treasures, and ways to show
a bit of spine (owner, 2026-11-04).  They reach him when the hero rides off to him at the end, one
of the directions he can take (@ST-Sunset, owner 2026-11-04).

<a id="st-lady"></a>**The lady** (owner, 2026-11-04): another one interested in the mine is an older lady with quite
some magical skills, and aides.  That is not forbidden, but it is not generally approved of.  She
is the witch, an elder lord, and she has the magical knowledge (owner, 2026-11-04).  She is a party
in the ownership dispute (@ST-Decree), and her interest is the magical crafting materials deep in
the mine, unknown to most: the same as the wizard's (owner, 2026-11-04).

<a id="st-neighbour"></a>**The elder lord next door** (owner, 2026-11-04): the last of the lords, an elder male lord, and the
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
them too, for food, but that is hardly enough to sustain their tribe.

<a id="st-hunter"></a>**The hunter** (owner, 2026-11-04): someone the villagers know, and dislike already.  He was cast
out of the village in his youth, and has now returned to it.  He has a grudge against the village;
he is not evil (owner).  He might be befriended, but he will
never take the side of the village.  He already knows that the village's main income has dried up
for now: nobody is working the mine.

<a id="st-cyclopes"></a>**The cyclopes** (owner, 2026-11-04): a tribe that recently settled in a mine.  The villagers
have met them only a few times, and fled, using their knowledge of the terrain to get home.  The
group plans to take over the village eventually, but for now the cyclopes are content in the mine.

<a id="st-debate"></a>**The village's debate over them** (owner, 2026-11-04): the cyclopes are already a subject of
debate among the villagers.  Some want to blow up the mine with the cyclopes in it (not easy to
do); some want to just abandon the town.  The debate is not settled.  Only a single family has
fled because of it; they are missed, and might even return when the threat wanes.

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

<a id="st-bestending"></a>**The best ending** (owner, 2026-11-04): getting the cyclopes to leave.  Blowing up the mine is not
it: a cave-in might kill some of them, but the group as a whole would be able to dig itself out
and start an offensive.

<a id="st-sunset"></a>**The hero's ending** (owner, 2026-11-04): the hero rides into the sunset in any scenario, just leaving
a different world behind.  Riding off to the young lord is one of the directions he can take
(@ST-Court, owner 2026-11-04).

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
- **The climax is a world event.**  The cyclopes invading, and retreating from a well-defended
  village, asks for: an event that comes at a moment or on a condition; a measure of "well
  defended" read at that moment from what the player has done; and the group marching from the
  mine to the village through doors (monster bands, WORLD.md step 3, and things following through
  a door).  The cyclopes' rules today are per place and per figure; a group decision to retreat is
  new.
- **Hints for a stuck player** need to know the player is stuck (time spent in a place, tries,
  returns) and lines that depend on it: facts and conditions again, the same mechanism as gossip.
- **Relations that last** (the hunter befriended, a family returning, the village's mood) are facts
  too; one mechanism serves gossip, hints, relations and endings.
- **No fixed order** keeps places gated by the world (a puzzle, a tool, a way down) rather than by
  story flags where it can; the editor's checks (K, the way to a place) can then verify that every
  order still reaches the climax.
