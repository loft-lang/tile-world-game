/* raster3d — the landscape kept as a picture, the actors drawn over it every frame
 * (@PLN218 in 3D; BUILD.md Phase E, E2 and E3).  The fixed camera from the south, a parallel
 * projection (owner, 2026-11-15): a cell PX pixels wide, its ground PZ deep, a layer PY tall.
 *
 * The landscape is a set of textured, lit triangles, in tiles, drawn into a kept picture B (colour
 * and depth).  A camera movement shifts B and draws the strips that came into view; a change of
 * the landscape (a tile given again: a door opened, a thing taken; a group of triangles hidden or
 * shown: the walls cut away in front of the hero) draws the rectangles it covered.  Each frame a framebuffer is made from B where something
 * changed since it was last shown, and the actors are drawn there, depth-tested against B.
 *
 * The one rule: every frame equals the whole scene drawn from scratch, pixel for pixel
 * (`r3_reference`).  It holds because a pixel's colour depends only on where the pixel lies
 * against each triangle: triangles in 1/16 pixel, integer edge functions with a fill rule that
 * gives a shared edge to one triangle, and attributes stepped in exact integer arithmetic.
 *
 * Buffers are laid out as the Tanmatsu's panel: portrait, screen column x is panel row x, so a
 * screen column is contiguous (R3_AT).  Colours are RGB565. */
#ifndef RASTER3D_H
#define RASTER3D_H
#include <stdint.h>

#define R3_SW 800
#define R3_SH 480
#define R3_AT(x, y) ((x) * R3_SH + (y))
#define R3_TS 16       /* a picture's texels each way */
#define R3_NONE 63519  /* a texel that is not there */

/* The projection: pixels a cell across, a cell's ground deep, a layer tall. */
#define R3_PX 48
#define R3_PZ 40
#define R3_PY 48

typedef struct R3 R3;

typedef struct { int x0, y0, x1, y1; } R3Rect; /* half-open, screen pixels */

typedef struct {
    int64_t b_px;       /* landscape pixels drawn into B (strips, rectangles) */
    int64_t fb_px;      /* pixels copied from B into the framebuffer */
    int64_t actor_px;   /* actor pixels written */
    int64_t tris;       /* triangles set up for a span pass */
    int rects;          /* rectangles the frame drew */
    int uncovered;      /* wall groups r3_uncover holds hidden now */
    int lights;         /* cells whose light is kept: those a tile is lit from, after a drop */
} R3Count;

/* A scene for a screen of R3_SW x R3_SH; `sky` the colour where nothing is. */
R3 *r3_new(uint16_t sky);
void r3_free(R3 *r);

/* A picture: 16 x 16 RGB565 texels, row by row, R3_NONE where there is none.  Answers its index. */
int r3_picture(R3 *r, const uint16_t *texels);

/* The landscape is kept in tiles, each the faces given under one key (a square of cells, say):
 * `r3_tile` starts tile `key` again — its faces so far are gone — and the faces after it are its
 * own, until the next `r3_tile`.  The change is made at the next camera, frame or build: the
 * tile's triangles set up, and B drawn again where they were and where they are, nothing else.
 * Tiles are drawn in the order of their keys; faces given before any `r3_tile` are tile 0's. */
void r3_tile(R3 *r, int key);
/* Tile `key` gone. */
void r3_drop(R3 *r, int key);

/* A landscape face, of the tile being given: picture `pic`, n corners of (x, y, z, u, v, light)
 * — x east, y up in layers, z south, u and v 0 .. 1 over the picture, light 0 .. 1 — and the
 * group it is hidden with (any number, -1: none; a group's faces may lie in many tiles). */
void r3_face(R3 *r, int pic, int n, const float *corners, int group);

/* The light of cells as data (owner, 2026-11-15): a face may say where its light comes from —
 * cell (lx, ly) on layer lz, times lk (a side a little darker) — and then it is lit from the light
 * `r3_light` gave that cell (its corners' own light until one is given).  When a cell's light
 * changes, the triangles lit from it are lit again and B is drawn again where they lie, with no
 * face given again: a torch carried or flickering costs its triangles, not their tiles. */
#define R3_NO_LIGHT (-1000000)
void r3_face_lit(R3 *r, int pic, int n, const float *corners, int group, int lx, int ly, int lz, float lk);
/* The light (0 .. 1) of cells x0 .. x0 + w - 1, y0 .. y0 + h - 1 on layer z, row by row. */
void r3_light(R3 *r, int z, int x0, int y0, int w, int h, const float *v);
/* Cell (x, y, z)'s light into *v; 0 when none was given.  When a tile is dropped, the light of
 * the cells no tile is lit from any more is forgotten (at the next camera, frame or build): the
 * store holds what the kept tiles read, a cell round them, not every cell the scene ever lit. */
int r3_light_at(const R3 *r, int x, int y, int z, float *v);

/* The depth's origin, the cell (ox, oz, layer oy): set before the first frame, and set again when
 * the scene has gone far from it (a depth is 16 bits, about 40 cells either way).  A new origin
 * sets every tile up again and draws B whole; the same one only makes the changes. */
void r3_build(R3 *r, float ox, float oy, float oz);

/* The camera: the screen's top-left at these world pixels.  The first call draws B whole; each
 * later one shifts B and draws the strips. */
void r3_camera(R3 *r, int px, int py);

/* Hide or show a group's faces; B is drawn again where they were. */
void r3_hide(R3 *r, int group, int hidden);

/* The actors of this frame: begin, then one card each — a picture standing upright facing the
 * camera, its bottom middle at (x, y, z), `size` cells wide and tall, its light.  `id` names the
 * actor from frame to frame: one drawn exactly as a framebuffer last drew it costs nothing. */
void r3_actors_begin(R3 *r);
void r3_actor(R3 *r, int id, int pic, float x, float y, float z, float size, float light);
/* An actor posed: its picture turned `deg` degrees clockwise (as the camera sees it) about its
 * point (pu, pv) — 0 .. 1 across and down the picture; r3_actor's is (0.5, 1), the bottom middle,
 * unturned — and mirrored left to right when `mirror`.  (x, y, z) is still the bottom middle of the
 * picture as it stands unturned. */
void r3_actor_pose(R3 *r, int id, int pic, float x, float y, float z, float size, float light, float deg,
                   float pu, float pv, int mirror);

/* The actor `id` is never hidden: every wall group that covers its rectangle and reaches nearer
 * than it is hidden, and those it hid before that no longer do are shown again.  Call it after
 * the actors. */
void r3_uncover(R3 *r, int id);

/* A picture over everything — the status line, talk bubbles — `w` x `h` pixels row by row, each
 * drawn `scale` x `scale` on the screen from its top-left, R3_NONE where it is clear.  Where it
 * differs from the last one given, the frame is made again; NULL takes it away. */
void r3_overlay(R3 *r, const uint16_t *pix, int w, int h, int scale);

/* Make framebuffer `fb` (R3_SW * R3_SH) for this frame: `which` is 0 or 1, the buffer as last
 * made by this call two frames ago (or never: then it is made whole). */
void r3_frame(R3 *r, uint16_t *fb, int which);

/* The same frame drawn from scratch, for the check. */
void r3_reference(R3 *r, uint16_t *col);

R3Count r3_count(R3 *r); /* since the last call */

/* The same through handles, for loft (src/raster3d.loft). */
int64_t r3h_new(int32_t sky);
void r3h_free(int64_t h);
int64_t r3h_picture(int64_t h, const int32_t *texels);
void r3h_faces(int64_t h, const float *d, int64_t len);
void r3h_tile(int64_t h, int64_t key, const float *d, int64_t len);
void r3h_light(int64_t h, const float *box, const float *v, int64_t len);
float r3h_light_at(int64_t h, int64_t x, int64_t y, int64_t z);
int64_t r3h_lights(int64_t h);
void r3h_overlay(int64_t h, const int32_t *pix, int64_t w, int64_t hh, int64_t scale);
void r3h_drop(int64_t h, int64_t key);
void r3h_build(int64_t h, const float *origin);
void r3h_camera(int64_t h, int64_t px, int64_t py);
void r3h_actors(int64_t h, const float *d, int64_t len, int64_t keep_seen);
void r3h_frame(int64_t h, int32_t *out);

#endif
