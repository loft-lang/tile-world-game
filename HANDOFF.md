# Handoff — the tile_world game, its libraries, and the loft work around them

State on 2026-10-29, for picking the work up in a fresh session.  Paths are on the owner's
machine (`~/workspace`).

## Where things are

| What | Where | State |
|---|---|---|
| The game | `~/workspace/tile-world-game` (this repo, local git) | **no GitHub remote yet**: creating `loft-lang/tile-world-game` was blocked by the session's permissions; the owner runs `gh repo create loft-lang/tile-world-game --public --source . --remote origin --push` |
| Game libraries | `./fetch-libs.sh` → `libs/` (git-ignored) | **broken until fixed (step 1 below)**: it still fetches `tiles` from `tuxedo-tiles-shade` and `imaging` from `tuxedo-imaging-c`, and both branches were deleted when their PRs merged.  `libs/tile_world` is a hand copy of 0.4.1, which is now on `main` |
| loft branch | `~/workspace/loft2`, branch `tuxedo-pln204-int32` @ `72602f4b6` | = `main` (ec098768e) + one commit, the loft#2222 fix; force-pushed, no PR yet |

## Library PRs — all merged

| PR | What |
|---|---|
| loft-libs-graphics#61 | imaging 0.5.0 (lodepng, a `[c]` shim) |
| loft-libs-graphics#62, #63 | tiles 0.1.0, tiles 0.2.0 (8 shades) |
| loft-libs-world#37, #38 | tile_world 0.4.0 (edges, ground, rise), 0.4.1 (`ground_layers`) |

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
- **loft#2219** (open, sev:high, silent-wrong): `x = f() ?? return;` in a loop corrupts a
  returned struct's nested hash from the second iteration, on both backends.  The game avoids
  it (one load at a time).  Repro and boundary in the issue: only `?? return` triggers it; the
  same unwrap through `if !m { return; }` is right.  Not yet fixed.
- loft#2215 and loft#2216: on `main` (#2218).

## The game, as it stands

- **The split.** `tile_world` (library) holds the world and answers questions: paths, sight,
  light.  Everything a piece *does* is the game's: `rules.loft` reads only the properties in
  `kinds.json`, never a kind's name.  The look is `theme.json` through `theme.loft`.  Levels are
  `levels/*.json` through `levels.loft`, played in `levels/order.json`'s order.
- **Runs** on a desktop window (`screen`, X11) with Kenney credits first, sound through `audio`,
  keys through `screen`/`input`.  24 tests: `loft --lib libs --tests tests`.  Pictures:
  `TW_SHOT=f.png [TW_LEVEL=1] [TW_MOVES="..."] [TW_CREDITS=1] loft --native --lib libs main.loft`.
- **Design docs:** `DESIGN.md` (the game: principle, rules agreed, slices) and `EDITOR.md` (the
  in-game editor: model, file format, places, keys, checks, platforms, order of work).

## Next steps, in the order agreed

1. In `fetch-libs.sh`, set the `tiles` and `imaging` refs to `main` (their branches are gone),
   re-fetch, and run the 24 tests.
2. **Editor step 2** (EDITOR.md § 8): `E` toggle, cursor and scrolling, layers, palette,
   paint/erase/stack, start and goal markers, save and open, undo, play-test.  Needs `tiles`
   `rect` / `frame` (a library addition) and the follow / rooms camera (EDITOR.md § 4).
3. **Rules still to build** (DESIGN.md slice 2): 4-step layers (so a slope's top and a chair on a
   table both step onto the layer above), fence climbing, doors that open, stairs, chests and
   the signpost, the held item (sword, axe), enemies, guards, wells.
4. Fix loft#2219.

## Elsewhere (all landed or handed over)

- @PLN204 32-bit regime: on loft `main`; the library 32-bit versions (text2d, input, audio_bus
  under `#width 32`) wait for it in a release.
- audio 0.2.0 (output), audio_bus 0.2.0, input 0.3.0 (device keys), screen 0.1.0: merged in
  loft-libs-game.  random 0.5.0: merged in loft-libs-core.  imaging 0.5.0, tiles 0.2.0,
  tile_world 0.4.1: merged.  None published to the registry yet — publishing waits for the
  owner's go.
- loft docs: `doc/claude/TANMATSU.md`, `doc/claude/HARDWARE_BACKENDS.md`.
