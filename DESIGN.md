<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# tile_world — the game design

This is what the game built on tile_world should be.
- What is shipped is in [README.md](README.md).
- This file is the target: every rule here is either built (with a test) or open.
- Each rule names the slice that builds it.

## The principle: a dungeon is a puzzle, not a grind

- **Puzzles come first.** A dungeon should feel like one big puzzle. Nobody should get through it by
  grinding until the character is strong enough to trivialise it.
- **Fun, never lost** (owner, 2026-11-07).  The game is to be fun, never one where the player
  feels lost for forgetting a key.  Every key is a help, never a requirement: what a player does
  without it still works, only less well.  Switching the focus between enemies is one of the
  things that most easily leaves a player lost, so the focus mostly takes care of itself
  (@GM-Combat, *Locked on*).
- **Not a combat simulator** (owner, 2026-11-07).  An RPG is not a combat simulator: one way of
  acting serves fighting, gathering food, building, talking and trading.  Here that way is reading
  a figure's manner and answering it in time, with the same few keys, by the same templates
  (@GM-Combat, *Enemies by templates*; TEMPLATES.md): a villager's manner tells when to press as
  an enemy's tell tells when to lunge.  The same goals as Moros's system, which is not used here.
- **Puzzles from parts** (owner, 2026-11-08).  A scene's puzzle is made of parts, each a small
  challenge in itself and interchangeable with another like it; the complexity lies in how the
  parts chosen interact.  A seed picks the parts, and a check over many seeds finds the ones
  whose parts together are solvable.  **The map is fixed; the parts live under it** (owner): every
  place has its layout drawn by hand, so things can be made believable and readable, verified
  through gameplay; the templates deal the state and the insides of what is there.  Designed in
  PUZZLES.md (proposed, not built).
- **Growth comes later.** Character growth comes only after a base dungeon and world feel alive and
  are satisfying to beat. Growth means carried weapons, worn armour and stat increases.  Read again
  (owner, 2026-11-07): growth is breadth, not height: a new thing to carry, wear or do, each with
  its trade (the knife, the shield, armour), never a bigger number; the hero's health and its
  temporary health stay as they are.
- **Growth is never necessary.** Correct puzzle solving and fighting strategy must always let the
  base character beat every dungeon. It may get increasingly harder without upgrades.
- **Nothing is tutorialized** (owner, 2026-10-30).  A dungeon is presented in its natural state;
  no level teaches a rule, no hint explains one.  The player learns the world as in Spelunky: by
  watching it and by dying in it.  So every rule must hold everywhere, the same way every time —
  consistency is the teacher — and what is about to happen must be readable from the world itself
  (a sound, a sight), never from a message.  Villagers' hints are not a tutorial (owner,
  2026-11-04): they are a system to aid a player who is stuck on a puzzle (STORY.md § 1).
- **Hurt, rarely killed** (owner, 2026-10-30).  Because nothing is taught, the dungeon does not set
  up situations that easily kill: the hero gets hit, and then has to search for potions.  Damage
  is a cost to recover from, not an ending.  Proposed: a red health potion (Kenney's dungeon:115,
  beside the green one) restores 3 health up to the start's 6 when walked over, found in the
  dungeon like chests.  A deep fall stays deadly — it is seen and avoided (owner) — but big
  heights do not come from the start: a dungeon's first drops are shallow, deeper ones further in.
  Special potions, and later stat growth, give the hero temporary hitpoints (owner): health above
  the start's 6, to survive more hazardous situations — never needed (growth is never necessary).
  They are lost first when the hero is hit, and do not fade with time (owner).  Later, sleep and a
  day-night cycle clear them, and lingering ailments with them (owner, planned).  **Built**
  (`tests/potions.loft`): the red health potion (Kenney's 115) restores 3 up to the start's 6; the
  blue spirit potion (116) gives 2 temporary health — every hurt (a bite, a blast, a fall, a cart)
  takes it first (`rules::harm`), and it never fades.  The status line shows `6+2`.
  **A potion is carried, and heals over time** (owner, 2026-11-04): walked over empty-handed it
  is picked up, to be put down (L) where a fight may come or drunk later (J); walked over with the
  hands full it is drunk (walking over no longer takes an item, owner 2026-11-07: Q does; potions in
  a fight are decided later).  Never an instant effect on a fight: a drunk potion's points come one
  every 1500 ms (proposed, `rules::POTION_MS`) — a health potion's 3 over 4.5 s, never above 6, a
  spirit potion's 2 the same way; a second potion adds its points, not its speed, and healing
  still to come goes along through a door.  The green strength potion is carried the same way but
  is strength, not healing: it acts at once.  Tried in the test world: a health potion by the
  cyclops den's door (`tests/fights.loft`, `checks/game.loft` with the keys).
  A blast pushes the hero over a ledge, and the fall's cost adds to the blast's (owner): a rule
  that stays, and one the level designer keeps in mind — a bomb, or a cart at speed, beside a
  ledge is a level's choice to make deadly or not.
- **Real time** (owner, 2026-10-30).  The world runs on its clock whatever the hero does: carts
  roll, blasts go off, torches burn while the player thinks.  It stays deterministic — the same
  inputs at the same times, the same outcome — so a level's solution is still a test.  Danger
  must be readable before it arrives (proposed): a rolling cart is heard before it is seen, a
  chain of bombs ripples (each one 150 ms after the blast that set it off) rather than all at
  once.  The outcome never depends on how the time is cut into ticks: the world steps
  from one due time to the next, so what one thing sets off within a tick (an enemy's arrow, a
  thrown bomb) runs in it at its own time (`tests/clock.loft`).  A doomed save is allowed — it is fun (owner) — but never bricks a game: see the wells.
- **A test enforces it.** Every shipped level carries a solution test played by the base character
  (`tests/first_climb.loft` is the model).

## Gameplay mechanics wanted

The owner's set of gameplay mechanics for the game, apart from the story's parts (STORY.md): each
has a tag, `@GM-<Name>`, that a place, a puzzle or a piece of code cites the way the story's parts
are cited, and that links to its paragraph here.  `loft tools/tags.loft check` keeps them in step.
Each becomes part of the test dungeons (owner, 2026-11-04): a room of the demo village that shows
it, as the five rooms show the mechanics built so far, and names it in the room's `serves`.  The
new items get rules of their own, tested in a small scenario, and they fit into the mechanics
already designed and built (owner, 2026-11-04).

| Mechanic | What |
|---|---|
| [`@GM-Flooded`](#gm-flooded) | part of a mine flooded; machines needed to enter |
| [`@GM-CartLift`](#gm-cartlift) | a lift that can carry mine carts |
| [`@GM-Flotilla`](#gm-flotilla) | wood pulled together to cross an underground lake |
| [`@GM-Combat`](#gm-combat) | moves and swings of both sides, combined: the standoff and the point, lunges, clashes, dodges and balance, weapons as profiles |

<a id="gm-flooded"></a>**The flooded part** (owner, 2026-11-04): part of a mine is flooded and needs
machines to be entered.

<a id="gm-cartlift"></a>**The cart lift** (owner, 2026-11-04): a lift that can carry mine carts.

<a id="gm-flotilla"></a>**The flotilla** (owner, 2026-11-04): wood that can be pulled together into a
flotilla to cross an underground lake.  **Built** (BUILD.md B5).

**Proposed rules, to be ruled by the owner** (each built from rules that exist; not built).

- **The flooded part.**  **Built** but the daily refill (BUILD.md B4; the numbers there).
  - Water stays the terrain it is: not walked, bats fly over it, things pushed or blown into it
    are lost, a bomb in it douses the torches it reaches.
  - **There is a single flooded part of the mine** (owner, 2026-11-04): its lower tunnels, with no
    special name; it runs in several directions, some of which connect to natural caves.  One of the
    flooded tunnels leads towards the underground lake, but does not connect to it directly — at
    least not any more once the pump has done its first lowering (owner, 2026-11-04): the lake is
    water of its own, which the pump does not drain (@GM-Flotilla).
  - A **pump** is a fixed big thing standing beside the water.  Running, it drains the body of
    water it stands at — the water cells joined to the cell beside it — one cell at a time, each
    drained cell becoming the floor it covered.
  - **Its drive is magical, and depleted** (owner, 2026-11-04): the drive must be brought to the
    alchemist (@ST-Alchemist), and charging it is outside his profession, so he needs aid to get
    it charged (@ST-Magic: magic is crafted).  It needs a lightning strike, which the alchemist
    cannot produce himself; the wizard (@ST-Wizard) has enough energy, but is not easily swayed
    to chime in (owner, 2026-11-04).  The lightning can also be captured on the hill-top, by an
    installation that is already there, where a local monster lives: bigfoot (owner, 2026-11-04;
Tiny Ski's 78–80); it also
    needs a bit of help from the weather (@ST-Outside: the climate), to be fitted in (owner).  The
    alchemist is needed to work the installation, so the way there must be safe for him; he comes
    on his own, but needs convincing (owner).  The drive is a crystal inside the pump (owner): an
    item, taken out of the pump and put back into it charged, as a torch is mounted in a socket.
  - **One charge is not enough** (owner, 2026-11-04): the terrain is unlocked gradually.  The pump
    was designed to be running constantly, and it has been off for some time.  The crystal is
    already inside the pump, depleted; there is probably a spare one somewhere too.  The spare is
    for the same pump, and holds a bit of power, enough for the first step: an alternative way to
    start (owner).  A crystal cannot be harmed: a blast only flings it away, as it flings any
    item (owner).  Small and large crystals do not mix (owner, 2026-11-04): a large crystal in the
    village's well pump makes it explode like a bomb, and the mine's pump ignores a small one.  A charge
    normally lasts two days, but with the amount of water to be pumped now it is spent much
    quicker.
  - Proposed: charged, the pump runs by itself, a cell every so many seconds, heard while it runs,
    until the charge is spent and the crystal is depleted again; each charge drains a part of the
    flooded area, the nearest first.
  - **The water refills without the pump, but only a bit a day** (owner, 2026-11-04), so it never
    suddenly locks the player in or out.  Proposed: a day without the pump turns back a few of
    the drained cells, the last drained first (the ones nearest the pump stay dry longest), never
    a cell the hero or a thing stands on.
  - Under the water lies what was left there: floor, rails, the things a level placed below it
    (a chest, a cart, a lever), found as the water goes — the flooded part's reward.
  - Scenario: a room whose far half is water over a chest, the pump on the dry side; one room
    with a second pump that drains the water off a rail, opening a cart's way.
- **The cart lift.**  **Built** (BUILD.md B3).
  - **In the Old Seam it joins all the levels** (owner, 2026-11-04), but the lowest are not reached
    by it until the pump has run several times: its stop there is under water till then
    (@GM-Flooded).
  - **Some places are reached only with a cart at hand to clear rubble out of the way, and a working
    lift to get the cart there** (owner, 2026-11-04).  Holding a **pick**, with a cart beside him,
    the hero clears rocks away completely: the rubble goes into the cart.  Rubble and rocks are the
    same thing (owner, 2026-11-04): rubble is mostly rocks touching each other, which pushing cannot
    move (a rock is not pushed into another), so the pick and the cart clear it.  The cart must then be
    emptied elsewhere before it can take more (owner).  Proposed: a pick is a held tool as the axe
    is (the axe fells trees, the pick breaks rocks), its blows paced the same way.  As a weapon the
    pick is worse than a sword (owner, 2026-11-04); proposed: a blow with it costs an enemy 1, as
    bare hands do, where the sword and the axe cost 2.
  - **Unloading places** along the track have room to unload onto (owner, 2026-11-04): with the
    cart there, the hero bumps it from the side to empty it.  A full cart is heavier to push (owner,
    2026-11-04).  Proposed: it counts as two carts against push_strength and pull_strength, so a
    full cart in a train makes the train as hard to move as one cart longer.
  - A **lift** is a platform cell with a shaft above or below it: a floor cell, rails on it or
    not, that moves between two layers, carrying its whole stack — a cart, what rests on the cart,
    the hero.  At each stop its rails join the track there, so a cart rolls on and off it as onto
    any rail cell.
  - It is sent by a lever beside it, at either stop — walked into, or hit by an arrow (arrows
    throw levers) — and takes a time to travel, heard and seen, a layer every second.
  - It moves only with its way clear: nothing standing in the shaft, nothing half on and half off
    (a train longer than the platform stops it).
  - **The open shaft can be fallen into** (owner, 2026-11-04): the hero is not protected too much,
    so with the platform at another level there is a way to drop into the shaft — a fall by the
    usual rules — though not from all sides.  Proposed: the shaft is open on its track's sides and
    fenced or walled on the others.
  - A cart rolling onto it at speed stops at its far end when the track does not go on there, as
    at a track's open end with a fixed thing after it.
  - Scenario: an upper track with a cart and a chest at the end of a lower track; the cart is sent
    down on the lift and pushed on to the chest, which it carries back up.
- **The flotilla.**
  - **The wood is in the mine** (owner, 2026-11-04).  The flotilla is not hard to craft: the hard
    part is getting enough straight beams to the lake, and rope to fasten them, which can be found
    in several places (another mine, an abandoned house).  Wood not well crafted the hero just does
    not step on.
  - **Rope is carried beside what the hero holds** (owner, 2026-11-04): around the shoulders, an
    exception to holding one item at a time, drawn as a picture of its own (the hero with the rope
    over the shoulders).  Rope on the shoulders does not block pushing and pulling (owner): that is
    why it is carried apart, since the way to the flotilla's lake has puzzles that rope blocking
    them would make impossible.
  - **The sack** (owner, 2026-11-04): carried in the same shoulder slot as the rope, so never beside
    it, and solely for foraging: it holds what is foraged (plants, meat, ingredients), where the
    hand holds one item at a time.  **Built** (BUILD.md B7b; the demo's herbs house): it carries
    6 (proposed), and a talk's gift is met from it.  Felled trees
    may still be a source too; the eventual layout of the place decides (owner).
  - Proposed: wood (a new big thing) floats: pushed or pulled into water it stays, a cell of
    floating wood, where any other thing is lost.  Proposed: beams fastened with rope are well
    crafted; a beam on its own is not stepped on.
  - Floating logs that touch **link into a flotilla**, as carts link into a train: it moves as one.
  - Proposed: a flotilla of well-crafted wood carries the hero and what rests on it (it is stood
    on, as a derailed cart is); wood not well crafted is not stood on, the hero's step is blocked
    (owner).
  - The hero on it moves it by pushing off: walking into the shore, a wall or a fixed thing sends
    the flotilla the other way, a cell every roll_ms, until it touches something — the rolling
    rule of carts, on water.  It can also be pulled from the shore while the hero stands beside it.
  - Scenario: an underground lake with logs at its shore; two pulled together carry the hero across
    to an island with a chest, the way back pushed off the island's rock.

<a id="gm-combat"></a>**Combat** (owner, 2026-11-04): fighting is one of the hardest things to get right.  A
huge part of it is movement and the swings of both opponents and how they interact: what a step
and a swing does against a side-step and a swing, or a side-step without one.  Combinations of
inputs do different things — a step and a swing with a follow-up swing is not a swing standing
still.  Think of combat as in Dark Souls: parries, dodges, and light and heavy swings.  Each enemy
is tried in a fight room of the test world, a weapon by the door and the enemy at the far end
(owner).  Combat is dynamic in place (owner, 2026-11-04): the hero can stand still and swing, but
that is never the best way — standing still is how two opponents trade blows, where parrying,
blocking and dodging are what comes naturally in a fight.

**The fight, designed again** (owner, 2026-11-07): more dynamic and lifelike, made of distance,
timing and position, both sides moving by the same rules.  It replaces the moves built in B1b (*What
is built*, below).  What the owner ruled is marked (owner); the rest is proposed, every number to be
found by playing in the fight rooms.  Being built (BUILD.md B1c): the keys and the bat's swoop
are built (2026-11-07); the rest is designed.

*The keys* (owner)
- **WASD** walk.  **Q** takes a small item and puts it down; held while stepping, it pulls (a cart,
  anything pulled).  **E** uses: a weapon swings, a bow shoots, a bomb is thrown, an anvil is worked.
  **Tab** chooses the target, for the sword and the bow alike.  **Esc** quits.  **F1** switches
  between the 2D game and the first-person view, **F2** opens and closes the editor.  B, R, N or
  Enter and M stay as they are.
- **Walking over a small item does not take it** (owner): only Q does, so no sword is lost mid-fight
  by stepping on a stick.  Potions in a fight are decided later (owner).
- Proposed: Q takes the item in the cell faced, else the one underfoot (in the first-person view
  what lies underfoot is out of sight), and puts the held one down in the cell faced.  Q acts when
  let go with no step taken, so a press held into a step is a pull, never a take.  Tab locks the
  nearest target in sight, then the next; past the last it lets go.  A lock is lost when its
  target is out of sight or ended, and goes to the nearest other.  With the bow, levers and bombs
  are targets too (§ Bow).  Esc asks twice, as B does.  The arrows walk as WASD do.  In a browser,
  Tab and F1 never reach the page's own use of them.
- **Built** (2026-11-07; `tests/moves.loft`, `checks/game.loft`): WASD and the arrows walk.
  **Q acts on the cell before the hero, never the one it stands on** (owner): `rules::take` takes
  the item there — the rope or the sack onto the shoulders, forage into the sack, anything else
  into the hand, swapping with a full one — and with nothing to take puts the held one down there
  (`rules::put_down`: on the floor, on a bare table, falling over a drop; never into a wall or onto
  an enemy).  Let go with no step taken it acts, held into a step it pulls.  The way the hero
  faces is the way it last stepped.  E uses, Tab aims as the bow's aim did,
  Esc quits, F2 the editor; Shift is the built guard (pressed a parry, held a block) until step 4.
  Still open: Esc twice, Tab's lock for the sword.
- **F3 roams free** (owner, 2026-11-07, an experiment; built, `checks/game.loft`): the keys held move
  the hero's body smoothly at walking pace, diagonals too, and it stands between tiles, drawn in
  the figures' own pass (by its feet, so what is in front still hides it); the rules see it on
  its cell, and crossing half a cell takes the step there (a walk, a push, a pull, a door).  F3
  again, back on the tiles.

*Locked on* (owner)
- **Hit with no target, the hero locks on to the one that hit it** (owner, 2026-11-07): a blow
  landing on a hero that has no enemy locked turns the lock onto the attacker, so a fight begun
  from behind or the side is faced at once.  Proposed: an arrow's shooter too, when the hero sees
  it; a lock already held is kept.
- **The focus takes care of itself; Tab only helps** (owner, 2026-11-07: switching the focus
  easily leaves a player lost; § The principle, *Fun, never lost*).  Proposed:
  - Without a lock E still fights: it swings at, or shoots, the nearest enemy in the arc the hero
    faces, turning to it — a player who never presses Tab fights well enough.
  - The lock comes by itself: hit with no target (above); an enemy that winds up a blow at the
    hero in the arc it faces.
  - It goes by itself and never strands the player: the target ended, out of sight, or left
    behind (the hero more than 4 cells from it); then WASD walk as always.
  - The moves against the target (W in, S back, A and D aside) hold only while it is near, within
    the fight's distances (3 cells): further off, W, A, S and D walk the way they always do, so the
    keys never mean something unexpected far from a fight.
  - The lock is shown in the world: the mark on the target (as the bow's aim is marked), the hero
    turned to it.  Tab picks another in the arc, nearest the line first; past the last it lets go.
- With no target the hero walks; its sword is held at its right, a shield, when it has one,
  against its body.
- With a target the hero faces it, its point raised to it, and WASD are moves against it: **W**
  steps in (a lunge where there is room, *The lunge*); **S** steps back with the guard up, the
  sword's and the shield's; **A** and **D** dodge to the left and the right of the line to it.
- So a fight is the same fight in 2D and in the first-person view: locked, the keys mean the same
  in both, and the 3D page passes them on as they are (RENDER_3D.md turns only free walking).
- Proposed: S steps back once when pressed and keeps the guard up while held; with no room behind,
  the guard is kept where the hero stands (*Position*).

*The weapon's direction* (owner): a weapon and its attacks go eight ways, the diagonals too;
walking and pushing keep four.  A swing sweeps the cell it goes to and the two beside it (facing
north-east: north, north-east and east).  Proposed: a diagonal swing or lunge does not pass
between two walls that meet at a corner.

*The swing* (owner): E pressed starts the swing, drawn back; held longer it becomes a heavy swing
(500 ms, as built); let go, it strikes.  **Swinging opens the swinger to lunges** (owner): while it
is drawn back, its point is out of line.
- Proposed: a swing travels from the release to where it lands (light 100 ms, heavy 200 ms).  Early
  in its flight it is *outgoing*, late *incoming* (*The clash*).

*The point and the standoff* (owner)
- A weapon's point fills the tile in front of its holder (a spear's, two: *Weapons*).  Two ready
  sword fighters stand with **two tiles between them, each filled with a point**: the standoff.
  Nobody is hurt there, and it is the normal default, a stalemate (owner).
- **Stepping in meets the point** (owner): who steps into a ready point is hurt first, flung back
  out, and lunged at.  Without coordination a fighter gets into trouble.
- **Eagerness breaks the stalemate** (owner): the one who starts swinging draws its point out of
  line, and the patient one lunges into it.  A quick tap of E is a feint: it invites the lunge, and
  let go into it the meeting is a clash, which the lighter swing wins (*The clash*).
- The distances, sword against sword:

  | Apart | Between | What it is |
  |---|---|---|
  | 4 or more | 3 tiles or more | approaching: W is a step |
  | 3 | 2 tiles, a point in each | the standoff: safe |
  | 2 | 1 tile, both points in it | blades crossed: who walked in uninvited is flung back to 3, open |
  | 1 | none | inside: the sword is stuck, a knife works (*The off hand*) |

- Proposed: a point run onto costs 1 and flings back a tile, open for 400 ms.  Three ways in:
  stepping in swinging (the meeting is then a clash, not a point: the built lunge's step and swing
  within 150 ms come back as the way to enter), the other's wind-up, and two against one.
- Proposed: each enemy kind has a patience (`patience_ms`, kinds.json; a person's varied by its id,
  so play stays the same each time): how long it holds the standoff before it gets eager.  Its
  eagerness shows, a shift of weight and a sound, before it winds up.

*The lunge* (owner): W with a target at the standoff goes towards it, stepping between the tiles;
at rest after the attack the fighter goes back to the closest tile.  **No lunge beside an enemy: the
sword is stuck there** (owner), so W cannot repeat into blows.
- Proposed: a lunge carries the body about a cell and a half and the point reaches.  It works where
  the other's point is out of line (drawn back, crossed, open); into a ready point it is a step in
  (above).  It strikes for the weapon's damage +1 and breaks a wind-up (poise excepted), as built.
- Proposed, the rules' side: places stay whole cells; a lunge is an offset of the hero's, in 1/16
  of a cell (a pixel of the tile), for the lunge alone, so play stays the same each time.  Between
  two tiles, a blow aimed at either reaches it.  Equally near two, it settles back to the one it
  came from.  It stops where a step could not go on (a closed edge, a fence, a ledge, deep water),
  and never settles onto a door, a well or a ledge's lower side.
- Proposed: walking into an enemy is no longer a blow; blows come from E and the lunge.  Walking
  into a person still talks to it.

*The clash* (owner): when two weapons get close, both attacks stop; a longer swing takes longer to
ready after it, giving the other an open riposte window.  **An incoming heavy swing is hard to
parry, an outgoing swing easy** (owner).
- Proposed: two swings clash when their flights overlap.  After a clash each side recovers for as
  long as its swing was drawn back, plus 200 ms; the one ready first has the riposte (double
  damage, as built).  A swing met:

  | Swing | Met outgoing (its first third) | Met incoming |
  |---|---|---|
  | light | turned aside, the swinger open | turned aside, the swinger open |
  | heavy | turned aside, the swinger open long | it goes through: the guard's damage, the parrier staggered |

- So the cyclops's sweep that cannot be parried becomes this rule: the heaviest swing, met only as
  it leaves, or dodged.
- What beats what (proposed reading):

  | Move | Beats | Loses to |
  |---|---|---|
  | patience, the point ready | a careless step in or lunge | a feint, a heavy swing |
  | the lunge | a swing being wound up | a ready point |
  | the parry | a light swing; a heavy one met outgoing | a heavy swing incoming |
  | the heavy swing | a late parry | a lunge into its long wind-up |
  | the feint | the lunge it draws | a patient opponent |

- Open: whether the parry is the hero's swing meeting the other's (E), its guard turning it aside
  (S), or both, with outcomes of their own.  Two heavy swings clashing leave both open, neither
  first.

*The guard* (owner): S steps back with the guard up, sword and shield.  Proposed: a blow on the
guard is halved, as built; with a shield a light blow costs nothing and a heavy one half.  **With a
wall at the back a fighter cannot readily block** (owner): *Position*.

*The off hand* (owner): instead of a shield, a knife in the other hand: stepping in with it against
swords was deadly in its time.  **That is possible only after a good parry; otherwise the stepper
is hurt first** (owner).  A clash won, the other's blade bound, and the knife comes in.
- Proposed: W beside the target, with a knife in the off hand, stabs.  The off hand holds only a
  knife or a shield (one item more, as the shoulders carry the sack, § What the hero holds) and
  counts as full hands, so a torch never goes in it and the puzzles that need empty hands stay.  A
  knife: 1, a 250 ms recovery, no heavy swing, it cannot clash a sword; held alone in the hand, a
  quick weak weapon.  Open: which hand Q fills first, and which it empties first.

*Weapons are profiles* (owner): the axe has no point but a heavier swing than a sword, a chance
against swords since a heavy swing is hard to parry; yet it will likely lose to a defensive sword
fighter, or to a trained shield and spear fighter.  **A spear fighter is in real trouble once
someone steps close.  Animals lunge without reach, like a pure knife fighter.  Knife fighters and
animals need their dodges a lot** (owner).
- A weapon is a few properties the rules read from kinds.json, never its name (proposed):

  | | point | weight | lunge | off hand | turns 45° in |
  |---|---|---|---|---|---|
  | sword | 1 | middle | from the standoff, with reach | knife or shield | 100 ms |
  | axe | 0 | heavy | none | none, two hands | — |
  | spear | 2 | light, a thrust | from its standoff | shield | 250 ms |
  | knife, small animals | 0 | light | a leap to beside, no reach | — | — |
  | big animals (bigfoot, cyclops) | 0 | heavy, poise | none | — | — |

- **The spear** (proposed): its point fills the two tiles in front, so a sword stands off at four
  apart.  A thrust strikes where it ends, not on its way.  The tile beside the spearman is shaft,
  where it cannot strike, and getting out of it takes two steps back, each one followed.  Its way
  out is a shield's bash; a bare spear inside is lost.
- **Animals** (proposed): a crouch (the wind-up, the tell), then a leap from two tiles that lands
  beside the hero and bites, aimed at the cell the hero stood in when the crouch began.  A leap
  into a ready point is hurt, so animals come from the sides, where the point is not, and a pack
  flanks.  A leap is light: a swing meets it easily as it leaves.  The built back-off from a blade
  swung too early is an animal respecting the point.
- **The axe** reads now as a match-up, not as worse (§ What the hero holds): worse against
  patience, better against eagerness, weak against animals and knives.  It is the tree's tool, so a
  hero who carries it for the trees fights without a point.  Open: whether a heavy swing on a
  shield breaks the guard or only pushes its holder back.
- Open: the profiles of the torch, the pick and bare hands; which of the story's enemies fights
  with which (STORY.md).

*Dodging* (owner: knife fighters and animals need it).  **A dodge brings the dodger out of balance:
that is its cost.  Out of balance, no dodge is possible: the fighter still moves, but enemies
follow freely** (owner).  **Armour protects, and makes dodges less effective: it makes the time
out of balance longer, so a dodge in armour works, but only once** (owner).
- Proposed: **a point takes time to turn**, at the weapon's speed above; the hero's locked point
  follows its target at that speed, not at once.  So a dodge that puts the dodger at the point's
  diagonal opens a moment to leap in.
- Proposed: a dodge glides in 100 ms (a step in 140) and leaves the dodger out of balance for 500
  ms.  A leap or a swing right after a dodge is allowed: dodge-then-leap is the combination,
  dodge-dodge-dodge is not.  Out of balance, A and D are plain steps: a wind-up's blow follows the
  fighter to its new cell instead of landing on the cell it aimed at, and a point turning after it
  is never outpaced.
- Proposed: armour has a weight of its own and takes some of every blow (a point of a blow of 2 or
  more).  The dodge itself is as quick as without it; the time out of balance after it grows with
  the weight (heavy armour twice the 500 ms), so the armoured fighter has one dodge and then
  stands its ground.  Armour is growth (§ The principle): never needed, and the dodges it costs
  are the trade.
- A dodge needs room at the side, so in a corridor a pack cannot get round a point, and on open
  ground it circles.  Animals dodge the hero too, a quick sideways hop: the small and quick often,
  the big barely.
- **A backstab needs no rule of its own** (owner): the point faces one way, the guard faces the
  attacker and a point takes time to turn, so a blow from behind meets no point, no guard and no
  clash.
- Open: a dodge against a diagonal target, a diagonal step between tiles settling as a lunge does,
  or the four-way step that keeps the distance.

*Position* (owner: with a wall at the back a fighter cannot readily block).  Proposed: **giving
ground needs room**, the cell straight behind, away from the attacker, or one of the two diagonal
beside it:

  | | Room behind | A wall behind |
  |---|---|---|
  | the guard (S) | a step back: the blow halved | kept in place: a light blow halved, a heavy one in full, staggered |
  | flung off a point | back a tile, open | pinned: open twice as long |
  | a heavy swing's knock-back (built) | back a cell | not moved: +1 |
  | getting out from inside | a step (a spear, two) | none |

  A fence, a table, deep water or another fighter behind pins as a wall does.  A cornered animal has
  no patience left: it leaps at once.  A ledge behind is not a wall: who gives ground there goes
  over, and the fall's cost is added, as to a blast's; a fight beside a deep drop is the level's
  choice.

*Enemies by templates* (owner): an enemy fights by templates, a set of specific actions as a Dark
Souls boss has, each with the conditions under which the enemy may choose it; some are aggressive,
some defensive.  **The variety of templates gives enemies, animals above all, qualities of their
own, so that how each behaves can be learned** (owner).  **Templates, with their timings in them,
are a better way to make enemies differ than statistics** (owner): a harder enemy is a pattern
harder to read, not a bigger number to wear down (§ The principle: a puzzle, not a grind).
- **Small enemies hit hard but wind up long** (owner, as Souls-like games already engineered it):
  one alone is easy, unless the hero misses it in its sight, or several swarm it.  The danger of
  a fight is in what is not seen and in numbers, never in a blow that gives no warning.
- So (proposed) an enemy kind keeps few numbers: its health, its profile (*Weapons*) and its
  senses (sight, light).  The timings it has today (`windup_ms`, `bite_ms`, `chase_ms`) move
  into its templates, each step with its own, and a blow's damage is the step's too, paired
  with its tell: the harder it hits, the longer its wind-up.  A kind made harder gets a
  template more or a timing tighter, never more health to wear down.
- **The body and the weapon are apart** (owner): a variety of cyclopes, each with another weapon or
  none, all still acting as a cyclops, and some that only grab.  Proposed: the kind is the body
  (its health, senses, poise, size, and what it does outside a fight: searching, calling for
  help), and its templates are chosen by the kind and by what it holds, a template's conditions
  naming the weapon's profile (a point, heavy, none).  What an enemy holds is set where the level
  places it (it holds an item already, and drops it where it falls), so a cyclops with an axe
  sweeps, one with a spear keeps its distance and is in trouble inside, and one with nothing
  grabs and pulls (its grab, built).  The player learns the body once and each weapon once, and
  reads the pair from what it sees.  Open: whether a clash or a heavy blow can knock a weapon
  from a hand: a cyclops disarmed would turn to its grab in the middle of a fight, a boss's
  second phase that needs no rule of its own.
- **Out of a fight too** (owner): the same templates let people and creatures walk, talk and work
  in a way of their own that is recognised.  Proposed: templates for walking (a gait, a stop to
  look round, a patrol's turn), talking (turning to the hero, a gesture on a line, turning away
  when it walks off), working (an axe on a barricade's beam, carrying, sweeping) and resting (to
  sit, to doze), chosen under conditions as a fight's are: the time of day, the place, the facts
  known (talk.loft), what a talk is at.  Templates give the *manner*; what is said stays the
  talk's (talk.loft), where a person goes stays its points by the clock (WORLD.md), what a work
  does stays the rules' (a barricade's units).  So the elder is known by its slow walk and its
  stops, a maid by her hurry, before either says a word, as § What the player sees wants: the
  figures show it.  One set serves a figure in peace and in a fight, so a villager defending the
  barricade fights by the same machinery and in its own manner.  Asks of `motion.loft` poses
  beyond the fight's (a nod, a point, a wave, sitting, a work's swing); each built where a
  template first needs it.  After the fight (BUILD.md B1c), on the same machinery.
- What it asks of the rest (proposed): **sight is a weapon** — in the dark a wind-up is not seen,
  so light (a torch held, a flame set down, daylight) is what makes a fight readable, and rats
  that shun light and attack from the dark are dangerous for that alone; every wind-up is heard
  as well as seen, so one behind the hero or in the dark still warns, more faintly; and a swarm
  is the hard fight, met by the ground (a corridor, a wall at the back of *them*), not by
  outlasting it.  "Hurt, rarely killed" (§ The principle) is kept by the level: swarms are
  placed where the ground gives the hero an answer.
- A template (proposed): its *steps*, a fixed sequence of the moves the hero has (step in, step
  back, dodge, a swing held so long and let go, guard, wait) and of the kind's own where its
  profile gives it one (a leap, a drop from the wall, a bomb thrown, a grab); its *conditions*
  (the distance, the hero's state: drawn back, out of balance, guarding, turned away; its own
  health, a wall behind it or behind the hero, others of its kind near and what they do); its
  kind, aggressive or defensive; what breaks it off or branches it (hit, out of balance, a clash
  won or lost); and a rest before it can be chosen again.  Templates are data the rules read,
  named in a file of their own so kinds can share one (a pack's flank), never a kind's name in
  the code.
- Patience is the time an enemy keeps to defensive templates; when it runs out, or the enemy is
  cornered, the aggressive ones may be chosen.  Two against one is a template with a role in its
  conditions: another of its kind holds the hero at the standoff.
- Fair (proposed): every template opens with its own tell, seen and heard, so the player reads
  which is coming; a template begun runs to its end unless something breaks it, a commitment as
  the hero's held swing is; every aggressive template ends in a gap the hero's lunge fits into;
  the choice among those allowed is seeded by the enemy's id and its count of choices, so a fight
  is the same each time; the rest keeps the same template from coming twice in a row.
- **Enemies live there** (owner): they also scrounge for food, loiter around, or search for the
  hero when they have a reason to.  Proposed: by the clock, never by a hunger meter; food is bait
  (meat put down draws them); a sleeping enemy wakes only for a reason; each kind has its own
  reasons (a blast, a noise, light, a trace, a scent, a call) and its own way of searching, so
  what gives the hero away to whom is learned (TEMPLATES.md § Living there).
- **Scared and wounded have templates of their own, with a different pace or line** (owner), so an
  enemy's state is read from how it moves.  Healing at home is slow, about a point a minute
  (owner): a wounded enemy is off the road for a while and can be found wounded.  Proposed: its
  state chooses its set (calm, searching, scared, wounded, cornered), and a wounded one leaves a
  trail home to follow (TEMPLATES.md § Living there).
- **The area's unease** (owner): all of this is combined with a general anxiety level of the
  dungeon, per area, where what the player does can make everyone more restless and on guard.
  Throwing bombs around is not without consequence (owner): proposed, a blast raises its area
  two steps and its neighbours one, at once.  Proposed: four steps (calm, uneasy, on guard,
  alarmed), raised by blasts, fights, the ended and
  the wounded found, thefts, a light where none should be, lowered by quiet over the clock,
  spreading through doors; it changes which templates are chosen and how patient, how sleepy and
  how searching figures are, never health or damage; nothing on screen shows it but the figures
  (TEMPLATES.md § The area's unease).
- Every kind's set, the people's manners and the hero's test players are written out in
  TEMPLATES.md (proposed, kind by kind, for the owner to rule).
- The scripted players of `tests/fights.loft` are templates run for the hero: one way to drive
  enemies, the fights' tests and, later, followers fighting at the hero's side.

*Found open in the evaluation* (2026-11-07; proposed until ruled):
- **One state, out of balance**: the riposte's *open* and the dodge's *out of balance* are one
  state, entered by a dodge, a clash lost, a point run onto, a hit, or a heavy blow guarded with
  a wall behind; armour makes it longer; a blow on a fighter in it doubles.
- **The guard covers the three cells in front**, the ones a swing sweeps; a blow from the side or
  behind gets through (so the backstab).  Whether the built +1 for a blow on a side stays beside
  it is to rule.
- **Height**: no point and no lunge reaches across a layer's edge, to begin with.
- **Arrows**: an arrow can be dodged (it travels); a shield stops it; out of balance, it is not
  outpaced.
- **Bare hands** are a knife fighter without a knife: no point, so dodging is all it has.
- **A tap is never lost**: the feint and the light swing are taps, so a key pressed and let go
  between two frames must still be seen (RENDER_3D.md: a press held until the next poll, in
  `screen`), in both views.
- **The fight is built and tuned in 2D first**, where both fighters, their points and the flanks
  are seen; the first-person view follows once the figures there hold their weapons and show
  their facing (RENDER_3D.md, *Not yet*), with a sound for what is behind the hero.

*Kept from what is built*: every attack has a tell, both ways, and lands on the cells it aimed at
when its wind-up began; standing still trades blows; the light chain; the heavy swing's +1 and
stagger; a blow on a side +1; poise; the back-off from a blade swung too early; groups; the order
of acts (`rules::acts_before`); fair and readable.  **Growth is never necessary** (§ The
principle): every match-up the story puts before the hero is won by the base sword with skill
(patience, a thrust parried as it leaves, a step inside), and a fight room holds each such claim in
a test.

*What is built (BUILD.md B1b), and what replaces it.*  `tests/combat.loft` times each built move by
hand; `tests/fights.loft` plays the fight rooms with three scripted players (an attacker, a
parrier, a dodger) and holds the owner's two claims: the attacker is hurt in the rat run and the
cyclops den, and the defensive players land their first blow later.

| Built | Replaced by |
|---|---|
| keys by hand (owner, 2026-11-04): J swing, K guard and pull, L put down, I aim, or the arrows with Space, Shift, X and T; E the editor; Q quits | *The keys* |
| walking over an item swaps it with the one held | Q |
| the block: K held standing still, facing the blow, halved | the guard, S |
| the parry: K pressed in the last 150 ms of a wind-up, the enemy left open; a riposte doubles | the clash; the riposte stays |
| the lunge: a swing within 150 ms of a step the same way, +1, a thrust | W from the standoff; a step and a swing together return as a way in |
| the stance: in a fight a step does not turn the hero | locked on |
| a walk into an enemy is a blow | proposed: no blow |
| a sweep cannot be parried | the heavy swing, incoming |
| a swing lands the moment it is let go | a swing travels |

The claims stay: a fighter who only swings is hurt, a defensive one lands its first blow later.
The three scripted players become a striker, a patient fencer (holds, feints, lunges into
eagerness) and a dodger, and the fights are played again.

- **How it is seen** (built: `motion.loft`, `game.loft`; every pose and number proposed, to change
  once played).  Nothing on screen says what happened; the figures show it (§ What the player sees):
  - *Guard* (built on K held; S in the new keys): the weapon turned across the body, drawn in, the hero crouched and leaning
    back.  A blow on it: a dull knock, a small spark where the weapons meet, the attacker's strike
    seen, and the hero pushed back two pixels with the guard held — a brace, never the stagger of a
    hit, even when half the blow came through (heard, softer).  Empty-handed, the crouch and lean
    alone.
  - *Parry* (built on K tapped; a clash in the new design): the weapon flicks out from across the body past the facing.  Turned aside:
    a bright ring, a big spark, the attacker flung back twice as hard as a hit; it is then drawn
    *open* — off balance, its weapon hanging out past its stroke — for as long as a riposte is
    possible.  Too early or late, the flick meets nothing and the blow lands: the timing is taught
    by seeing it.  No glint marks the window: the wind-up's draw-back, furthest at the end, is the
    cue.
  - *Riposte*: a heavy crack, the enemy reeling twice as far.
  - *Wind-ups*: an enemy raises its weapon back as the hero's swing does; a sweep, which cannot be
    parried, swings it far back level instead, leaning further — the two read apart.
  - *Wounded*: any figure at a third of its health or less sags a pixel and, standing, sways.
  - Not built, to try in play: a hit-stop (the figures held still for a moment at a parry).
  - Proposed for the new design: a ready point held out into its tile, for the hero and the
    enemies alike, so the standoff is seen; the locked target marked, in the first-person view on
    its card; an enemy's eagerness, a shift of weight and a sound, before its wind-up; a heavy
    wind-up drawn far back, a light one short; out of balance, a fighter's lean and stumble.  In the
    first-person view an enemy's card faces the eye and its point aims at it, so stepping onto a
    point also glints and is heard.

*Engine notes (not leading).*  All three are new to the engine.  Water today is a terrain that
blocks walking (bats fly over it) and never changes: a part that floods and drains asks for water
whose extent follows a machine's state.  A lift asks for a platform that moves between layers
carrying what stands on it, a cart and its rails' joins included; today a cart changes layer only
by being pulled over a layer's edge.  The flotilla asks for things that float: today nothing stands
on water, and things pushed or blown into it are lost.

## The world

- **Layers.** `z` is the floor level, unbounded both ways.
  - Each layer is an unbounded grid of sparse 32x32 chunks.
  - Terrain is 2 bytes a cell; movable things are per-cell stacks (slice 1).
- **Heights.** Height counts in steps.
  - A layer is 4 steps: the height of a wall.
  - A chair is 1 and a table 2, so a chair on a table is 3.
  - The hero steps up 1 and down any amount.
  - So a chair climbs a table, and a chair on a table climbs a wall: the wall's top is the next
    layer's floor.
  - A slope is cells rising one step at a time, and its top (3) steps onto the layer above too.
  - Built with 4-step layers, slopes included (slice 2, `tests/moves.loft`).
- **Falling.** A drop of a full layer (4 steps) or more is a fall, and costs health by the layers
  fallen: 1 layer costs 1, 2 cost 3, 3 cost 6 (kinds.json `fall_damage`; deeper falls cost the
  last).  The hero starts with 6 (`health`), so a fall of three layers takes it all.  A shorter
  drop — onto a chair below a ledge — costs nothing.  With no health left the hero moves no
  more; until wells and safe points exist, R restarts the level.  The three-layer fall stays
  deadly on purpose; levels keep big heights out of their beginnings (§ The principle).  Built (slice 2). A bomb that drops a level
  explodes on landing.
- **Ceilings.** A cell has a ceiling when the layer above it is not void.  How low it hangs, in
  steps, is kept per cell for the 3D renderer (RENDER_3D.md § Ceilings); the 2D draws none and no
  rule reads it.
  - The ceiling is never drawn.
  - The hero cannot climb up through a ceiling (built, slice 2), and neither can a spider.
- **Terrain kinds.**
  - Floor and rails.
  - Water: blocks walking, but bats fly over it.
  - Wall: rough walls are climbable by spiders; smooth walls (optional) are not.  A wall can be
    slanted: drawn at 45 degrees, still a whole wall (§ Slanted rock).
  - Fence: the hero climbs over it like a height-1 step. Nothing is pushed or pulled across it,
    and it stops trains. Bats fly over it. Built (slice 2) as a terrain cell and as an edge
    between two cells; bats wait for enemies.
  - Tree: blocks walking; light and sight pass it (built, slice 2).
  - Door: walking into a closed one opens it (built, slice 2).
  - Stairs: walking onto them takes the hero to the same cell a layer up or down (built, slice 2).
  - Well.

## Moving things

- **Push and pull.** Walking pushes; `pull` drags what is behind the hero (slice 1).
- **One item a cell.** A cell holds at most one item (what the hero walks over and takes: a
  potion, the staff, the axe, a torch) — on its floor, or resting on a big thing there — and
  nothing is ever put on top of one, so every item stays in sight and in reach.  Built
  (`tests/items.loft`).
  - Items are light: a thing pushed or pulled onto one pushes it one cell on, any time, when
    there is room (floor at its height, nothing on it, no edge in the way).
  - A string of two: the far item goes to the side, if there is room, and the near one takes
    its cell.  Only two are moved; a third stays.
  - **Which side** is the place's own: every place on the ground prefers the right or the left
    of the way (then tries the other), fixed by a hash of where it is.  The same push in the
    same place always goes the same way — the game is deterministic — but the pattern follows
    no row, column or checkerboard, so it is learnt by playing, not read off the grid.  Each
    layer has its own.
  - **Over a drop an item falls**: shoved (or sent aside) into the void, a light item falls
    down the column, a burning torch keeping its time.  It comes to rest on the first floor
    with nothing on it, or on a big thing with nothing on it.  Onto anything else — another item,
    a chair, an anvil — it rolls off beside it (the side that place prefers, then the other,
    then straight on, falling on over a drop there); the chair is never harmed.  With nowhere to
    go (water, no floor at all, no room around) the falling item is destroyed, and the push
    still happens.  A rock or a chair stops at the edge.
  - On its own layer, with no room for the item, the push or the pull does not happen.
  - The hero puts nothing down where an item lies (it stands on one only after a swap), nor on
    a chair; on a big thing it stands on, it does.
- **Big things.** A chest, a table, a stone, a tar barrel and a cart are big: the hero stands on
  them (all height 2, the cart 1), and an item comes to rest on them — fallen, or put down
  there.  The hero standing there takes it.  A big thing that moves carries its item along (a
  stone, a barrel, a cart in its train); a chest opened keeps it on top.  Built
  (`tests/items.loft`, `tests/moves.loft`).
  - A burning torch pushed along keeps its time.
- **Carts.**
  - Carts run on rails, and a line of carts is a train.
  - The base strength pushes 3 carts and pulls 2.
  - A train that cannot move — too heavy, at the rails' end, held by a stone — is climbed: the
    hero steps onto its first cart (height 1), so a stuck train is a walkway, not a wall.
  - A green potion gives +2 to both for 60 s of the world clock, at once.  Walked over with the
    hands full it is drunk; empty-handed it is picked up, and J drinks it (owner).
- **Tracks and rolling carts.**  The owner's rules (2026-10-30), and the proposals not yet
  confirmed marked so.  Built, step 1 (`tests/tracks.loft`): tracks from the rails' joins (each
  rail cell joins the rail beside it; painted pieces, which keep parallel tracks apart, come with
  the junctions), carts along them round bends, pushed and pulled, trains along the track, and
  derailing past an open end.  Built, step 2 (`tests/tracks.loft`): painted pieces (a cell's
  picture, theme.json `tracks` and `junctions`, keeping parallel tracks apart), junctions (in by
  the toe: the setting; in by a branch: to the toe), the lever (walked into, it sets the junctions
  beside it the other way: their twin picture, its handle over), and the lean's choice.  Built,
  step 3 (`tests/rolling.loft`): rails on slopes (a step up or down between rail cells; pushing
  up a step counts every cart twice), rolling by itself on the world clock with speed that builds
  and coasts (the times worked by hand in the tests, the same in one tick or many), linking by
  weight, the hero hurt and pushed aside or taken on board, riding, and leaning by the arrows while
  rolling.  The game hears every cell a cart rolls and returns a hero a cart kills to its well.
  A track runs on over a layer's edge: a rail on a slope's top (rise 3, nothing overhead) joins the
  rail beside it on the layer above at its floor, both ways, so a long slope crosses layers and a
  train can span two; a cart is pushed up over the edge too.  The hero pushes standing on the
  ground within a step of the cart's base, on a slope as well.  Pulled over the edge too
  (`tests/rolling.loft`): the cart behind the hero is found along its track across the edge, a
  train is followed car by car across it, and the hero may step down over the edge onto the
  slope's top while pulling (less than a layer; never up, as everywhere while pulling); a step up
  onto the hero's cell counts every cart twice.  The editor paints junctions ("junction 1" .. "junction 8", each as drawn set
  straight, in the Tracks category) and slopes (`]` / `[`).  A hero riding a cart that derails stays on it (proposed, as built).
  - **A track follows its picture.**  A rail cell's tile (Kenney's straight and bend pieces, and
    our junctions) says which sides it joins: straight, bend, end or junction.  Two tracks side by
    side do not fuse.
  - **Carts stay on the track.**  Pushed, pulled or rolling, a cart moves along its track, round
    bends (a push east into a bend that turns south sends it south); it never falls off.  A
    train's carts follow each other along the track.
  - **Derailing.**  Past a track's end with nothing placed after it, a cart derails: it leaves the
    track onto the next floor and becomes a fixed big thing, like a chest — stood on, holding an
    item, never moved again.  With something after the end, it stops at the end.
  - **Junctions: a switch, and leaning.**  A junction (drawn from Kenney's straight and bend
    pieces, in our sheet) has two settings, shown by its picture; walking into the lever beside it
    flips it.  Whatever reaches the junction follows the setting — unless the hero rides the cart
    and leans: one arrow press at any time during a downhill run is remembered until the next
    junction, which then takes the branch in that direction if it has one (generous: no timing).
    While the cart rolls the arrows lean; the hero steps off once it stops (proposed).
  - **Slopes.**  Rails can lie on a slope; a cart there rolls downhill by itself on the world clock
    (deterministic), and the hero can ride it.  Several slope tiles build speed, and it takes time
    to stop again.  Proposed numbers: speed 0..4; a step down +1, a step up -2, on the flat -1 every
    2 cells (8 cells' coast from 4); at speed n a cell every 600 / n ms; pushing uphill counts each
    cart twice.  A train rolls as one, at its front cart's speed.
  - **Carts that meet link together** and roll on as one train, sharing a combined slower
    speed: the momentum shared over the carts, each counting once (rounded down, proposed) — a
    train of two at 4 meeting one standing cart rolls on at 2.
  - **A rolling cart hurts the hero** it meets, pushing it aside — a cart on a slope doubles as a
    trap.  Proposed: it costs the cart's speed in health (1..4; a three-layer fall is 6); the hero
    goes to the side the place prefers, then the other; with no room either side the cart takes
    it on board as a passenger, hurt all the same — a costly way to get on a cart.
  - **Open:** what counts as "something after the end" (any thing or wall, proposed).
  - **Already built** that carts keep: they carry an item, are climbed when they cannot move, and
    light items on the rails ahead are pushed out of the way.
- **Stones (rocks), anvils and tar barrels.** Pushed only, never pulled, and never by a cart or
  another stone. On the rails they block trains (slice 1).  A stone and a tar barrel are big
  things (height 2, stood on); an anvil is not (height 1, items roll off it).
- **Chests.**
  - A chest never moves: it is not pushed or pulled.
  - Walking into a closed chest opens it (one move; the hero stays put).
  - With a chair beside it, it is walked on like a table (height 2), but nothing stands on it:
    a chair is not lifted onto it.
  - Built (slice 2).
- **Chairs.**
  - A chair slides on the floor or along tables.
  - It is pulled up onto a table the hero stands on (slice 1).
  - A bomb breaks it.

## What the hero holds

- **The axe is a worse weapon than the sword** (owner, 2026-11-04): it does the same damage, but
  its weight makes it take longer to ready again after a blow.  Proposed: the axe's recovery is
  half again the sword's (600 ms against 400), a recovery per weapon where the game has one for all.
  Read again (owner, 2026-11-07; @GM-Combat, *Weapons are profiles*): no point and a heavier
  swing, so a match-up rather than simply worse.
- **A torch is a reasonable weapon against bats** (owner, 2026-11-04): it hurts them for 2, and
  any other enemy for 1, as the pick does.  A weapon is a good idea in combat (owner).
- **Reach** (owner, 2026-11-04): any item held in the hand reaches 3 tiles when swung, where bare
  hands reach only the one tile faced.  *Reading*: the swing sweeps the cell faced and the two
  beside it across the swing (the swing already turns from 70° behind the facing to 70° beyond).
- **A blow takes its time** (owner; built, `tests/blows.loft`; 400 ms proposed).  Every blow —
  a swing, a walk into an enemy, an axe into a tree — is followed by a recovery.  A blow pressed
  within it is not struck then; one more blow lands when it is over, however often it was
  pressed — unless the hero does anything else first (a step, a pull, putting down, throwing,
  shooting).  So a fight is paced by the hero's blows, not by how fast keys are pressed.
- **One item, no inventory.** The hero holds at most one item, and it is always on screen.
  - Walking over an item swaps it with the one held.  Designed again (owner, 2026-11-07;
    @GM-Combat, *The keys*): walking over a small item no longer takes it; Q takes it and puts
    the held one down.
  - `drop` (L, or X) puts the held item down; Q in the new keys.
  - **Full hands.** While holding any item, the hero cannot push, pull or lift anything. So
    the cellar's torch is put down beside the rock before the rock is pushed.
  - Built (slice 2): the swap, drop and full hands.
- **Sword.** The hero fights with it: a blow costs an enemy 2 (§ Enemies; built with the rats).
- **Axe.** Walking into a fence or a tree with it chops it down to floor (built, slice 2).
  - A tree takes 4 blows (kinds.json `hits`); each tree counts its own, and a well's save keeps
    the count.  A fence still falls at one.
  - Every blow leaves a stick beside the tree (`drops`), when there is room: the side its place
    prefers, then the other, as an item pushed aside — over a ledge it falls to the floor below.
    A tree is where sticks come from — the axe's link to the torches.  So a tree reached only
    from one ledge, its sticks falling to the layer below, is a puzzle: each stick costs the way
    down and back up, and a player who wants several pays it several times.  Built
    (`tests/moves.loft`, `tests/items.loft`).
- **Bow.**
  - Shoots enemies higher up on walls (bats, spiders), with no ammunition limit.
  - An arrow sets off a bomb it hits, as it hurts an enemy (owner): a bomb set off from afar.
  - The hero's arrow throws a lever it hits (owner) — a junction set from across a chasm; the
    hunter's arrows do not.
  - **Aim** (owner).  The target is the closest enemy or thing to hit (a lever, a bomb) in line
    with the hero; pressing the target key again switches directly to the next.  The lock is lost
    when the target goes out of the hero's view.  The targeting has to be good enough for this to
    work — levers and bombs among enemies.
  - **Built** (`tests/bow.loft`; the numbers proposed).  J (or Space) shoots (at the locked
    target, else the way the hero faces, turning it there); I (or T) locks on the next target.  An arrow flies
    straight, a cell every 30 ms (`arrow_ms`) up to 8 cells (`arrow_range`), striking what it meets
    when it gets there — an enemy can still step out of its way: over items, fences, water and
    chasms; stopped by a wall, a closed edge or a big thing.  It strikes an enemy for 1, a spider
    up on the wall it reaches too (out of a sword's reach), throws a lever and sets off a bomb.  The
    targets are the first such thing in each of the four ways, nearest first (north, east, south,
    west among equals); a lock is lost when its target is no longer one.  The game marks the
    target (Kenney's battle reticle) and draws the arrow flying.
- **Bomb.**
  - It is thrown like a bowling ball: it rolls in a straight line and ignites on impact.
  - **A bomb rolls; other things do not** (owner).  On a slope it starts rolling downhill by
    itself; it speeds up downhill and slows uphill as a cart does, and on the flat it slows one
    speed every 4 cells (`bomb_coast`).  Thrown (J or Space, the way the hero faces) it starts at speed
    4, a cell every 150 ms: up to 16 cells on the flat.
  - **The thrower fuses it** (owner).  A thrown bomb goes off on **impact** — hitting something
    while it moves: a wall, a thing, the hero, the ground after a fall — and rolling into water
    sets it off there.  A thrown bomb that slows to a stop by itself lies there, not lit, to be
    picked up again (owner).  A throw into a wall or a thing at the hero's feet is refused, as a
    walk into it is (owner).
  - **A bomb nobody fused** — rolling down a slope by itself, or put down on one — only stops
    where it hits, and sinks in water; so one put down on a slope rolls off harmlessly.  **But
    damage sets any bomb off** (owner): a blast reaching it, and a fall of `bomb_fall` layers or
    more (proposed 2: a one-layer drop it survives) — so a bomb rolling off a high ledge goes off
    below.  An arrow will too (the bow).
  - **Built** (step 1, `tests/bombs.loft`): rolling, fusing, impact, falls, the reach (radius 3, `blast_reach`),
    the hero's damage and push, chairs breaking, the ripple of chains (the hero's own bomb
    included), torches lit in their sockets and put out by a blast in water.  **Built** (step 2,
    `tests/blast.loft`): items flying 5 - d (over items, short of walls, big things and the hero;
    falling over drops; lost in water; rolling off an item they come down on, or stopping a cell
    short), potions shattering within 1, big things sliding 3 - d, farthest first, an item on a
    big thing landing ahead of it, carts rolling off along their tracks at 4 - d and held across
    them, carts beside a blast blown onto the next rail or derailed, and derailed carts beside a
    blast pushed the same way — a rail they reach puts them back on a track (owner).  A bomb the
    blast sets off goes off where it lies, not thrown first; one 3 away is thrown like any item.
    **Built** (step 3): the pictures, in our own sheet (ART.md, tiles 60–65) — the bomb, its
    sparks while it is lit, the blast's flash, fireball and smoke — and the blast as a light for
    its 300 ms (`flash_ms`), fading three shades, so the room flashes.
  - **The blast** (the owner's rules, 2026-10-30, and proposals marked so).
    It reaches every cell within its radius (proposed 3) by the shortest way the blast can take:
    it spreads like light, so walls and closed doors stop it and a wall cell takes it but passes
    none on.  Distance d counts those steps, 0 on the bomb's own cell.  Everything it reaches is
    struck at once, resolved farthest first so nothing is pushed into a thing about to move.
    - **Push, faster the nearer** (owner): a thing at distance d is pushed away from the bomb
      with speed 4 - d (3 beside it, 1 at the edge).  "Away" is the axis the bomb lies further
      along; on an even diagonal, the side that place prefers (as for items pushed aside).
    - **Big things resist** (owner): they move less than small things — proposed, one cell less
      than the push speed (2 beside the bomb, 1 two away, none at the edge), sliding until
      something stops them; a stone, a barrel, an anvil, a chair.  A chest never moves.
      Proposed: a big thing stopped by another big thing stops there, passing nothing on.
    - **Carts are pushed along their tracks** (owner), and along the track they do not resist:
      the full push speed becomes the train's rolling speed in the direction of the track nearest
      to "away".  Across the track the rails hold them: they do not move.  Rolling, a cart is a
      cart like any other: it slows on the flat, links with what it meets, hurts the hero.
    - **Beside the bomb a cart is blown off its track** (owner): at distance 1 it slides away like
      a big thing, whatever way its track runs.  The first rail it reaches catches it: it stays a
      cart on that track — so a well-placed bomb moves a cart onto the track beside its own (the
      owner's puzzle).  Landing on plain floor, it comes to rest derailed (a fixed big thing).
      With no room to leave the rails, it stays on its track (proposed).
    - **Damage, more the nearer** (owner): the hero and enemies lose 4 - d health (proposed: 4 on
      the bomb's own cell); the hero is also pushed, like a light thing.
    - **Fragile things break**: a chair within distance 2 (proposed).
    - **A directed blast** (owner, 2026-11-05): stones pushed up to a bomb — a small dyke of them
      behind it — direct its explosion: the push goes one way, instead of the general push to all
      sides.  It is a rule of the world, not a learnt skill: any player can find it, but it is not
      obvious to gamers, and Wick can explain it (STORY.md @ST-Blaster).  The model (owner): a
      simplified version of real blasting, where TNT needs a sand foot — tamping — to be directed;
      without it, it blows up mostly air.  The ordinary blast stays as it is, everywhere (owner,
      2026-11-05); the directed one does more besides: it can crack walls and blow out locked
      doors (owner, 2026-11-05).  Its air travels further from the stones, out the open way, so
      more can be hit there, and harder (owner, 2026-11-05; proposed: a reach of 5 instead of 3,
      the push and the harm one stronger at each distance).  Five stones direct it fully (owner,
      2026-11-05): read as the five cells round the bomb that are not ahead of it — behind it,
      beside it, and the two behind it on the slant — so the blast goes out over the three ahead.
      That is the difference (owner, 2026-11-05): a full, interconnected dyke holds — its stones
      stay, and the blast is directed; a partial dyke is partly moved — its stones are pushed as
      in any blast.  No fuse, no delay (owner, 2026-11-05): lit, it goes off at once, and the dyke
      is the only way to light it safely — the wall that keeps the hero from harm.  Lit from
      behind the dyke: walking into one of its stones with a burning torch, the bomb right behind
      that stone (proposed, taken by the owner as the reason it stays hidden).  A full dyke built
      and then lit over a stone: players will not come across it unless they are very persistent
      in their experiments (owner, 2026-11-05) — which is why Wick telling it matters.  Built with
      the proposals: a cracked wall becomes rubble (a stone on floor); the first wall straight
      ahead cracks, one a blast.
    - **A door held shut** (owner, 2026-11-05): the locked door is a door held shut by a beam on
      the side it opens towards, and that beam can be demolished.  From the beam's side the player
      lifts it, no bomb needed (owner, 2026-11-05); from the other side only the directed blast
      opens the door.  Built: one terrain kind, `barred`, the side of each door's beam in the
      level's `bars`; the beam lifted comes away whole (proposed), into the hand or at the feet,
      the door left closed; one a blast hits is broken, the door blown open.
    - **A way round puzzles** (owner, 2026-11-05): the directed blast — cracking a wall, blowing
      out a door held shut — is a way to circumvent some of the puzzles a place sets up: the
      persistent experimenter's reward.  So a place is designed knowing it: a puzzle it lets be
      skipped is a choice.  There is no wall it cannot crack (owner, 2026-11-05); what holds is
      room: the dyke is five stones pushed to the bomb, and a wall is not one of them, so in a
      one-wide tunnel no dyke can be built to blast a side (owner, 2026-11-05).  It is still
      possible by blasting the tunnel's sides again and again until it is three wide — and that is
      where it ends (owner, 2026-11-05): a tunnel widened so brings a bigger cave-in, which beams do
      not help against, and getting its rubble out for the next blast becomes impossible.  So a
      place keeps a puzzle from being blasted round by giving it no room for a dyke, and the
      widening that would make room brings the tunnel down for good.  It is impossible by how
      wide the cave-in is (owner, 2026-11-05): in a one-wide tunnel a cave-in puts its stones once
      — every cell there is beside a wall, and once cleared it stays clear (so the cave-in of a
      standing beam, B2b, is cleared again with the pick and a cart); but a cell one step further
      from a wall keeps gathering stones — clear it and more fall.  A widened span never empties,
      so its rubble cannot be got out for the next blast.  A natural cave holds: it has no level
      ceiling but a dome (owner, 2026-11-05), so its open middle does not cave in; only a span
      made wide by blasting, under a level ceiling, keeps gathering stones.  The dome is modelled
      with the layers (owner, 2026-11-05): a rock wall can move one cell in per layer up, so a wide
      cave needs height — its walls step in layer by layer until they close.  Proposed, read from
      that: a cell under a ceiling (something set on the layer above it) must have a wall beside
      it, or it is unsupported — and an unsupported cell keeps gathering stones.  A one-wide tunnel
      holds (walls beside every cell); a dome holds (above its inner cells the cave goes on up, no
      ceiling there; above its edge cells the stepped-in wall, with a wall beside them); a tunnel
      blasted three wide does not (its middle has the level ceiling above and no wall beside).
    - **Crag and dome** (owner, 2026-11-05): walls moving in one cell a layer make a crag, not a
      dome; in a dome the higher layers push so far towards the middle that bigger spans hold
      there — the middle 8 at the top, the walls beside it a step of 4, then of 2, then the one-cell
      steps all the way down.  Built as a **ceiling-height field** (the kit's `domes`, proposed by
      us for any width and height and for shapes combined): each floor cell's ceiling layer from its
      distance to the edge of the floor (eight neighbours; a straight side counted `straight`
      further) — a crag a layer a step; a dome measured from its middle, r steps in: the top for r
      up to 4, a layer down to 8, another to 10, then a layer a step to its foot, so its own height is
      its middle's distance less 8.  A chosen `height` raises it (straight walls first) or flattens
      it.  Shapes on one floor that overlap are one cave: each shaped by its distances to the edge
      of the joined floor (so where they meet the ceiling does not dip), every cell's ceiling the
      highest any gives it — a dome with a crag's gallery off it, two domes and the saddle between.
    - **Built** (BUILD.md B2c; `tests/blasting.loft`, the demo's blasting house), with these
      proposals, to tune in play: a dyke is checked for the ways north, east, south, west in
      turn; the directed blast strikes only the cells ahead, all pushed that way, out to 5 steps,
      each as if a step nearer; lit over a stone it goes off at once, whatever set it off; the
      first wall straight ahead cracks into a stone on floor, a door there (held shut or not) is
      blown open; a beam lifted comes away whole (in the hand, else at the feet); a ceiling is
      rock above (a floor or a bridge up there is none); a stone falls on a free unstable cell
      1000 ms after it is free, and strikes the hero standing there (1).  The cave-in acts where a
      crack leaves cells unsupported — within 2 of it — never on a place as it was drawn: the
      early dungeons, flat under a cover of wall for their dark (the cyclops hall, the old mine,
      the torch room), stay as they are; `tools/stable.loft` shows what blasting there would
      bring down.
  - **Small items in the blast** (proposed).  They are light, so they fly further: speed 4 - d + 1,
    a cell a step, in the same "away".  They stop at the cell before a wall or a big thing; over
    a drop they fall (and come to rest, roll off, or are lost, as any falling item); landing on
    another item they roll off beside it.
    - **A potion shatters** within distance 1: glass.  The rest survive.
    - **A burning torch keeps burning** and its time; a flung torch is a moving light.
    - **An item resting on a big thing is flung off it**: lighter, it flies further than its
      stone or cart moves, so it lands ahead of it.
    - **Items lying on a track** are flung like any item; the track does not hold them.  A cart
      the blast sets rolling shoves the items ahead of it on the rails out of the way, as a push.
  - **The blast lights a torch on a wall** (owner): an unlit torch in a socket within its reach
    stays in the socket and burns, with its whole time; a burning one burns on (proposed); a
    stick has no tar and is not lit.
  - **A bomb in water throws water around** (owner): it still goes off, but its blast douses the
    torches it reaches instead of lighting them — proposed: every burning torch in reach, in a
    socket, on the floor or in the hero's hand, becomes an unlit torch (still tarred, relit like
    any), and it lights none.  It pushes, hurts and chains like any blast.
  - **Chains.**  A bomb within distance 0, 1 or 2 of a blast goes off too (owner) — the one the
    hero holds as well: it goes off in the hero's hands, distance 0, the full damage.  A hazard the
    player learns by meeting it (owner) — no level teaches it.  Proposed: a
    tar barrel within distance 1 bursts into a fire that burns like a flame (it lights torches)
    for its burn time, further out it is only pushed; a bomb riding a cart goes off when the cart
    is stopped by something — a way to send a blast down a track.

## Torches and light

- **No general fire spreading** (owner, 2026-11-04; INTERACTIONS.md 21): fire does not spread from
  wood to wood in the mine.  A possible puzzle: a tar barrel set alight to burn away an accidental
  barricade (fallen wood) blocking a walkway.  The accidental barricade looks as the village's
  barricade does (one picture for both), and can also be taken apart with a pick, into 2 beams
  that can be carried all the way down to the lake (owner, 2026-11-04; @GM-Flotilla): burning it
  opens the way, taking it apart opens the way and gives beams.  There are more beams in the mine,
  but some hold a tunnel up: taken out with a pick, they collapse it (owner, 2026-11-04), so the
  player plans with care which beams to take.  Most beams just lie on the ground, put there for the
  builder miners; but there are too few of those if the accidental barricade is burned, and then
  the ones with an active function are needed (owner, 2026-11-04).  So a beam lying loose is safe
  to take and a beam standing in its place holds something up: the difference is on screen.  The
  risk is in the planning (owner, 2026-11-04): a caved-in tunnel no longer gives ready access to
  where it led.  Proposed: the collapse fills it with rubble (rocks touching), so the way can still
  be cleared with the pick and a cart, at a cost in time, never a dead end.  **Built** with the
  pick, rubble and the cart's load (BUILD.md B2, B2b; the demo's cave-in house).

The player lights the dungeon.  Built (`tests/torches.loft`, and `the torch room`, the third
level, played to its goal in `tests/levels.loft`) except the enemies' part, which waits for the
enemies.  The tiles are the game's own sheet ([ART.md](ART.md)).

- **Light is a bonus, never a wall.**  The darkest shade is never black: the player can always
  go on in the semi-dark, or spend the time to fetch sticks, tar them and relight.  For now
  light helps (and draws bats); later, darkness becomes a tool too (see the cyclopes, under
  Enemies), so carrying a torch is a choice with a cost as well as a gain.
- **Dark by default.**  A dungeon cell is lit only by what gives light: open sky, a lit
  terrain, or a burning torch.  Light falls off one shade a step from it, darkest 7 steps away,
  and it never passes an opaque cell or edge.  This is `tile_world::shades` as it stands today.
- **Stick, torch, burning torch.**  These are three states of one held item, and all follow the
  held-item rules.  Walking over one swaps it with the one held, L (or X) drops it, and holding one
  means full hands.
  - A **stick** gives no light.
  - A **torch** is a stick dipped in tar.  It gives no light until it is lit.
  - A **burning torch** gives light wherever it is: held, on the floor, or in a socket.
- **The tar barrel.**  Walking into one while holding a stick turns the stick into a torch.  The
  barrel never moves (like a chest), stands 2 steps high and never runs dry.
- **Lighting.**  A torch is lit from a flame, never by itself.
  - Holding a torch and walking into a burning torch in a socket lights the one held.
  - Holding a burning torch and walking into a socket with a torch lights the socket's torch.
  - In both, the hero stays put and keeps what it holds.
  - A level gets its first flame from the editor: a burning torch in a socket or on the floor.
    It starts burning when the level does, with its whole time.
- **Wall sockets.**  A socket is a thing on a wall cell, placed in the editor.  It never moves
  and it is never taken.
  - A torch or the staff is fixed to a wall only in a socket.  This replaces "any wall
    mounts", so the level decides where light can be hung.
  - Walking into a socket with the empty hand takes what it holds.  Walking into an empty
    socket while holding a torch (burning or not), a stick or the staff fixes it there.
- **Burning out.**  A torch burns for 10 minutes of the level's clock (kinds.json `burn_ms`,
  600 000) from the moment it is lit, wherever it is.
  - **The warning:** its light fades as it burns, `fade` (4) shades over its life, a shade every
    2:30, so a fresh torch lights 7 cells and a dying one about 4.  In its last minute the
    flame is drawn low too (kinds.json `low_ms`).  A burning flame flickers between two pictures.
  - **Its light flickers** (a torch's, not the staff's): now and then one shade dimmer than its
    fade, never more and never brighter, steady through each 500 ms step, about one step in four
    (kinds.json `flicker_ms`, `flicker_odds`), each torch on its own pattern — a living flame,
    not a flashing screen.
  - At the end it is a stick again, in the hand, on the floor or in the socket, and can be
    dipped in tar again.
  - Each torch keeps its own time.  The time is part of the situation a well saves.
- **Light and the enemies** (they wait for the enemies to be built; the numbers are open).  Both
  rules use the shade of the cell an enemy may step to, so they are exact on the world clock and
  testable:
  - **Rats shun light, mostly.**  A rat avoids lit cells (shade ≤ 2) when a dark way exists that
    is at most twice as long.  It crosses light when it has to: no dark way home, or cornered.
  - **Bats drift towards light, slightly.**  Among the steps a wandering bat would take anyway,
    it takes the brightest.  It is never drawn straight to a flame and never stays at one.
- **The staff** (today the cellar's light) stays a light of its own: it never burns out, and
  it hangs in a socket like a torch.

## Goals: chests and points

- **A point per chest.**  Opening a chest gives its money and one point (built: a chest's
  `money`, 10 proposed; the status line shows GOLD once there is any, and the coins are heard).  A level's goal `chests` is solved when no chest is left closed.  A region or a
  dungeon has a fixed number of chests, so the points it can give are limited.
- **A chest is never hidden.**  It may stand out of the direct route — a door behind a tree is
  a fair way to it — but never behind or beside a wall where it cannot be seen.
- **A chest is earned, never cheap** (owner, 2026-11-08): its challenge is higher than the rest
  of its area's and than the things that gate progression there (PUZZLES.md § Chests).
- **Chests and progression items have their place** (owner, 2026-11-08): each has a set
  location, part of the game's structure; only the way to it is dealt (PUZZLES.md § Chests and
  progression items have their place).
- **The signpost counts.**  Walking into a farm signpost lists how many of the region's chests
  are still unopened (built: the status line, until the next move).

## Enemies

- **Kinds.** Rats, bats and spiders.  A further kind that shares a look (a second rat) is added
  only with a behaviour of its own (owner, 2026-11-04).
  - Walking into an enemy attacks it.
  - The hero has hearts, and losing them all returns the hero to the last safe point.
- **They do not fight to the death.**
  - A hurt enemy that is not cornered retreats to its nest and heals there over time.
  - Only a cornered enemy fights on and can be finished off.
  - **Cornered** means it has no step away from the hero and no path home that avoids the hero.
- **Spiders.**
  - They climb rough walls, and they flee to another level through openings (not through a
    ceiling).
  - Their nest is never on a wall in a level as made; one that fled settles where it arrived.
- **Enemies act on the world clock**, so every behaviour is testable at exact times.
- **Built: rats** (step 1, `tests/enemies.loft`; the numbers proposed, in kinds.json `enemies`).
  - Health 3; a step every 600 ms wandering, every 350 ms chasing or fleeing (the hero steps every
    140 ms, so it can always get away); a bite costs the hero 1, at most every 900 ms; it sees 5
    cells (line of sight, its own layer); it roams within 3 of its nest; it heals a point every
    4 s there (ruled again, owner 2026-11-07: about a point a minute, not a quick reset — a wounded
    enemy is off the road for a while and can be found wounded; TEMPLATES.md § Living there).
  - A healthy rat that sees the hero comes at it by the shortest way and bites when beside it;
    one that does not wanders, its steps chosen by its id and how far it has walked (the same
    every time).
  - Hurt, it goes home by a way that passes nowhere beside the hero, even when home lies towards
    the hero; with no such way, it steps away from the hero; with no step away either — cornered —
    it fights.
  - The hero strikes it by walking into it or swinging at it: a sword or an axe costs it 2, bare
    hands or anything else 1.  A blast strikes it 4 - d.  At no health it is gone.  Rats block
    rolling bombs as any obstacle does.
  - Level files list them (`enemies`, each at its nest); the game draws them as figures (§ Figures):
    a step hobbles, a blow staggers it, a bite lunges at the hero.
- **Caught in light, a rat gets out of it**: in a torch's light with a dark cell within 4 steps,
  it walks to the dark (proposed; a rat lit by daylight, with no dark near, carries on).
- **Built: step 2** (`tests/enemies.loft`; numbers and the marked choices proposed).
  - **Rats keep to the dark.**  A rat standing in the dark takes a dark way (no cell at shade 2 or
    less) when one is at most twice as long; it does not come into light to chase the hero, so a
    torch keeps rats off.  Going home hurt it crosses light when it has to.  A rat already in light
    (daylight, a torch beside it) has nothing to shun.
  - **Bats** (health 1, quick): they fly over water; wandering, they take the brightest step,
    never back where they just came from and never onto a flame — so they drift towards light.
  - **Spiders** (health 2): they climb rough walls (walls are rough); on a wall a sword cannot reach
    them (the bow will).  **Built: fleeing to another layer** (`tests/enemies.loft`, proposed as
    built): hurt or scared, with the hero on its layer, a spider climbs the wall's face up onto
    what is over the wall, or down from a floor onto the rough wall under it — only through an
    opening: a cell in front of the face that is no wall and lies under the open sky (the world's
    definition: no layer set over it).  It settles where it arrives, its nest now (on a wall face
    too, out of the sword's reach), and heals there.
  - **A blast scares** every ordinary enemy within 8 cells on its layer: it flees the place for
    5 s, as far as it can step, then walks home.  Any enemy outside its roam walks home.
  - **Wells**: water only when no enemy is within 10 cells in sight of the hero (Step.Watched,
    heard as a bump).  Darkness does not hide an enemy yet — the owner's ruling, to try in play.
  - **Guards** stand, block (walking into one is blocked, a blow does nothing, a blast spares it),
    and step aside — the side the place prefers — when the hero is beside them and the area is
    clear; they do not count against a well.
- **Explosions scare them** (owner): an ordinary enemy that hears a blast flees, even from its
  nest, and returns to its nest after a while.  Open: how far a blast is heard, how long they stay
  away.  A fleeing enemy still in sight counts for a well's clear area (§ Guards, doors and wells).
  Cyclopes are not scared: slightly more intelligent, they go to search for what set the bomb off
  (owner) — a blast draws them to it.
- **Cyclopes: step 1 built** (`tests/cyclops.loft`; Kenney's dungeon:109; the numbers and the
  way they see are proposed, in kinds.json `enemies.cyclops`):
  - **They see by light**: a hero standing in light (shade 2 or less: daylight, a flame near, its
    own torch) within 8 cells in sight; one in the dark only within 2.  So the dark hides the
    hero and a torch gives it away — the reason to move in the dark.
  - **Seeing the hero anew, a cyclops shouts** (heard: the voice pack's "fight") and **calls** the
    others of its kind within 6 cells to the place, seen or not; it comes at the hero (a cell
    every 450 ms) and strikes for 2 (every 1200 ms), a sweep wound up for 700 ms (@GM-Combat).
    Health 8: four of a sword's blows, more than fit inside one sweep's wind-up, so a fight with
    one is an exchange, never a free kill (§ What the hero holds: a blow takes its time).
  - **Losing it, it searches** the place it last saw it, for 6 s after; there it stands looking,
    then walks home.  Called ones search the same way; one finding the place taken stands beside it.
  - **A blast draws them** (owner): within 8 cells they go to search its place instead of fleeing.
  - **Brave**: hurt, a cyclops fights on, and heals at its nest once nothing is left to search.
- **Cyclopes: step 2 built** (`tests/cyclops.loft`, `tests/levels.loft`; proposed numbers).
  What a cyclops holds is its kind's (an axe) unless its level entry names another as a fifth
  word: `["cyclops", x, y, z, "bomb"]`, or `"none"`.
  - **Armed** (the axe), it strikes for 2.
  - **Holding bombs** (3), it throws one when it sees the hero in its row or column, 5 to 8 cells
    away, the way clear for a rolling bomb — nearer, its own blast would reach it.  The bomb is
    like any other: fused, rolling at the throwing speed, heard (the hero's throw's sound), going
    off where it meets something; it pushes, hurts, chains, lights or douses torches, scares the
    ordinary enemies around.  Its last one thrown, it is unarmed.
  - **Unarmed** — or beside the hero with bombs in hand — it grabs the hero (heard) instead of
    striking, and every 800 ms steps towards its nest, the hero pulled into the cell it leaves;
    at its nest it holds on, while its group gathers.  The hero pulled into the cell the cyclops
    left goes only where the cyclops walks — never over a ledge (INTERACTIONS.md 30, proposed).
  - **Breaking free**: 3 presses any way but into it (each a struggle, heard as a bump); the third
    takes the step, and the cyclops staggers, acting again a blow's time later.  Walking into it
    strikes it instead.
  - They search for what set a bomb off (above).
  - **A bested enemy leaves what it held in sight** (owner: enemies are not bested for loot, but
    what is seen is there; built, `tests/cyclops.loft`): a cyclops its axe, a bomber the one bomb
    in its hand (lying, unlit), a hunter its bow; the others nothing.  It falls where the enemy
    stood, or beside it when an item lies there (as a tree's stick), lost with no room.  A blast
    that bests a bomber sets off the bomb it drops — damage sets bombs off.
- **The hunter: built** (`tests/hunter.loft`; Kenney's dungeon:112 with the bow; proposed: health
  3, sees as a cyclops does — a lit hero within 8, in the dark within 2 — and shoots every
  1500 ms).  It shoots what it sees in its row or column with the arrow's way clear (nothing an
  arrow stops at before it, within the arrow's range): the hero first; else it hunts the nearest
  other enemy it sees, shooting it in line or going after it at its chasing pace.  Its arrows
  fly as the hero's (30 ms a cell, real time): a point off the hero they meet, a point off an
  enemy, a bomb set off; a lever stops one unthrown.  Hurt it flees, a blast scares it.
- **The hunter** (owner).  A hunter with a grudge against the village, not evil (owner,
  2026-11-04; STORY.md @ST-Hunter), shoots arrows as the hero does, and follows the
  ordinary enemies' rules (fleeing hurt, healing at its nest, scared by blasts).  It throws no
  bombs: it does not want to lose its bow.  Its arrows are arrows: they set off a bomb they hit,
  and they hurt other enemies too, but throw no levers (owner).  While wandering it hunts other enemies, but it prefers
  the hero: seen, the hero is its target.
- **Cyclopes and light.**  Light is double-edged: a burning torch shows the hero to them (step 1,
  above).  Still open: whether a cyclops's own sight counts for a well's clear area in the dark
  (INTERACTIONS.md 23) — for now any enemy in line of sight within 10 does.

## Guards, doors and wells

- **The clear area.** Guards and wells share one rule: no enemy is within 10 cells in sight. A
  fleeing enemy that is still in sight counts.
- **Guards.** A guard blocks a door by standing in front of it. It steps aside when the hero
  approaches and the area is clear.
- **Wells and safe points.** Drinking from a well saves the whole situation.  In the story game the
  safe points are strange fountains in the mines and the hills, whose liquid turns to mud when taken
  and whose secret the alchemist cannot find (owner, 2026-11-04; STORY.md @ST-Fountains).
  - A well gives water only when the area is clear.
  - A key returns to the saved situation, after a confirmation (B, then B again).
  - Losing all health returns the hero to the last safe point; with none, R restarts.
  - Built (slice 2): the well (walked into, it saves; the hero stays put), the return on no
    health, B twice.  The clear-area condition waits for the enemies: until then the area is
    always clear.
  - This is what makes the unsolvable states a wrong push or a bomb can create acceptable.
  - **A doomed save is allowed** (owner): a well saves whatever is about to happen — a cart
    rolling at the hero, a last flame's final seconds.  It never bricks the game: just sent back
    to a save, B twice goes one well further back, which becomes the save, and no further.  Built
    (`checks/game.loft`).

## What the player sees

- **The way the hero faces is seen** (owner, 2026-11-07; built, `game.loft` `draw_figure`): the
  way it last stepped, where Q acts.  Facing east or west it leans FACE_TILT degrees (6) that way,
  its picture mirrored facing west; facing north it shows its back, the game's own picture
  (`world:96`, ART.md); facing south its front.  It stays centred on its cell (owner: moving it off
  the centre does not help).  The hero only: enemies keep their pictures (owner).
- **Looking down through an opening** (owner, 2026-11-07; built, `game::look_down`,
  `checks/game.loft`).  From an opening in its floor the hero sees the layer below under the
  opening and **a bit into what lies past it** (owner): DOWN_LOOK cells (1) from it, as far as a
  sight line from the opening goes, the rock of the hero's own layer over that cut away — from the
  room at the top of a slope, the slope and a cell of the room below.  **Only standing in the
  opening does it see everything** (owner): the whole area joined to where it stands, within the
  view, the rock of the layer above over it left out (that layer's floors stay).
- **What happens is shown by the world** (built, proposed): combat in § @GM-Combat's *How it is
  seen*; a door shut until something holds (the barn) is drawn closed; a person who starts or
  stops following the hero nods (a hop in place).
- **Almost nothing on the screen** (owner, 2026-11-05).  The story game has a minimal UI: no time,
  seconds, moves or gold on screen, and most of the rest taken off too — how is still to be worked
  out.  The demo world keeps its status line as a test bench.  So a mechanic is presented by the
  world itself (a figure's pose, what it holds or wears, the light, what people say), never by a
  readout; a mechanic only a readout shows is not yet presented.
- **The hero's position decides what is drawn**, never what is on screen.
- **Up.**
  - In a low corridor (under a ceiling) only the hero's own layer is drawn, even beside a taller
    room.
  - Stepping into a room with no ceiling draws the layer above it as well. The view itself tells
    the player they stand in a higher room.
- **Down.**
  - Standing one cell back from a ledge, the drop shows as plain dark.
  - Standing beside a pit, the layers below are drawn, darker with each level, up to about 4 deep.
- **Drawing.** It uses the `tiles` library's shaded palettes (100 / 75 / 50 / 35 %).

### Figures: how characters move (owner, built: `motion.loft`, `tests/motion.loft`)

The hero and every enemy are **figures**: cut-outs, as South Park animates them. The body is one
picture that glides, hops and rocks; a weapon is a second picture, held at the hand, swinging a
little on its own. No frames are drawn for it: the motion is all there is. It is drawing only;
the rules stay cell by cell, and it runs on the world clock, so it is the same at any frame rate.

- **Walking.** A step glides to the next cell in 140 ms (no longer than a held key's repeat),
  hopping 2 px and rocking 4° about the feet, one way on one step and the other way on the next:
  the hobble. A step taken mid-step glides on from where the body was drawn.
- **Standing.** After a step the figure stands still (owner): no idle bob.
- **The weapon leads.** Held at the hand, it points the way the figure faces (up, right, down,
  left) and trails the rock by a quarter step. Facing up it is behind the body. It is the
  figure's only sign of direction (owner): the body faces the viewer. A carried thing that is
  no weapon (a torch, a stick) is held as before.
- **Facing** is the way the figure last tried to walk, a wall included; pulling, it faces what
  it pulls (`Level.face_x`, `face_y`).
- **The swing** (J, or Space): a wind-up 70° behind the facing, a strike through to 70° beyond with a
  2 px lunge, and the recovery, 240 ms. It strikes the cell faced (`rules::swing`): an axe chops a
  tree there as walking into it does; a weapon strikes the enemies in its reach (@GM-Combat).
- **The stagger** (owner): walking into something that does not give way knocks the figure
  3 px back and lets it wobble back, 280 ms, its weapon flung back. A hit (a rolling cart) is
  the same stagger, and a figure the hit moves (shoved aside) glides there while it staggers.
- **Depth** (owner): figures can stand slightly behind what is in front of them. The ground is
  drawn first; then, row by row from the back, the upright terrain (`"upright"` in theme.json:
  walls, trees, fences, doors, wells), the stacks, and the figures whose feet stand at that
  row. So a chest raised on a table in the row in front hides the hero's feet, and a figure
  stepping into the row behind stays behind that row's things until its feet pass them.
- **The engine** turns pictures with `tiles::draw_turned` (tiles 0.5.0): every pixel kept at any
  angle, so a 4° rock does not smear the pixel art.

### The ground: slopes bent to fit, light that fades (owner, built: `game.loft` § the ground)

- **Slopes.** Every ground cell is drawn with `tiles::draw_ground` (tiles 0.4.0), bent to its four
  corners: a corner is lifted 4 px a step by the mean rise of the ground cells sharing it that
  are within a step of the cell, so a slope bends smoothly and a rail on it stays joined; a top
  corner within a step of the ground north of it is that cell's bottom corner, so the two meet
  exactly. A drop of two steps or more leaves a bank under the higher cell, its ground darkened
  two shades. Things, upright terrain and figures stand at their cell's rise (the hero riding a
  cart on a slope sits on it).
- **Light fades.** A corner's light is the mean of the ground cells sharing it (a standing
  cell's, of every set cell), so light fades across the ground and up the walls, dithered
  between the shades, instead of in squares; not through a wall into the dark beyond.
- **Shape by light.** The light falls from the top left, as in the pictures: ground falling away
  from it (its top-left corner higher than its bottom-right) is a shade darker for a step of
  slope, two at most, so a mound reads as one.
- **A layer is a wall high** (owner: the wall pictures set it).  Kenney's raised blocks are a
  row of wall tops over exactly one row of wall face (tile 40), so a layer stands one tile — 16 px
  — above the one below, and a step of the rules (4 a layer) is 4 px: the slopes already rise at
  that pitch.  Every layer is drawn its number of tiles up from layer 0, so the world stays still
  when the hero changes layer, and a step onto another layer glides like any other.  The ground's
  corners look across a layer's edge (a cell of the layer above is 4 steps up, of the one below 4
  down), so a slope's top meets the floor above without a step, and a ledge's edge has its bank
  down to the floor below.  The view follows the hero's row less its layer (a layer is a row up).
- **Behind things** (owner, built: `checks/game.loft`, `tests/sight.loft`).  A figure may be
  partly or wholly hidden behind what stands in front of it — a door, a pillar, a fountain, a
  tree, a smaller thing — and a level's maker keeps the hero from being hidden for long.  Only a
  wall in front of the hero is left out of the picture: the ledge's top on a wall, drawn a row up
  over a hero just behind it.
- **What the hero does not see is not drawn** (owner): on the hero's layer, the things and figures
  past a wall from the hero — a straight line from its cell that a wall cell or a wall edge stops
  (`rules::in_view`; two walls touching at their corners stop it too).  Only walls: a door, a
  tree, a thing never hides anything, so nothing pops in and out of view as things move.  The
  terrain is always drawn, and so is the light a hidden flame casts.
- **One sight line, both ways** (owner, 2026-11-06, built: `rules::hero_line`,
  `tests/cyclops.loft`).  An enemy, a person, the clear area of a well or a guard see the hero
  only along the hero's own line: the one the picture is drawn by, traced from the hero, with
  anything opaque on it (a closed door) stopping it too.  So an enemy that spots the hero is on
  the screen, and one the screen hides — behind two pillars touching at a corner — does not spot
  it.  Their own limits come on top: how far they see, in light or in the dark.  Measured over a
  13 x 13 room with every placement of one wall and of two walls touching at a corner (75 200
  cases), the two answers now agree everywhere; before, an enemy saw an unseen hero in 604 of
  them (the corner gap, and a line traced from the other end) and was unseen-yet-seen-by-the-hero
  in 240.  An enemy looking at another enemy (a hunter's prey) keeps its own line.  Whether the
  line itself should be tuned (for hero and enemies alike) is for play to tell.

### Slanted rock: walls at 45 degrees (owner, built: `intent.loft`, `theme.loft`, `tests/slants.loft`)

- **A picture only** (owner).  A slanted cell is its kind for every rule: rock is not walked into,
  stops sight and light, and holds up what stands on it from the layer above — a small ledge.  The
  slant only says how it is drawn: one picture, cut along a diagonal, in one of four ways — which
  corner its open half looks into (north-east, south-east, south-west, north-west).
- **Kept in the world.**  `tile_world` (0.8.0) holds a cell's slant beside its edges and its
  rise, so saves, stamps and copy and paste carry it; a place file lists its slanted cells in
  `slants`.
- **Derived where rock is natural.**  A cave's walls on every layer it climbs, and the rim of a
  blob or a tunnel, are cut where they turn a corner: a rock cell whose two neighbours on one
  corner's side are inside the shape, and whose other two are not, looks into that corner.  A
  round cave reads round instead of as stairs, a tunnel's bend is cut on its inside, a pillar
  becomes a diamond.  Built walls — a room's, a corridor's, a house's — stay whole.  A slant is a
  derived detail like any other: `/` in the editor turns a cell to the next way (Shift+`/` back),
  that cell is then set by hand, and a fill keeps it until the adjustment is taken away.
- **Drawn from the pictures there are.**  When the theme loads, every picture an upright kind can
  show — its look's, and its materials', autotile pieces included — is cut four ways: the open
  half made clear, the cut drawn in the picture's darkest colour.  No new art.  Under the open
  half the ground of the neighbour on that side is drawn; on a layer above, with no ground there,
  the half stays open onto what lies below.
- **Only rock is cut.**  The rule cuts rock, never floor, so a corner where the floor bulges out
  into the rock stays square.
- **Sight at a cut corner.**  Two walls touching only at a corner stop the hero's line (§ What the
  player sees, one sight line both ways), so a 45-degree wall one cell thick hides what is behind
  it, from the hero and from the enemies alike.

### A square test bed for dryopea's walls (owner, 2026-11-06: agreed, not built)

The 16 x 16 tile model can test dryopea's rules without its 3-D engine.  Its one geometric hurdle is
dryopea's *a curved wall is stronger than a straight one*; with slanted cells the square grid can
draw a wall in all eight directions.  The agreed rule: a wall cell is CURVED when the wall's
direction two cells back differs from its direction two cells ahead, and it is stronger seen from
the outside of the bend (from the inside it counts as straight, never weaker).  Measured on the
two-step check: a 3-wide side between two 45-degree cells counts curved all along (the octagon);
circles of radius 3, 5 and 8 cells count 100 %, 85 % and 72 % of their cells curved, radius 12
about half; a straight wall, square or at 45 degrees, none.

## Slices

| slice | content | state |
|---|---|---|
| 1 | layers and chunks, walls, rails, carts and trains, rocks, chairs and tables, potion, demo level | shipped (0.1.0) |
| 2 | 4-step layers and falls, ceilings, fence, tree, one held item (sword, axe), chests and the signpost, enemies that flee and heal, guards and doors, wells and safe points | in progress: 4-step layers, slopes, falls and health, ceilings, fence (a cell or an edge), tree, doors that open, stairs, the held item (full hands, drop, the axe), chests and the signpost, wells and safe points built |
| 3 | bow and aim, bomb, the depth renderer with Kenney tiles | open |
| — | torches and light: stick, tar, sockets, burning out; the game's own sheet (ART.md); light in the enemies' moves | built: the tiles, the rules and `the torch room`; light in the enemies' moves waits for the enemies |
| later | growth: carried weapons, armour, stats — never required (the principle above) | open |
