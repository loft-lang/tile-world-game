# Handoff from the loft stream — what changed for the game, and what is the game's to do

Written 2026-11-15 by the agent that builds loft and its libraries, for the agent that builds
this game.  It was read against this repo at `0a3829a` and changes no other file here.  The
loft side's own record is `doc/claude/plans/209-platform-agnostic-games/` in the loft repo
(`GPU_LAYER.md` the plan, `API_GPU.md` the GPU boundary).

## The short of it

1. **The libraries the game pins are on `main`.**  `fetch-libs.sh` can name `main` for
   `audio`, `screen` and `tiles`.
2. **The game plays on a Mac** as a shipped native program, in 2D, and its first-person view
   draws there in a window of its own.  Two lines in `render3d.loft` turn that on (below).
3. **The four loft defects this game met are fixed, but not yet in a loft the game runs.**
   Keep the workarounds until a loft build carries the fixes.
4. **A GPU layer that names no graphics stack is being built** (`gpu`).  Nothing in the game
   changes for it yet; what it will ask of `render3d.loft` is listed so it is no surprise.

## 1. The libraries on `main`

| Library | Repo | What `main` now has | Pull request |
|---|---|---|---|
| `screen` 0.2.1 | loft-libs-game | The branch `tuxedo-screen-esp-present`, a window on macOS, and `screen::program_keys()` | #32, #33 |
| `audio` | loft-libs-game | The branch `tuxedo-screen-esp-present` | #32 |
| `tiles` 0.7.0 | loft-libs-graphics | The branch `tuxedo-native-pixels`: the camera and the display surfaces | #81 |

The two branches still exist and GitHub shows them "ahead" of `main`; that is the squash
merge, their content is in `main`.

**For the game to do:** in `fetch-libs.sh`, name `main` for `audio`, `screen` and `tiles`,
then run the tests.  Measured on the loft side with this game at `dc27cf0` and the merged
libraries: 480 tests in 52 files pass on the interpreter and on the C backend.  It was not
measured again at `0a3829a`.

None of these versions is published to the registry yet.  The game fetches by branch, so that
does not hold it up.

## 2. The game on a Mac

Measured on an M3 Max with the game at `dc27cf0`, built `--native-c --ship`:

| What | Result |
|---|---|
| The 2D game in `screen`'s window, 832 x 640 | plays; draw 2.4 ms, copy 0.1 ms, show 3.0 ms a frame |
| The first-person view in its own 960 x 720 window | draws the same picture from a `--native-c` and a `--ship` build; 6.3 to 6.9 ms a frame |
| 25 seconds standing still, each | no slow frame (over 50 ms) in the play log |

**For the game to do**, to have the first-person view on a desktop until the GPU layer gives
it one window with the frame (the owner's choice, 2026-11-15):

- **`render3d::toggle`** (the line `if !v.on && !tiles::on_display(g.surface) { return false; }`):
  let the view come on where the frame is a page's **or** where `screen::program_keys()` is
  true.  That call answers true where `keys()` reads the program's keys whichever of its
  windows is in front (macOS) and false where each window has its own (X11).  On X11 the gate
  stays shut: the view's window would take the keys.
- **`render3d::draw`**: after the view is drawn, call `gl.win.swap()` where the frame is not a
  page's (`!tiles::on_display(g.surface)`).  In a page `swap` hands the browser a frame, which
  `screen::poll` already does.

In this form the hearts and the note (`overlay`) show in the 2D window, not over the view.

**Two more things a shipped build needs:**

- A shipped program reads its files beside the executable, not in the working directory.
  Build with `--out` into the game's folder, or copy the executable there.
- It needs `libloft_net.dylib` (the `web` library's component, `make components` in loft)
  beside it.

**Still open, and the game's:** a scripted run in a window.  `TW_MOVES` plays only into a
picture (`TW_SHOT`).  The loft plan times the game with a scripted run (its steps P5 and M4),
so a `TW_MOVES` that replays into the open window, with the play log on, is what lets the Mac's
GL and Metal be compared on the same walk.

## 3. The loft defects this game met

All four are fixed with a guard on loft's branch `tuxedo-pr2438-post`.  That branch has no
pull request yet, so no released or daily loft has them, and the issues stay open until it
merges.

| Issue | What it was | The game's workaround | After the fix reaches the game's loft |
|---|---|---|---|
| loft#2507 | On the interpreter, a library's function reached another loaded library's of the same name | raster3d's names are unique (`new_scene`) | The names may go back, or stay |
| loft#2510 | The same cause seen from natives: `tiles::clear` ran `tile_world`'s `clear`, and `checks/game.loft` aborted with `--interpret` | `tools/scene3d.loft` fills with `tiles::rect` | `tiles::clear` works; with the fix `checks/game.loft` passed on the interpreter at `dc27cf0` |
| loft#2509 | A name bound in an inner loop and bound again after it was "Unknown variable 'd#b0'" | The inner names are renamed (`fd`, `il`, `rl`, `pl`, `cl`) | They may go back, or stay |
| loft#2508 | Narrow C arguments past the eighth arrived scrambled under the interpreter on Apple silicon (met through `tl_draw_ground`) | None needed on Linux | `tiles`' 43 tests pass on a Mac's interpreter |

HANDOFF.md says `main.loft` aborts on the interpreter because of loft#2507.  With the fix
`checks/game.loft` was measured; `main.loft` itself was not run on the interpreter.

loft#2492 (a capturing lambda stored in a collection gives a wrong answer and says nothing) is
open and unfixed.  Avoid that shape.

## 4. The GPU layer that is coming

The owner ruled (2026-11-15) that games and `stage` build against one GPU library that depends
on no graphics stack, with each stack a library beneath it: OpenGL and WebGL2, Metal, Vulkan,
later a console's.  A game never names a backend; a host package asks each in turn.

Where it stands:

| Step | State |
|---|---|
| The boundary on paper, tried on every caller including this game's view | Done (`API_GPU.md`) |
| `gpu` and a recording backend, `gpu_record` | Built, in review as loft-libs-graphics#83 |
| A backend finds the layer's header once both are installed | In progress, in loft |
| `gpu_gl`, then graphics and `stage` onto it, then `gpu_metal` | Not started |

`gpu` is a working name and is not published.  **Do not build against it yet.**

When the game's view moves (the plan's step L8, with this stream supplying the library), this
is what changes in `render3d.loft`, so that nothing written meanwhile makes it harder:

- **The shader pair becomes one declaration**: the three inputs (position, shade, texture
  coordinate), one uniform block, one texture slot, and the shader text per backend.
- **Uniforms are set as one block, not by name.**  A block holds floats, pairs, four-float
  vectors and matrices.  It holds no three-float field and no integer: `eye` and `fogc` become
  four floats read as three.  Measured on a Mac, OpenGL and Metal place the field after a
  three-float one at different bytes, so a block that had one would be read differently on
  each.
- **Render state belongs to a pipeline.**  The depth test switched off for the hand becomes a
  second pipeline of the same shader, not a call between two draws.  `enable`, `disable` and
  `depth_write` on the window go.
- **A texture is given by slot** in the pass that draws with it; `bind` and the sampler
  uniform go.  `atlas.put` and nearest filtering stay.
- **Everything a draw needs is named in its own pass.**  Nothing stays set from an earlier
  frame or an earlier draw with another pipeline.

`gpu_record` writes every call down as text, so the view's drawing can then be tested with no
window and no GPU, on every machine.

## 5. `local/raster3d`

The fixed camera's engine is C in this repo, with loft bindings, which fits the owner's rule
that the engine side is C and loft describes the scene.  Proving it here first is the right
order.  Libraries are the loft stream's to build and publish, so when its API has settled,
say so in HANDOFF.md: it then moves to a library repo with its tests, and the game fetches it
like the others.

Two things from the loft side that bear on it:

- The frame tier (`tiles`, the CPU) is the floor every game is held to, and the plan's
  recommendation is that the Tanmatsu's renderer stays there and does not go under `gpu`.
  The owner has not ruled on that yet.
- A wasm build's C library formats no floating-point number in `printf` (`%g` prints as
  itself).  Met in `gpu_record`; it applies to any C that logs a float and is built for a
  page.

## How to report a gap

A loft or library defect is a GitHub issue on `loft-lang/loft` with the smallest program that
shows it, run on the interpreter and on the C backend, and the loft version it was measured
on.  There is no `hit-by:` label for this game yet, so say "found in tile-world-game" in the
body.  This stream builds and fixes; the game reports, and keeps its workaround until the fix
is in a loft it runs.
