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
- **Growth comes later.** Character growth comes only after a base dungeon and world feel alive and
  are satisfying to beat. Growth means carried weapons, worn armour and stat increases.
- **Growth is never necessary.** Correct puzzle solving and fighting strategy must always let the
  base character beat every dungeon. It may get increasingly harder without upgrades.
- **Nothing is tutorialized** (owner, 2026-10-30).  A dungeon is presented in its natural state;
  no level teaches a rule, no hint explains one.  The player learns the world as in Spelunky: by
  watching it and by dying in it.  So every rule must hold everywhere, the same way every time —
  consistency is the teacher — and what is about to happen must be readable from the world itself
  (a sound, a sight), never from a message.
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
  A blast pushes the hero over a ledge, and the fall's cost adds to the blast's (owner): a rule
  that stays, and one the level designer keeps in mind — a bomb, or a cart at speed, beside a
  ledge is a level's choice to make deadly or not.
- **Real time** (owner, 2026-10-30).  The world runs on its clock whatever the hero does: carts
  roll, blasts go off, torches burn while the player thinks.  It stays deterministic — the same
  inputs at the same times, the same outcome — so a level's solution is still a test.  Danger
  must be readable before it arrives (proposed): a rolling cart is heard before it is seen, a
  chain of bombs ripples (each one 150 ms after the blast that set it off) rather than all at
  once.  A doomed save is allowed — it is fun (owner) — but never bricks a game: see the wells.
- **A test enforces it.** Every shipped level carries a solution test played by the base character
  (`tests/first_climb.loft` is the model).

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
- **Ceilings.** A cell has a ceiling when the layer above it is not void.
  - The ceiling is never drawn.
  - The hero cannot climb up through a ceiling (built, slice 2), and neither can a spider.
- **Terrain kinds.**
  - Floor and rails.
  - Water: blocks walking, but bats fly over it.
  - Wall: rough walls are climbable by spiders; smooth walls (optional) are not.
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
  - A green potion gives +2 to both for 60 s of the world clock. It is used by walking over it
    (slice 1).
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
  ground within a step of the cart's base, on a slope as well.  Not built: pulling a cart over a
  layer's edge.  The editor paints junctions ("junction 1" .. "junction 8", each as drawn set
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

- **One item, no inventory.** The hero holds at most one item, and it is always on screen.
  - Walking over an item swaps it with the one held.
  - `drop` (X) puts the held item down.
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
  - **Built** (`tests/bow.loft`; the numbers proposed).  Space shoots (at the locked target, else
    the way the hero faces, turning it there); `T` locks on the next target.  An arrow flies
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
    speed every 4 cells (`bomb_coast`).  Thrown (Space, the way the hero faces) it starts at speed
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
  held-item rules.  Walking over one swaps it with the one held, X drops it, and holding one
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
- **The signpost counts.**  Walking into a farm signpost lists how many of the region's chests
  are still unopened (built: the status line, until the next move).

## Enemies

- **Kinds.** Rats, bats and spiders.
  - Walking into an enemy attacks it.
  - The hero has hearts, and losing them all returns the hero to the last safe point.
- **They do not fight to the death.**
  - A hurt enemy that is not cornered retreats to its nest and heals there over time.
  - Only a cornered enemy fights on and can be finished off.
  - **Cornered** means it has no step away from the hero and no path home that avoids the hero.
- **Spiders.**
  - They climb rough walls, and they flee to another level through openings (not through a
    ceiling).
  - Their nest is never on a wall.
- **Enemies act on the world clock**, so every behaviour is testable at exact times.
- **Built: rats** (step 1, `tests/enemies.loft`; the numbers proposed, in kinds.json `enemies`).
  - Health 3; a step every 600 ms wandering, every 350 ms chasing or fleeing (the hero steps every
    140 ms, so it can always get away); a bite costs the hero 1, at most every 900 ms; it sees 5
    cells (line of sight, its own layer); it roams within 3 of its nest; it heals a point every
    4 s there.
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
- **Built: step 2** (`tests/enemies.loft`; numbers and the marked choices proposed).
  - **Rats keep to the dark.**  A rat standing in the dark takes a dark way (no cell at shade 2 or
    less) when one is at most twice as long; it does not come into light to chase the hero, so a
    torch keeps rats off.  Going home hurt it crosses light when it has to.  A rat already in light
    (daylight, a torch beside it) has nothing to shun.
  - **Bats** (health 1, quick): they fly over water; wandering, they take the brightest step,
    never back where they just came from and never onto a flame — so they drift towards light.
  - **Spiders** (health 2): they climb rough walls (walls are rough); on a wall a sword cannot reach
    them (the bow will).  Not built: fleeing to another level through openings.
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
- **Cyclopes** (owner, planned).  Hard to fight and living in groups; the way past them is stealth.
  - They use weapons, and throw bombs at the hero — bombs like any other: they push, hurt, chain,
    light or douse torches, and scare the ordinary enemies around.
  - Without a weapon a cyclops pulls the hero towards its own nest; the hero breaks out of the pull
    with some effort (open: how much — proposed, a few presses away from the cyclops).
  - They search for what set a bomb off (above).
- **The hunter** (owner, planned).  An evil hunter shoots arrows as the hero does, and follows the
  ordinary enemies' rules (fleeing hurt, healing at its nest, scared by blasts).  It throws no
  bombs: it does not want to lose its bow.  Its arrows are arrows: they set off a bomb they hit,
  and they hurt other enemies too, but throw no levers (owner).  While wandering it hunts other enemies, but it prefers
  the hero: seen, the hero is its target.
- **Cyclopes (planned).**  Hard to fight and living in groups, like the other monsters, so the
  way past them is stealth: sneaking by in the dark.  They are the reason to move in the dark,
  and they make light double-edged: a burning torch shows the hero to them.  How a cyclops
  sees (light on the hero, distance, sight lines) is to be designed with them.

## Guards, doors and wells

- **The clear area.** Guards and wells share one rule: no enemy is within 10 cells in sight. A
  fleeing enemy that is still in sight counts.
- **Guards.** A guard blocks a door by standing in front of it. It steps aside when the hero
  approaches and the area is clear.
- **Wells and safe points.** Drinking from a well saves the whole situation.
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
- **The swing** (Space): a wind-up 70° behind the facing, a strike through to 70° beyond with a
  2 px lunge, and the recovery, 240 ms. It strikes the cell faced (`rules::swing`): an axe chops a
  tree there as walking into it does; a sword cuts the air until enemies exist.
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
  Open: a figure on a lower layer just behind a ledge is hidden by the ledge drawn after it.

## Slices

| slice | content | state |
|---|---|---|
| 1 | layers and chunks, walls, rails, carts and trains, rocks, chairs and tables, potion, demo level | shipped (0.1.0) |
| 2 | 4-step layers and falls, ceilings, fence, tree, one held item (sword, axe), chests and the signpost, enemies that flee and heal, guards and doors, wells and safe points | in progress: 4-step layers, slopes, falls and health, ceilings, fence (a cell or an edge), tree, doors that open, stairs, the held item (full hands, drop, the axe), chests and the signpost, wells and safe points built |
| 3 | bow and aim, bomb, the depth renderer with Kenney tiles | open |
| — | torches and light: stick, tar, sockets, burning out; the game's own sheet (ART.md); light in the enemies' moves | built: the tiles, the rules and `the torch room`; light in the enemies' moves waits for the enemies |
| later | growth: carried weapons, armour, stats — never required (the principle above) | open |
