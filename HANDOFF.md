# Handoff — the tile_world game, its libraries, and the loft work around them

State on 2026-10-28, for picking the work up in a fresh session.  Paths are on the owner's
machine (`~/workspace`).

## Where things are

| What | Where | State |
|---|---|---|
| The game | `~/workspace/tile-world-game` (this repo, local git) | clean, 3 commits; **no GitHub remote yet**: creating `loft-lang/tile-world-game` was blocked by the session's permissions; the owner runs `gh repo create loft-lang/tile-world-game --public --source . --remote origin --push` |
| Game libraries | `./fetch-libs.sh` → `libs/` (git-ignored) | fetches each from its repo; `libs/tile_world` is currently a hand copy of branch `tuxedo-tile-world-grounds` (0.4.1) |
| loft branch | `~/workspace/loft2`, branch `tuxedo-pln204-int32` @ `d6e670755` | pushed; joined up to `d6e670755` into `tuxedo-pr2205-next` by session loft3-05 |

## Open PRs (all auto-merge)

| PR | What | Blocked by |
|---|---|---|
| loft-libs-world#37 | tile_world 0.4.0: edges between cells, a layer's ground, a cell's rise | its CI (was running) |
| branch `tuxedo-tile-world-grounds` (no PR yet) | tile_world 0.4.1: `ground_layers(w)`, stacked on #37 | open its PR (base `main`) once #37 merges |
| loft-libs-graphics#61 | imaging 0.5.0 (lodepng) | `drawing`'s CI red, which `main` has too (its guide asserts a PNG that graphics' save_png does not write on `--interpret`) — fix that next |
| loft-libs-graphics#62 → #63 | tiles 0.1.0 → 0.2.0 (8 shades) | same `drawing` red; #63 is stacked on #62 |

When #61/#62/#63 merge, change `fetch-libs.sh`'s `tiles` and `imaging` refs to `main`.

## Open loft issues of this work

- **loft#2219** (open, sev:high, silent-wrong): `x = f() ?? return;` in a loop corrupts a
  returned struct's nested hash from the second iteration, on both backends.  The game avoids
  it (one load at a time).  Repro and boundary in the issue: only `?? return` triggers it; the
  same unwrap through `if !m { return; }` is right.  Not yet fixed.
- **loft#2216** (fixed-pending-merge): the fix is `advice[linked-group-same-key]`
  (`d6e670755`), in the join.  Two keyed collections of one record type in one struct are one
  record set by the language's rule (Col-Group), so tile_world uses separate record types for
  its kind tables.

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

1. Fix `drawing`'s red on loft-libs-graphics `main`, so #61, #62 and #63 merge.
2. Open the PR for `tuxedo-tile-world-grounds` once #37 merges; point `fetch-libs.sh` at main.
3. **Editor step 2** (EDITOR.md § 8): `E` toggle, cursor and scrolling, layers, palette,
   paint/erase/stack, start and goal markers, save and open, undo, play-test.  Needs `tiles`
   `rect` / `frame` (a library addition) and the follow / rooms camera (EDITOR.md § 4).
4. **Rules still to build** (DESIGN.md slice 2): 4-step layers (so a slope's top and a chair on a
   table both step onto the layer above), fence climbing, doors that open, stairs, chests and
   the signpost, the held item (sword, axe), enemies, guards, wells.
5. Fix loft#2219.

## Elsewhere in the same session (all landed or handed over)

- @PLN204 32-bit regime: on `main` through the joins; the library 32-bit versions (text2d,
  input, audio_bus under `#width 32`) wait for it in a release.
- audio 0.2.0 (output), audio_bus 0.2.0, input 0.3.0 (device keys), screen 0.1.0: merged in
  loft-libs-game.  random 0.5.0: merged in loft-libs-core.  None published to the registry
  yet — publishing waits for the owner's go.
- loft docs: `doc/claude/TANMATSU.md`, `doc/claude/HARDWARE_BACKENDS.md`.
