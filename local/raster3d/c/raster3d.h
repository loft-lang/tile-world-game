/* raster3d — the landscape kept as a picture, the actors drawn over it every frame
 * (@PLN218 in 3D; BUILD.md Phase E, E2 and E3).  The fixed camera from the south, a parallel
 * projection (owner, 2026-11-15): a cell PX pixels wide, its ground PZ deep, a layer PY tall.
 *
 * The landscape is a set of textured, lit triangles drawn into a kept picture B (colour and
 * depth).  A camera movement shifts B and draws the strips that came into view; a change of the
 * landscape (a group of triangles hidden or shown: the walls cut away in front of the hero, a
 * door) draws the rectangle it covered.  Each frame a framebuffer is made from B where something
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
} R3Count;

/* A scene for a screen of R3_SW x R3_SH; `sky` the colour where nothing is. */
R3 *r3_new(uint16_t sky);
void r3_free(R3 *r);

/* A picture: 16 x 16 RGB565 texels, row by row, R3_NONE where there is none.  Answers its index. */
int r3_picture(R3 *r, const uint16_t *texels);

/* A landscape face: picture `pic`, n corners of (x, y, z, u, v, light) — x east, y up in layers,
 * z south, u and v 0 .. 1 over the picture, light 0 .. 1 — and the group it is hidden with. */
void r3_face(R3 *r, int pic, int n, const double *corners, int group);

/* Made ready to draw after the last face; the depth's origin is the cell (ox, oz, layer oy). */
void r3_build(R3 *r, double ox, double oy, double oz);

/* The camera: the screen's top-left at these world pixels.  The first call draws B whole; each
 * later one shifts B and draws the strips. */
void r3_camera(R3 *r, int px, int py);

/* Hide or show a group's faces; B is drawn again where they were. */
void r3_hide(R3 *r, int group, int hidden);

/* The actors of this frame: begin, then one card each — a picture standing upright facing the
 * camera, its bottom middle at (x, y, z), `size` cells wide and tall, its light.  `id` names the
 * actor from frame to frame: one drawn exactly as a framebuffer last drew it costs nothing. */
void r3_actors_begin(R3 *r);
void r3_actor(R3 *r, int id, int pic, double x, double y, double z, double size, double light);

/* The actor `id` is never hidden: every wall group that covers its rectangle and reaches nearer
 * than it is hidden, and those it hid before that no longer do are shown again.  Call it after
 * the actors. */
void r3_uncover(R3 *r, int id);

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
void r3h_faces(int64_t h, const double *d, int64_t len);
void r3h_build(int64_t h, const double *origin);
void r3h_camera(int64_t h, int64_t px, int64_t py);
void r3h_actors(int64_t h, const double *d, int64_t len, int64_t keep_seen);
void r3h_frame(int64_t h, int32_t *out);

#endif
