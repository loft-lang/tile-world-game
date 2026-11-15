<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# The scene contract — one 3D view, drawn by the CPU or by a GPU

**What this is.** The agreement between a game's 3D view and whatever draws it: what a view
gives (a scene), what every implementation draws from it, and what an implementation may do
better or must do less.  A game writes its view once, against this; it runs drawn on the CPU on
the Tanmatsu and on a GPU on a desktop or in a page.

**Status.** Two implementations draw the game's fixed view: `local/raster3d` (C, the CPU), whose
API is this contract's starting point and is not yet reshaped to it (§ From today's raster3d),
and `scene3d` (loft-libs-graphics, on `gpu`), the loft stream's.  `fixedview.loft` gives its
scene to `scene3d` in the device's own window where a GPU opens, and to `raster3d` through
`screen` where none does (`TW_GPU=0`); its § What draws is the one place that tells them apart.

## The owner's rulings it rests on (2026-11-15)

- **One engine that works for the Tanmatsu and does not hinder a game on a full GPU.**
- **A free camera on a full GPU, a limited one on the Tanmatsu**, for every game — this one,
  dryopea, moros, crawler: "So the games (even the tile_world) get a free camera, but a limited
  one for the tanmatsu."  The limited one is the fixed camera (RENDER_3D.md § The camera and
  what is seen): from it the player still looks behind things and into a room, through the
  cut-away.
- **The hero is never hidden** (RENDER_3D.md): walls in front of it are cut away.
- **The engine side is C, and loft describes the scene** (the loft stream's rule, HANDOFF_FROM_LOFT.md § 5).

## The parts

```
the game's rules, its world (tile_world, the hex libraries)        what is there
        │
the view, in loft: builds the scene from what is there             portable: render3d's faces,
        │   (tiles of faces, the cells' light, actors, the overlay,  fixedview's tile keeping, a hex
        │    the camera it wants)                                    game's chunk mesher
        ▼
   ── the scene contract ──────────────────────────────────────────
        │                                   │
the CPU implementation (raster3d)     the GPU implementation (on `gpu`)
  the Tanmatsu; any machine             a desktop, a page, a phone
```

The host — the package a program is linked with for a machine — asks each implementation in turn
and takes the first that opens, as `gpu` asks its backends.  A view never names one.

## What a scene holds

Units and axes are RENDER_3D.md's: x east, y up, z south; a cell (a hex game: its unit) is 1
across, a layer 1 high.  A colour is `0xAARRGGBB`; a light is 0 (dark) to 1 (full); a picture's
pixel (0, 0) is its top-left.

1. **Pictures**, each with a number the scene gives back.  Any size.  A picture is what faces,
   cards and models are painted with.
2. **Tiles**: faces given under a key (a square of cells, a hex chunk), given again whole when
   anything in them changes, and dropped by key.  A face is a picture on 3 to 8 corners, each
   corner a place and a point of the picture (u, v from 0 to 1), and:
   - **where its light comes from**: a cell (x, y, layer) and a factor (a side a little darker,
     an underside darker still), or a light of its own per corner;
   - **its group**: a number the cut-away hides it by (one cell's walls on one layer), or none;
   - optionally **its normal**, for an implementation that lights by direction.
3. **The cells' light**: boxes of cells on a layer, a light each, given whenever they change.
   Faces lit from those cells are lit again; no face is given again.  An implementation that
   lights by **torches** (§ Torches) is given the lights the game carries and sets down apart, and
   the cells' light without them.
4. **Actors**, given every frame, each with an id that names it from frame to frame:
   - a **card**: a picture standing upright facing the camera, its bottom middle at a place,
     its size, turned some degrees about a point of the picture and mirrored or not (the 2D's
     pose: `game::figure_turn`, `theme::held_place`); or
   - a **model**: a mesh (a `glb`'s) with its place and turn, **and a card that stands in for
     it** where models are not drawn;
   - its light: a value, or a cell to take it from.
5. **The overlay**: what is drawn over everything — the status line, talk bubbles — in the 2D
   game's own drawing, with the pixels clear where it does not draw.
6. **The camera** (§ The camera), set every frame.
7. **The hero**: which actor is never hidden.

## The camera

An implementation says which cameras it **offers**, and a view asks before it chooses:

| Camera | What it is | Offered by |
|---|---|---|
| **fixed** | Parallel, from the south at the 2D's angle (a cell 48 pixels across, its ground 40 deep, a layer 48 tall at the Tanmatsu's scale); the view gives the point it looks at, in world units | every implementation |
| **free** | An eye, a point looked at, and a field of view: perspective, anywhere | an implementation on a GPU |

- A view states an intent — keep the hero in view, follow it with some slack, let the player look
  round — and uses the free camera where one is offered, the fixed one otherwise.  The fixed
  camera is always there, so every game runs on every implementation.
- A camera that is not offered is refused, and the scene keeps the one it had: it never falls
  back silently to something else.
- The CPU implementation turns the fixed camera's point into whole pixels.  That is what makes its
  movements exact (a shift of the kept picture), and it is the implementation's business, not
  the view's: the view gives world units.
- Turning the free camera off on a desktop shows the Tanmatsu's view: the port is tested without
  the device.

## Torches

The lights the game gives — what the hero carries, the flames set down, a blast's flash
(`rules::torches`) — are the scene's torches.  On the CPU they are part of the cells' light, a
shade a cell, and a flicker dims a torch's cells a shade.  A GPU lights them smoothly instead:
each torch fades from its place across the cells it reaches, its flicker eased, its colour its
own.

An implementation says whether it lights by torches (`offers_torches`).  A view asks, as it asks
for the free camera, and gives a scene that does:

| What | When | The numbers |
|---|---|---|
| The cells' light **without** the torches | as § What a scene holds 3 | as today (`faces3d::light_box(.., false)`) |
| A torch's **reach** | when it is new, moved, or its `start` changed; dropped by id when it is gone | its id, its layer, the box's corner (x0, y0), width, height, then the light the torch alone gives each cell (0 .. 1), 0 where it does not reach (`faces3d::torch_reach`, from `tile_world::reach`) |
| The **torches** | every frame | 9 each: its id, its place (x, y, z), the shade it starts at, its glow (1 full, `rules::FLICKER_LOW` at a flicker's bottom), its colour's red, green, blue (0 .. 1) (`faces3d::torch_data`) |

A scene that does not light by torches (raster3d) is given the cells' light with them, as before.

What an implementation that lights by torches keeps:
1. **What is lit is the CPU's.** A torch lights a cell only where its reach is above 0, so walls,
   doors and opaque cells stop it as the rules stop it.  The reach is given per torch so the
   implementation never works it out.
2. **At a cell, the brightest way.** The light at a point is the brightest of its cell's light and
   each torch term there, a colour channel at a time, as the CPU takes a cell's light from its
   brightest light.
3. **A torch's term** is its colour times its glow times a smooth falloff, brightest at the torch,
   the cell's reach value at the cell's middle: a cell's light averages the CPU's step for it.
4. **Two torches a cell.** Where more reach a cell, the two strongest there (reach value times
   glow) are drawn, so a pixel costs at most two torch terms however many torches are in view.
   An implementation may draw more, never fewer.
5. **A flicker is the glow alone**: no reach is given again, and nothing is uploaded but the
   torches' numbers.

The check: `checks/torchlight.loft` — the cells' light without the torches and each torch's reach,
the brightest of them cell by cell, equal the cells' light with the torches (the flicker held
still: the CPU dims a flickering torch a shade, a GPU by its glow); and a glow eases between the
flicker's steps, equal to them at each step's start.

## The rules every implementation keeps

1. **What is there is the same everywhere.** The same tiles, actors, cut-away, overlay: the game
   is the same game.  How finely it is drawn differs (§ What an implementation may do more, or
   less).
2. **The hero is never hidden.** Every wall group between the camera and the hero's card is cut
   away, and shown again when it no longer is.  For the fixed camera: a group whose faces cover
   the card's rectangle on the screen and reach nearer than it (raster3d's `r3_uncover`).  For the
   free camera: a group that a line from the eye to the card crosses.  Only wall groups: doors,
   trees and things may hide part of it.
3. **A frame is the scene's, not its history's.** What a frame shows is a function of the scene
   as it is: the tiles, the light, the actors, the overlay and the camera of that frame — never of
   the order things were given in, or of what an earlier frame drew.  The CPU implementation
   keeps a picture between frames and promises more: every frame equals the scene drawn from
   scratch, pixel for pixel (`r3_reference`, `bench/frame3d.c`).
4. **Changes come by key.** A tile given again or dropped, a box of light, a group hidden: the
   implementation decides what that costs it to draw again.  A view never redraws anything itself.
5. **Nothing is lost in the reduction.** Where an implementation draws less (a smaller picture, a
   card for a model, the cells' light for named lights), it draws what was given, reduced — never
   nothing.  A face it cannot use is refused when given, not dropped when drawn.

## What an implementation may do more, or less

| | The CPU (raster3d, the Tanmatsu) | A GPU (on `gpu`) |
|---|---|---|
| The screen | 800 × 480, RGB565, laid out as the panel's portrait rows | Any window or page, any size |
| Pictures | Reduced to 16 × 16 RGB565 once, when given (nearest) | Full size, filtered, mipmapped |
| The camera | Fixed only; the point snapped to whole pixels | Fixed and free; smooth, any zoom |
| Light | A face lit by its cell's light times its factor; a torch a shade a cell | Torches smooth and coloured (§ Torches); also by normals, shadows, fog |
| Actors | Cards; a model is drawn as its stand-in card | Cards and models |
| The overlay | At the world's pixel size (a cell 16), scaled to the screen | Also at native resolution |
| Its promise | Every frame equal to a full redraw; only what changed drawn | Every frame drawn whole, fast |

## The CPU implementation: raster3d as it stands

It is single precision and 32-bit loft throughout, as the Tanmatsu runs it: its C holds no double,
and its binding takes `vector<single>` (BUILD.md Phase E).

`local/raster3d` (BUILD.md Phase E, raster3d), measured:
- the landscape kept as a picture B, shifted when the camera moves, tiles drawn again only where
  one was and is (binned for strips), groups hidden by the rectangle they covered;
- the cells' light as data, the triangles a change reaches lit again, the light no tile reads
  forgotten after a drop;
- posed actors over B, depth-tested; an unchanged one costs nothing; the overlay over them;
- two framebuffers, each with its own stale rectangles;
- a frame of Orbury's town 0.57–0.92 ms under `qemu-riscv32` following or sliding, with seven
  actors, the hero posed and what it holds swinging (1.4–1.9 ms with the landscape, the light and
  the overlay changing under it), the whole scene from scratch 10 ms; in the game, walking into
  Orbury natively, 2.08 ms a frame on average.

## The GPU implementation: what each part becomes

| Scene | On `gpu` (`gpu_gl` and later backends) |
|---|---|
| Pictures | One atlas texture (or several) |
| A tile | A vertex buffer, given again or released with it |
| The cells' light | A light-map texture indexed by cell; a box of light is one `texture_put`; the shader reads it by the face's light cell |
| Groups hidden | A per-vertex group and a small "hidden" texture, or a tile's draw ranges per group |
| Actors | Cards as an instance buffer (the turn and mirror in the vertex shader); models as meshes |
| The overlay | A texture drawn over the pass by `gpu_paint` |
| The camera | A matrix: parallel for the fixed camera, perspective for the free one |
| The never-hidden rule | The free camera's line test on the CPU side (it is the scene's groups, not pixels), the result as the hidden texture |

Its tests need no display: `gpu_record` writes down every call (§ Checks).

## Checks

- **The CPU implementation** keeps its own: `bench/frame3d.c` (every frame against the scene drawn
  from scratch, both cameras, with tiles, light and overlay changing; controls that must differ),
  and `tools/fixedshot.loft` (a walk's last frame against the same moment built fresh).
- **The GPU implementation**: `gpu_record`'s log for what it asks of a backend, on every machine;
  and its picture against the CPU's, the same scene, the fixed camera at the CPU's resolution:
  `xvfb-run loft --native --lib libs tools/fixedshot.loft orbury OUT.png "s d d" change night gpu`.
  Orbury by day and at night by the hero's torch: 27 of 384,000 pixels differ by more than one
  RGB565 step.  Most of the rest differ by exactly one step: the GPU rounds the light's product,
  the CPU truncates it.
- **The contract**: one scene script — `tools/scene3d.loft`'s files, which `bench/frame3d.c` reads —
  played through every implementation, the same tiles, light, actors and overlay; what differs
  must be what § What an implementation may do more, or less allows.

## From today's raster3d to the contract

What changes in raster3d's loft binding, so a view written against it today is written against
the contract (the engine's C stays behind it):

| Today | The contract |
|---|---|
| `add_picture`: 16 × 16 texels, RGB565 | A picture of any size, `0xAARRGGBB`; raster3d reduces it |
| `WIDTH`, `HEIGHT`: 800 × 480 | The host says the screen's size |
| `move_camera(px, py)` in the CPU's whole screen pixels; `build_scene(x, y, z)` for the depth's origin | A camera in world units (§ The camera); the origin is the implementation's own |
| Actors: 11 numbers (a card) | A card or a model with its stand-in card |
| `set_overlay(px, w, h, scale)` | The overlay as the host sizes it; the implementation chooses its scale |
| Faces with a light cell, `set_light` by boxes | The same (the contract took them as they are) |
| `set_tile`, `drop_tile`, groups, `keep_seen` | The same |
| `draw_frame(out)`: the frame as pixels for `screen::show` | The frame shown by the host: on the Tanmatsu raster3d's two framebuffers flipped (BUILD.md: not yet) |

## Open

- **Its name and its home**: `scene3d`, in loft-libs-graphics beside `gpu`? (The loft stream's.)
- **The hex games' views**: dryopea's `gridmesh` chunks, moros's `hex_mesh`, crawler's
  `worldmesh` given as tiles; a spike first (one dryopea map into raster3d, timed here and under
  the emulator).
- **Models on the CPU**: a stand-in card is the floor; a model drawn as faces on the CPU, where it
  is small enough, may come later.
- **The overlay at native resolution on a GPU**: today the overlay is the 2D game's pixels; a
  GPU drawing the 2D's text larger needs the game's 2D drawing at another scale.
- **The fixed camera turned**: the 2D and this camera look from the south; a fixed camera from
  another side (a quarter turn) is a parallel camera too, and would keep the CPU's promises.
