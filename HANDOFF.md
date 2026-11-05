# Handoff — the tile_world game, its libraries, and the loft work around them

State on 2026-10-29 (evening), for picking the work up in a fresh session.  Paths are on the owner's
machine (`~/workspace`).

## Where things are

| What | Where | State |
|---|---|---|
| The game | `~/workspace/tile-world-game`, hosted at [loft-lang/tile-world-game](https://github.com/loft-lang/tile-world-game) (public, `origin`) | `main` is the game; push there |
| Game libraries | `./fetch-libs.sh` → `libs/` (git-ignored) | every library from `main` |
| loft branch | `~/workspace/loft2`, branch `tuxedo-pln204-int32` @ `72602f4b6` | = `main` (ec098768e) + one commit, the loft#2222 fix; force-pushed, no PR yet.  Use its RELEASE build (`target/release/loft`) for the game: an older loft stalls in `parse` on `input` → `graphics` → `mesh3d` |

## Library PRs

| PR | What |
|---|---|
| loft-libs-graphics#66 (merged) | tiles 0.4.0: `draw_ground` — a tile bent to its four corner lifts and lit by four corner shades, dithered |
| loft-libs-graphics#68 (merged) | tiles 0.5.0: `draw_turned` — a tile turned about a pixel, every pixel kept (the figures' lean and swing) |
| loft-libs-graphics#69 (merged) | tiles 0.5.1: `categories`, which the registry requires of a new package.  **Published** 2026-10-30 (registry 53cdc19, signed with the local key on the owner's go); the GitHub release `tiles-v0.5.0` exists but never reached the index |
| loft-libs-world#40 (merged) | tile_world 0.6.0: `shades` takes a start shade per light |
| loft-libs-graphics#61 | imaging 0.5.0 (lodepng, a `[c]` shim) |
| loft-libs-graphics#62, #63 | tiles 0.1.0, tiles 0.2.0 (8 shades) |
| loft-libs-world#37, #38 | tile_world 0.4.0 (edges, ground, rise), 0.4.1 (`ground_layers`) |
| loft-libs-graphics#65 | tiles 0.3.0 (`rect`, `frame`: the editor's cursor, goal and palette frames) |

`drawing`'s red (which blocked #61–#63) was loft#2215: a library compiled for `--interpret`
linked a remapped `#native` to the wrong function, so graphics' `save_png` answered `false`.
Another session fixed it on loft `main` (#2218); a rerun of the CI went green.

The "unreleased work" check stays red on loft-libs-graphics until imaging 0.5.0 and tiles are
published.  It is not a required check.  Publishing waits for the owner's go.

## Open loft issues of this work

- **loft#2222** (fixed-pending-merge, on `tuxedo-pln204-int32` @ `72602f4b6`): the value-record
  pass took time cubic in a function's record temporaries, so mesh3d's `cube()` never compiled
  and any program using `graphics` with its libraries compiled could stall in `parse`.  Verified:
  a side-by-side cross-check of old and new answers over 2903 corpus programs (59611 questions,
  no disagreement), `tests/value_record.rs`, `make rewrite-census`, both clippy legs.  Guard:
  `tests/scripts/a-function-of-many-record-temporaries-compiles-in-seconds.loft`.  Not run:
  the full `make ci` gate.  It reaches `main` only through a PR, which the owner opens.
- **loft#2219** (fixed-pending-merge, by another session on `tuxedo-pr2218-next`):
  `x = f() ?? return;` in a loop corrupts a returned struct's nested hash from the second
  iteration.  Until it reaches `main`, the game unwraps with `if !m { return; }` first.
- loft#2215 and loft#2216: on `main` (#2218).

## The game, as it stands

- **The split.** `tile_world` (library) holds the world and answers questions: paths, sight,
  light.  Everything a piece *does* is the game's: `rules.loft` reads only the properties in
  `kinds.json`, never a kind's name.  The look is `theme.json` through `theme.loft`.  Levels are
  `levels/*.json` through `levels.loft`, played in `levels/order.json`'s order.
- **Runs** on a desktop window (`screen`, X11) with Kenney credits first, sound through `audio`,
  keys through `screen`/`input`.  128 tests, on the interpreter and `--native`:
  `loft --lib libs --tests tests` (the runner takes one target: a file or the directory).
- **Figures** (`motion.loft`, DESIGN.md § Figures): the hero is a cut-out that hobbles, holds
  what it carries the way it moves, swings (J) and staggers; drawn in depth order over the
  ground.  Verified: `tests/motion.loft` (every number by hand) and the facing and swing tests in
  `tests/moves.loft` on both backends; eight mutations (hop, trailing, sweep, knock, chaining,
  rock, facing, swinging without a weapon) each fail their test.  `tools/anim.loft OUT_DIR`
  films a scene through the game's own drawing.  Pictures:
  `TW_SHOT=f.png [TW_LEVEL=1] [TW_MOVES="..."] [TW_CREDITS=1] loft --native --lib libs main.loft`.
- **Design docs:** `DESIGN.md` (the game: principle, rules agreed, slices) and `EDITOR.md` (the
  in-game editor: model, file format, places, keys, checks, platforms, order of work).

## State 2026-11-05

345 tests (`loft --lib libs --tests tests`) and 29 checks (`loft --lib libs checks/game.loft`),
on the interpreter and `--native`.  Every mechanic of BUILD.md's phases A–C is built, each with a
house in the demo village; still open there: WORLD.md step 3's regions, bands and coverage table,
and a tribe acting across places (C3, C4).  The next phase is D, the story's places.

- **Places and doors** (WORLD.md step 1): the game starts in the demo village
  (`levels/demo.json`), a house per mechanic room, doors both ways, every place kept as left,
  floor text (`labels`), R = the place as entered.  `TW_WORLD` picks the start place; the demo
  stays its own world beside the story game to come (owner).  What follows the hero goes through a
  door with it: a person led, a pulled thing, an enemy on its heels (C3c).  A door can `need` a
  fact (SHUT until it holds) or `end` the game (the epilogue, C7).  Doors, text and the tags a
  place serves are set in the editor's panel (A2, A3).
- **What is known and people's days** (C1, C2, C3): one clock for the world; facts carried
  through doors, lines that set and clear them, barks on conditions, goodwill; a person's points
  by the clock (`routes`), and the world's people (`levels/people.json`) going from place to
  place through the door straight to the next.  Not yet: WORLD.md step 3's regions, bands and coverage table.
- **Saves** (C6): a well or fountain writes the game to `saves/`; `main.loft` reads it back
  unless `TW_NEW` is set.
- **A loft defect worked around:** loft#2404 (a struct held in a vector field, declared BELOW
  its holder, turns an `x[i]!.f` temporary into an owner that frees the holder).  Reading a save
  crashed on it; `Visited`, `Mover` and the other element structs of `Game` are declared above
  `Game` in `game.loft`.  Keep new element structs above their holder until the fix
  (fixed-pending-merge) reaches loft main.
- **The engine's limits met:** tile_world keeps sixteen terrain kinds (four bits) — the door held
  shut is one kind, its beam's side kept in the level's `bars`; one id is left (15).
- **Talk** (`talk.loft`, WORLD.md § Talking is moving): beside = talk, passing = bark; stay /
  bump / walk away while a line is said; gifts, take/give, `then`.
- **Combat** (DESIGN.md @GM-Combat, BUILD.md B1b): built — wind-ups, dodge, block, parry and
  riposte, heavy, lunge, side blow, poise, the light chain, the early-swing back-off; ties follow
  the place against the hero (`rules::acts_before`), never a level's list order.  Keys: J/K/L/I
  with WASD, Space/Shift/X/T with the arrows.
  Open: every number is proposed, to tune in play (owner); the hunter's hall is not played by the
  scripted players (the hunter keeps its distance and shoots); the dodger's first blow is no later
  than the attacker's (both open with a lunge on an enemy not yet winding up) — only the
  parrier's is.
- **Seven new houses** in the demo village's east end (BUILD.md A0b, B2, B2b, B2c, B7b, C2b, C3b,
  C6b; the open questions answered as proposals there), each to try by hand:
  - FOUNTAIN: walk into the green basin — SAVED; the rats beyond bite to nothing — BACK AT FOUNTAIN.
  - CAVE-IN: take the pick, strike the rock with the cart behind; push the full cart to the niche
    and bump it from above (BUMP); the two full carts are too heavy until one is tipped; take the
    standing beam out; take the barricade apart, or light the barrel with the far torch and push it.
  - HERBS: wear the sack, forage (SACK n/6), bump the alchemist — 2 herbs and a mushroom, a
    potion; the rope stays lying; the rat there leaves meat when ended, the farmer takes it.
  - NIGHT: in the dark street go into the house — nothing; with the torch — the neighbour saw it.
  - SABLE: his day (dig, then home, a 60 s demo day), his two rocks in the short way, and the cart
    held by a rock on the terrace — push the rock off and step away at once, or be run down.
  - DOME: a wide dome (31 by 23, raised to 5 layers) with a crag's gallery joined to its south-east,
    one cave; stairs up beside the way in, two layers, and a plank bridge across to a chest.
    `tools/stable.loft the_dome` says it holds.  (After an intent change that removes cells, draw a
    place again from empty: the fill tool never clears a cell.)
  - BLASTING: lift the beam of the way in; in the chamber put a bomb down, push five stones round
    it (behind and beside, open ahead), take the torch and walk into the back stone: the north wall
    cracks into a covered tunnel (a chest); three cracks side by side and its middle caves in, again
    and again; a dyke facing the east door blows it open (a chest).
  - PUMP: take the spent crystal out of the pump, put the spare in — four cells drain, the rail
    beneath comes up; the spent crystal on the rod is charged at the next strike (a 20 s storm),
    and drains twelve more; the alchemist fills the small crystal for the well pump (a large one
    there goes off like a bomb).
  - THE LIFT (rebuilt with a shaft): pull the cart up the spur onto the platform, stand on it,
    throw the lever beside it — up a layer together; the gallery's lever sends it back down.
  - THE LAKE: three beams from the shore into the water north of the jetty, the rope on the
    shoulders, walk into them — fastened; stand on the raft, walk to its east end and on into the
    water — it drifts to the island, the chest there.
  - THE CLOCK: the time on the status line; the bed beside the village's start (SLEEP) sleeps till
    20:00 — night: the street dark but for flames — and again till 07:00.  The game needs
    tile_world 0.7.0 (loft-libs-world#41): `./fetch-libs.sh` once it is merged.
  - CREATURES: the grey rat going for the herbs and carrying them to its nest; the crab along the
    pool's edge; the slime swallowing what it meets; bigfoot inside his moat; sleep (the bed) till
    night and the ghost is there.
  - SMITHY: take the burning torch from its socket, light the forge; the blunt sword struck at the
    anvil (J) is a sword; open the four old chests (PIECES 4), talk to the alchemist (the lore),
    take the lake's essence to the anvil: the artefact.
  - WORDS: Urgha's line in part; the scholar (east) teaches their words, then Urgha is heard whole;
    bump a wall three times, and the keeper's talk starts with a hint.  The night street's
    neighbour barks HMPF. SNOOPER. once the hero is disliked (seen snooping).
  - FOLLOW: talk to the wanderer — it follows; pull the chair to the east door and go through:
    the chair, the wanderer (and the rat, if it is on your heels) come along; talk again there —
    it stays.
  - WORK: the worker in the street by the work house (half a minute), then off to work through its
    door (the other half), found there inside; Lark inside guides the hero to the chest, waiting
    when it falls behind.
  - FARM: the barn (east) SHUT; talk to the horse — it follows; Dunn's offer: bump him while he
    makes it — struck, the horse stays and the barn opens (a bed); walk away — asked again.
  - RAID: a villager with an axe closes the gap with the pile's beams (a cell every 6 s); lead
    the other one there and it goes twice as fast.  The Kharn (west) eat a prey every 20 s; at one
    left one scouts, at none (80 s) all come — the barricade turns them back and they leave.  End
    a rat on their side and they come sooner.
  - Saves: drink at a well or fountain (SAVED), quit, start again — play goes on from there;
    `TW_NEW=1` starts afresh.
  - LEAVE (the gap in the village's east edge): walk out and the epilogue's stills follow from
    what is known — the bargain, snooping, being liked, Lark's path — then THE END.
  - The editor (E): figures after the things in the palette; the panel's DOOR TO, TEXT and
    SERVES (EDITOR.md § 4).
  Screenshots: `TW_WORLD=<place> TW_SHOT=out.png TW_MOVES="w ~300 a ~300"` — `~N` lets the glide
  of each step finish (without it the hero is drawn where the moves began).
  `tests/places.loft` plays each; `checks/game.loft` through the game.
- **Potions** (owner): picked up empty-handed, drunk walked over with full hands or with J; heal
  over time, a point every 1500 ms (health to 6, spirit above), carried through doors; strength
  stays instant.  A health potion by the cyclops den's door.  `tests/potions.loft`,
  `tests/fights.loft`, `checks/game.loft` (the keys, a door).
- loft defect met: an unwrapped vector element freeing its owner (`repro/loft-element-frees-owner`),
  worked around; not yet reduced without the game's libraries, not filed.
- The story game: DESIGN FIRST, no implementation yet.  The story is the OWNER's: STORY.md holds
  only what the owner said (§ 1–4), facts about what exists (§ 5) and engine notes kept apart and
  not leading (§ 6).  It is an outline (a game first, the world its backdrop; details follow from
  what the player does).  Outlined 2026-11-04: the decrepit mining town and its mine, the decree
  that stopped the mining, the hero (a sergeant who failed as an officer, lying low), villagers as
  hints for a stuck player, no fixed order, the monsters as residents, the hunter (cast out,
  befriendable, never the village's side), the cyclopes and the village's debate, the climax (an
  invasion, which a well-defended village turns back) and the best ending (they leave); then the
  farm, the decree's form (the lord fallen, a weak ownership dispute, neutral federal guards), the
  stealth puzzles, the other adventurers in a low-magic world, the alchemist.  Owner: the outline
  is enough; what stays open (the other endings, the hero's
  outcome, the adventurers' aims) is answered by building.  Nothing waiting on the owner in the
  outline itself.
  Later the same day (some of it from the owner's cloud session, branch ccr-0cdce633-v9vy6v,
  joined into main as aaa55be): the court — the young local lord (fell from grace over a princess;
  the hero served in his army), Lady Brigid the witch (far forest lands), the elderly lord of the
  mountains next door (sent the wizard, his adviser) — the magical materials deep in the mine, three
  ownership endings, the hero riding on at the end (a hidden ending where he settles), the younger
  villager's hidden path.  Names chosen (owner): Bram Holt (the hero), King Theobald of Aldmere,
  the region Sunmarch, the village Orbury, Lord Edric of Sunmarch, Princess Isolde, Lady Brigid of
  Greywood, Lord Garrick of Highcrag, Master Odran (the wizard), Hollis (the alchemist), Rennick
  (the hunter), Lark, the Fenns (fled), the Kharn (the cyclopes), the King's Wardens, the Old Seam
  (its flooded lower tunnels have no name), the Still Lake, Stormcap, the High Diggings.
  Then the people of Orbury (the Dunns of Thornfield with Lark, Garth the old foreman, Wick the
  blaster, Agnes, Sergeant Voss), Sable the rival, the Kharn's language and Urgha, the monsters'
  homes (slimes, crabs, bigfoot on Stormcap, the founder's ghost), the places validated (the
  Wardens on the main path; everyone else has a way in of their own; the hidden path to the cave-in;
  three tunnel levels; the lift to all of them; the pick, the cart and the unloading places).
  Then: TALKS.md (what people talk about); the court reaches the hero through Agnes and Voss;
  Tamsin; the artefact (pieces in chests, a key; crafted by the hero, taught by Hollis, at the
  smithy); the foreign mage behind the rumours; the attack when the Kharn's prey runs out; well
  defended = capable people behind a built barricade; the ending given after the player walks
  away, with stills of what follows; nothing withheld, only not delivered in a cutscene.
  Then LOCATIONS.md (ten places: unique, found, learned, look out for; proposals marked ◇), the
  bombs' meaning and places, the villagers' view on sneaking into houses, and the village as the
  unannounced tutorial (stones, a chest, a cart on the rails through the Wardens' camp, a switch,
  the slope back to town, a well pump with small crystals, climbing and pulling into houses, axes,
  trees, grey rats, a torch that gives the hero away).
  Then the fountains as the save points, aid to the village (the farm, foraging; more picked ◇),
  ore as the village's wealth, Rennick's woodland, the bow in the inn, the pick a worse weapon, the
  blunt sword in the smithy, the fallen adventurer's body in the mine.
  Then the weapons (the axe as strong as the sword but slower to ready, the torch 2 against bats
  and 1 otherwise, any held item reaching 3 tiles against the one of bare hands), the Kharn's
  nervous scouting before the attack, the walk from the cave-in to their tunnels, and every place
  having several paths, partly out of sight: sneaking past nearly everything, harder near a group.
  Then the barricade: beams at the village entrance, built by the town itself with time and an axe,
  the hands more than doubled once the Wardens understand their danger; the ◇ picks in
  LOCATIONS.md stand as the logical defaults (owner's leave); with little aid the hero defends the
  town himself, with potions from ingredients he gathers (Tiny Farm's plants).
  Then BUILD.md (the build in small safe steps: A tools, B mechanics each in a test room, C the
  world's systems, D the places); LOCATIONS.md's puzzles, traps and chests in sight but hard to
  reach; the item rulings (the open shaft, the sack in the rope's slot, rubble as touching rocks,
  crystal sizes that do not mix, no fire spreading but a tar barrel and the accidental barricade
  that a pick turns into 2 beams, standing beams that collapse a tunnel).
  Built since (BUILD.md A0): the drawing kit (intent.loft shapes: ground, trails, blobs, rooms,
  corridors, houses, slopes, items; editor refill paints materials and rises), tools/fill.loft and
  tools/map.loft; the test world's seven new rooms drawn with it and a street for them in the demo
  village (12 houses).  Kit bugs found by drawing them and fixed: rails split a room's free floor
  (no rubble placed), a door drawn into a wall was not kept clear, trails before rooms walled a
  tunnel's mouth.
  Then B1, the weapons (owner took the proposed numbers, to tune in play): sword and axe 2, the
  axe 600 ms to ready, a torch swings and costs a flier 2, a held thing reaches 3 cells; five fight
  rooms (rat run, bat cave, spider hall, cyclops den, hunter's hall: a weapon by the door, the enemy
  at the far end) in the demo street (17 houses).  Combat designed (DESIGN.md @GM-Combat, proposed,
  owner: as in Dark Souls — tells, light and heavy, lunge, stance and dodge, parry and riposte);
  BUILD.md B1b is its steps — then built: wind-ups aimed at cells (dodge by leaving them), block,
  parry and riposte, heavy swing, lunge, side blow, poise, the light chain, the early-swing back-off;
  a parry turns to the soonest blow; ties follow the place against the hero, never the list's order.
  The rat run has three rats and a cyclops 8 health, so an attacker is hurt (`tests/fights.loft`).
  Keys by hand (owner): WASD is the left hand, so the right acts — J swing (held: heavy), K guard
  (alone: parry/block; with a way: pull), L put down, I aim; with the arrows, Space, Shift, X, T.
  The floor labels name the right-hand keys; `checks/game.loft` presses both sets.
  Story discussion edits STORY.md locally and commits once a part settles, never per sentence;
  the owner may also work from a cloud session (a `ccr-*` branch): check `git ls-remote` first.

## Next steps, in the order agreed

**State 2026-10-31** (200 tests on both backends; levels: first_climb, the_cellar, the_torch_room,
the_old_mine, the_cyclops_hall).  Built since the list below: bombs, rats/bats/spiders/guards,
the bow and aiming, potions and money, the editor's slopes and junctions, cyclopes (sight by
light, shout, calls, search, blasts draw them; axe / bombs / unarmed pull), the hunter, and a
world clock that steps from one due time to the next (`tests/clock.loft`).  The hero's blows take
400 ms to recover from (owner ruling, `tests/blows.loft`).  A bested enemy leaves what it
held in sight (owner).
Built since: spiders fleeing to another layer through an opening, pulling a cart over a layer's
edge, a wall in front of the hero left out of the picture and nothing drawn past a wall from
the hero (owner; the ghost tried first was dropped — tiles 0.6.0's ghost option is unused).

1. ~~`fetch-libs.sh` refs to `main`~~ — done; 32 tests pass on both backends.
2. ~~**Editor step 2**~~ — done (EDITOR.md § 8): `editor.loft` (tested in `tests/editor.loft`),
   drawn and keyed in `game.loft`, the play camera (`follow` / `rooms`) in `camera.loft`.  Checked
   by pictures driven through the real keys (`TW_KEYS`, README), and the play frames are
   byte-identical to before.  Ctrl+O opens the next level of `order.json` until `input` reads
   typed text (EDITOR.md step 7).
3. **Rules still to build** (DESIGN.md slice 2).  Built: 4-step layers and slopes, ceilings,
   fence (cell and edge), tree, doors that open, stairs, the held item (full hands — ANY held
   item blocks push/pull/lift, the owner's rule — drop on X, the axe), chests (never moved,
   the owner's rule; walked into they open, a point; from a chair walked on like a table) and
   the signpost.  Built since: health and falls (1/3/6 for 1/2/3 layers), wells and safe points.  Open: enemies, guards, wells and safe points, the
   sword's fighting, a chest's money.
   Follow-ups: `checks/game.loft` reports `1 stores not freed at program exit: Theme×3` on the
   interpreter (the theme `new_game` loads; not yet looked at); game checks live in `checks/`
   until loft#2234 lets `--tests` open a `--lib` library's C core.
4. **Editor step 3**: materials and autotiling (EDITOR.md § 8) — built: grass, path, lake, castle wall, palette categories; fences as an autotile line set open.
   Step 5 brushes built (fill, rectangle, eyedropper, copy/paste) on tile_world 0.5.0 (loft-libs-world#39, merged); stamp files open.
   Step 6 checks built (K: path to a cell goal, chests cut off / hidden).  Next: step 7 platforms (typed text in `input` → names, Ctrl+O by name, stamp files, signs).

5. **Torches and light** (DESIGN.md § Torches and light, ART.md) — built: the game's own sheet
   (`tile-world.png`, `tools/sheet.loft`), the stick, the tar barrel, lighting both ways,
   sockets only (the staff too), burning out after 10 minutes wherever a torch is, the low
   flame and the flicker, and `the torch room` (third in `order.json`).  Open: light in the
   enemies' moves (with the enemies).
6. **The fading light** — built: a torch's light fades 4 shades over its ten minutes (the
   warning).  It needs **tile_world 0.6.0** (`shades(..., lights, start)`, loft-libs-world#40,
   merged): `./fetch-libs.sh` fetches it.  Owner
   2026-10-30: light is a bonus, never a wall (the semi-dark is always playable; no permanent
   flame needed); cyclopes later make darkness worth seeking (stealth).
7. **One item a cell** (DESIGN.md § Moving things) — built: items are pushed out of the way,
   a string of two sends the far one aside, no room blocks, no drop onto an item.  83 tests on
   both backends.  Still open from the 2026-10-30 evaluation (owner to decide): a permanent
   flame (a lit tar barrel?) so light is never a dead end; a floor flame lighting a torch like
   a socket's does; items on a table out of reach; sockets only on walls (editor); torches
   burning in real time while the player thinks; whether darkness hides enemies (the well's
   clear area, a hidden chest); the axe felling a tree leaving a stick.

## Elsewhere (all landed or handed over)

- @PLN204 32-bit regime: on loft `main`; the library 32-bit versions (text2d, input, audio_bus
  under `#width 32`) wait for it in a release.
- audio 0.2.0 (output), audio_bus 0.2.0, input 0.3.0 (device keys), screen 0.1.0: merged in
  loft-libs-game.  random 0.5.0: merged in loft-libs-core.  imaging 0.5.0, tiles 0.2.0,
  tile_world 0.4.1: merged.  None published to the registry yet — publishing waits for the
  owner's go.
- loft docs: `doc/claude/TANMATSU.md`, `doc/claude/HARDWARE_BACKENDS.md`.
