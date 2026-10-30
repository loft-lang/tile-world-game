# Handoff — the tile_world game, its libraries, and the loft work around them

State on 2026-10-29 (evening), for picking the work up in a fresh session.  Paths are on the owner's
machine (`~/workspace`).

## Where things are

| What | Where | State |
|---|---|---|
| The game | `~/workspace/tile-world-game` (this repo, local git) | **no GitHub remote yet**: creating `loft-lang/tile-world-game` was blocked by the session's permissions; the owner runs `gh repo create loft-lang/tile-world-game --public --source . --remote origin --push` |
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

## Next steps, in the order agreed

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
