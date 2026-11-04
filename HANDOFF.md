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
  what it carries the way it moves, swings (Space) and staggers; drawn in depth order over the
  ground.  Verified: `tests/motion.loft` (every number by hand) and the facing and swing tests in
  `tests/moves.loft` on both backends; eight mutations (hop, trailing, sweep, knock, chaining,
  rock, facing, swinging without a weapon) each fail their test.  `tools/anim.loft OUT_DIR`
  films a scene through the game's own drawing.  Pictures:
  `TW_SHOT=f.png [TW_LEVEL=1] [TW_MOVES="..."] [TW_CREDITS=1] loft --native --lib libs main.loft`.
- **Design docs:** `DESIGN.md` (the game: principle, rules agreed, slices) and `EDITOR.md` (the
  in-game editor: model, file format, places, keys, checks, platforms, order of work).

## State 2026-11-04

- **Places and doors** (WORLD.md step 1, mostly): the game starts in the demo village
  (`levels/demo.json`), a house per mechanic room, doors both ways, every place kept as left,
  floor text (`labels`), R = the place as entered.  `TW_WORLD` picks the start place; the demo
  stays its own world beside the story game to come (owner).  Not yet: what follows the hero
  through a door (a pulled cart, an enemy close behind), one clock across places (each keeps its
  own), doors in the editor.
- **Talk** (`talk.loft`, WORLD.md § Talking is moving): beside = talk, passing = bark; stay /
  bump / walk away while a line is said; gifts, take/give, `then`.
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
  is enough; what stays open (what "well defended" is made of, the other endings, the hero's
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
