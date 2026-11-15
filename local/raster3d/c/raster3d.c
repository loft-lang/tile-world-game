/* raster3d — see raster3d.h. */
#include "raster3d.h"

#include <stdlib.h>
#include <string.h>

#define SUB 16   /* subpixel steps a pixel */
#define FIX 24   /* fractional bits of a stepped attribute */
#define NA 4     /* attributes: depth, u, v, light */
#define BIN 64   /* the side of a tile's bins, in pixels */

typedef struct {
    int32_t X[3], Y[3];          /* world subpixels */
    int64_t ea[3], eb[3], ec[3]; /* edge i: ea*px + eb*py + ec + bias >= 0 inside */
    int64_t base[NA], gx[NA], gy[NA];
    int x0, y0, x1, y1;          /* world pixels, half-open, a pixel round */
    int tx0, ty0, tx1, ty1;      /* the same, tight: the pixels the corners reach into */
    int dmin;                    /* its nearest depth */
    int pic, group, hid;
    int src;                     /* the face's triangle it was set up from, in its tile */
} Tri;

typedef struct { Tri *v; int n, cap; } Tris;
typedef struct { R3Rect *v; int n, cap; } Rects;
typedef struct { int *v; int n, cap; } Ints;

/* An actor as drawn: what it was asked to be, and where that put it. */
typedef struct { int id, pic; double x, y, z, size, light, deg, pu, pv; int mirror; R3Rect rect; int t0, tn; } Shown;
typedef struct { Shown *v; int n, cap; } Showns;

typedef struct {
    int all;      /* to be made whole */
    Rects stale;  /* landscape drawn again since it was last made */
    Showns shown; /* its actors, as it last drew them */
} Buffer;

typedef struct { double x, y, z, u, v, l; } Corner;

/* A tile of the landscape: the faces the caller gave under one key (a square of cells), kept as
 * their triangles' corners — so a new depth origin sets them up again here — and set up. */
typedef struct {
    int key, changed;
    Corner *c;                   /* three corners a triangle */
    int *pic, *grp, n, cap;
    int *lx, *ly, *lz;           /* where each triangle's light comes from (lz R3_NO_LIGHT: its own) */
    double *lk;                  /* ... times this */
    Tris tris;
    R3Rect box;                  /* world pixels its triangles cover; empty when none */
    R3Rect was;                  /* ... before the change being made */
    /* Its triangles by the BIN x BIN squares of its box they reach: square b's are
     * bidx[bstart[b] .. bstart[b + 1]], in the order given. */
    int nbx, nby, *bstart, *bidx, *mark;
    /* The cells its faces are lit from: (lx0, ly0) .. (lx1, ly1), any layer; none when !lit. */
    int lit, lx0, ly0, lx1, ly1;
} Tile;

struct R3 {
    uint16_t sky;
    uint16_t (*pics)[R3_TS * R3_TS];
    int npics, cappics;
    Tile *tiles;                 /* by key, ascending: the order they are drawn in */
    int ntiles, captiles, cur, pending;
    int stamp, *pick, cappick;   /* a draw's triangles of one tile, once each */
    Tris act;
    Showns acts;                 /* this frame's actors: act holds their triangles, two each */
    double ox, oy, oz;           /* the depth's origin */
    int origin;                  /* ... was given */
    Ints hid;                    /* the groups hidden */
    /* The light of cells (`r3_light`): an open hash of (x, y, z) to the light, 0 .. 1. */
    int64_t *lkey;
    double *lval;
    int lcap, ln;
    int forget;                  /* a tile was dropped: the light no tile is lit from goes at the commit */
    /* The overlay (r3_overlay): ow x oh pixels, each os x os on the screen, R3_NONE clear. */
    uint16_t *ov;
    int ow, oh, os;
    Rects dirty;                 /* world pixels to draw again when the change is made */
    /* the camera and B */
    int cx, cy, placed;
    uint16_t *b_col, *b_dep, *f_dep, *r_dep;
    Buffer fb[2];
    R3Count cnt;
    Rects made;                  /* the rectangles the last frame made */
    int whole;                   /* ... or all of it */
    Ints uncovered;              /* the groups r3_uncover hid */
};

/* Rounding without libm, so the shim links on every target loft builds it for. */
static int64_t round_ll(double x) { return x < 0 ? -(int64_t) (-x + 0.5) : (int64_t) (x + 0.5); }
static double floor_d(double x) { double i = (double) (int64_t) x; return i > x ? i - 1 : i; }

static int min(int a, int b) { return a < b ? a : b; }
static int max(int a, int b) { return a > b ? a : b; }

static int64_t floor_div(int64_t a, int64_t b) {
    int64_t q = a / b;
    if ((a % b) != 0 && ((a < 0) != (b < 0))) q--;
    return q;
}

static void push_rect(Rects *l, R3Rect r) {
    if (r.x0 >= r.x1 || r.y0 >= r.y1) return;
    if (l->n == l->cap) { l->cap = l->cap ? l->cap * 2 : 16; l->v = realloc(l->v, (size_t) l->cap * sizeof(R3Rect)); }
    l->v[l->n++] = r;
}

static Tri *push_tri(Tris *l) {
    if (l->n == l->cap) { l->cap = l->cap ? l->cap * 2 : 256; l->v = realloc(l->v, (size_t) l->cap * sizeof(Tri)); }
    return &l->v[l->n++];
}

static R3Rect clip_screen(R3Rect r) {
    R3Rect c = { max(r.x0, 0), max(r.y0, 0), min(r.x1, R3_SW), min(r.y1, R3_SH) };
    return c;
}

R3 *r3_new(uint16_t sky) {
    R3 *r = calloc(1, sizeof(R3));
    r->sky = sky;
    r->b_col = malloc(sizeof(uint16_t) * R3_SW * R3_SH);
    r->b_dep = malloc(sizeof(uint16_t) * R3_SW * R3_SH);
    r->f_dep = malloc(sizeof(uint16_t) * R3_SW * R3_SH);
    r->r_dep = malloc(sizeof(uint16_t) * R3_SW * R3_SH);
    r->fb[0].all = r->fb[1].all = 1;
    r->cur = -1;
    return r;
}

void r3_free(R3 *r) {
    if (!r) return;
    for (int i = 0; i < r->ntiles; i++) {
        Tile *t = &r->tiles[i];
        free(t->c); free(t->pic); free(t->grp); free(t->tris.v);
        free(t->lx); free(t->ly); free(t->lz); free(t->lk);
        free(t->bstart); free(t->bidx); free(t->mark);
    }
    free(r->tiles); free(r->pick); free(r->lkey); free(r->lval); free(r->ov); free(r->hid.v); free(r->dirty.v); free(r->uncovered.v); free(r->made.v);
    free(r->act.v); free(r->acts.v); free(r->pics);
    for (int k = 0; k < 2; k++) { free(r->fb[k].stale.v); free(r->fb[k].shown.v); }
    free(r->b_col); free(r->b_dep); free(r->f_dep); free(r->r_dep);
    free(r);
}

int r3_picture(R3 *r, const uint16_t *texels) {
    if (r->npics == r->cappics) {
        r->cappics = r->cappics ? r->cappics * 2 : 64;
        r->pics = realloc(r->pics, (size_t) r->cappics * sizeof(*r->pics));
    }
    memcpy(r->pics[r->npics], texels, sizeof(*r->pics));
    return r->npics++;
}

/* --- Triangles --- */

/* The corner's place in world subpixels and its attributes: depth, u and v in texels, light. */
static void place(const R3 *r, const Corner *c, int32_t *X, int32_t *Y, double *a) {
    *X = (int32_t) round_ll(c->x * R3_PX * SUB);
    *Y = (int32_t) round_ll((c->z * R3_PZ - c->y * R3_PY) * SUB);
    a[0] = 32768.0 - 16.0 * ((c->y - r->oy) * R3_PZ + (c->z - r->oz) * R3_PY);
    a[1] = c->u * R3_TS;
    a[2] = c->v * R3_TS;
    a[3] = (c->l < 0 ? 0 : c->l > 1 ? 1 : c->l) * 256.0;
}

static int set_up(const R3 *r, Tri *t, const Corner *c0, const Corner *c1, const Corner *c2) {
    double a[3][NA];
    place(r, c0, &t->X[0], &t->Y[0], a[0]);
    place(r, c1, &t->X[1], &t->Y[1], a[1]);
    place(r, c2, &t->X[2], &t->Y[2], a[2]);
    int64_t area = (int64_t) (t->X[1] - t->X[0]) * (t->Y[2] - t->Y[0]) - (int64_t) (t->Y[1] - t->Y[0]) * (t->X[2] - t->X[0]);
    if (area == 0) return 0;
    if (area < 0) { /* one winding, so inside is every edge >= 0 */
        int32_t tx = t->X[1], ty = t->Y[1];
        t->X[1] = t->X[2]; t->Y[1] = t->Y[2]; t->X[2] = tx; t->Y[2] = ty;
        for (int k = 0; k < NA; k++) { double s = a[1][k]; a[1][k] = a[2][k]; a[2][k] = s; }
    }
    for (int i = 0; i < 3; i++) {
        int ia = (i + 1) % 3, ib = (i + 2) % 3; /* edge i runs from the corner after i to the next */
        int64_t dx = t->X[ib] - t->X[ia], dy = t->Y[ib] - t->Y[ia];
        t->ea[i] = -dy;
        t->eb[i] = dx;
        /* A shared edge's pixels go to the triangle that has it running one way. */
        int64_t bias = (dy > 0 || (dy == 0 && dx > 0)) ? 0 : -1;
        t->ec[i] = dy * t->X[ia] - dx * t->Y[ia] + bias;
    }
    double x1 = t->X[1] - t->X[0], y1 = t->Y[1] - t->Y[0], x2 = t->X[2] - t->X[0], y2 = t->Y[2] - t->Y[0];
    double det = x1 * y2 - x2 * y1;
    for (int k = 0; k < NA; k++) {
        double d1 = a[1][k] - a[0][k], d2 = a[2][k] - a[0][k];
        double gx = (d1 * y2 - d2 * y1) / det, gy = (d2 * x1 - d1 * x2) / det;
        t->base[k] = round_ll(a[0][k] * (double) (1 << FIX));
        t->gx[k] = round_ll(gx * (double) (1 << FIX));
        t->gy[k] = round_ll(gy * (double) (1 << FIX));
    }
    int mx = min(t->X[0], min(t->X[1], t->X[2])), Mx = max(t->X[0], max(t->X[1], t->X[2]));
    int my = min(t->Y[0], min(t->Y[1], t->Y[2])), My = max(t->Y[0], max(t->Y[1], t->Y[2]));
    t->x0 = (int) floor_div(mx, SUB) - 1;
    t->y0 = (int) floor_div(my, SUB) - 1;
    t->x1 = (int) floor_div(Mx, SUB) + 2;
    t->y1 = (int) floor_div(My, SUB) + 2;
    t->tx0 = (int) floor_div(mx, SUB);
    t->ty0 = (int) floor_div(my, SUB);
    t->tx1 = (int) floor_div(Mx + SUB - 1, SUB);
    t->ty1 = (int) floor_div(My + SUB - 1, SUB);
    double dm = a[0][0];
    if (a[1][0] < dm) dm = a[1][0];
    if (a[2][0] < dm) dm = a[2][0];
    t->dmin = (int) floor_d(dm);
    return 1;
}

static uint16_t lit(uint16_t t, int l) {
    if (l >= 256) return t;
    unsigned rr = ((unsigned) (t >> 11) * (unsigned) l) >> 8;
    unsigned gg = ((unsigned) ((t >> 5) & 63) * (unsigned) l) >> 8;
    unsigned bb = ((unsigned) (t & 31) * (unsigned) l) >> 8;
    return (uint16_t) ((rr << 11) | (gg << 5) | bb);
}

/* Triangle t drawn into (col, dep) where it lies inside screen rectangle `c`, the camera at
 * (cx, cy).  Answers the pixels written. */
static int64_t span(const R3 *r, const Tri *t, R3Rect c, int cx, int cy, uint16_t *col, uint16_t *dep) {
    int sx0 = max(c.x0, t->x0 - cx), sx1 = min(c.x1, t->x1 - cx);
    int sy0 = max(c.y0, t->y0 - cy), sy1 = min(c.y1, t->y1 - cy);
    if (sx0 >= sx1 || sy0 >= sy1) return 0;
    const uint16_t *pic = r->pics[t->pic];
    int64_t written = 0;
    for (int sx = sx0; sx < sx1; sx++) {
        int64_t pcx = (int64_t) (sx + cx) * SUB + SUB / 2;
        /* The rows inside: each edge bounds them from one side. */
        int64_t lo = (int64_t) sy0 + cy, hi = (int64_t) sy1 - 1 + cy;
        for (int i = 0; i < 3 && lo <= hi; i++) {
            int64_t k = t->ea[i] * pcx + t->ec[i];
            int64_t d = t->eb[i] * SUB, rr = -(k + t->eb[i] * (SUB / 2));
            if (d > 0) { int64_t q = -floor_div(-rr, d); if (q > lo) lo = q; }
            else if (d < 0) { int64_t q = floor_div(rr, d); if (q < hi) hi = q; }
            else if (rr > 0) hi = lo - 1;
        }
        if (lo > hi) continue;
        int64_t pcy = lo * SUB + SUB / 2;
        int64_t ddx = pcx - t->X[0], ddy = pcy - t->Y[0];
        int64_t at[NA], st[NA];
        for (int k = 0; k < NA; k++) {
            at[k] = t->base[k] + t->gx[k] * ddx + t->gy[k] * ddy;
            st[k] = t->gy[k] * SUB;
        }
        uint16_t *cp = col + R3_AT(sx, 0) - cy, *dp = dep + R3_AT(sx, 0) - cy;
        for (int64_t wy = lo; wy <= hi; wy++) {
            int dk = (int) (at[0] >> FIX);
            if ((unsigned) dk > 65534u) dk = dk < 0 ? 0 : 65534;
            if (dk < dp[wy]) {
                int tu = (int) (at[1] >> FIX), tv = (int) (at[2] >> FIX);
                tu = tu < 0 ? 0 : tu > R3_TS - 1 ? R3_TS - 1 : tu;
                tv = tv < 0 ? 0 : tv > R3_TS - 1 ? R3_TS - 1 : tv;
                uint16_t tx = pic[tv * R3_TS + tu];
                if (tx != R3_NONE) {
                    cp[wy] = lit(tx, (int) (at[3] >> FIX));
                    dp[wy] = (uint16_t) dk;
                    written++;
                }
            }
            at[0] += st[0]; at[1] += st[1]; at[2] += st[2]; at[3] += st[3];
        }
    }
    return written;
}

/* --- The light of cells --- */

static int64_t cell_key(int x, int y, int z) {
    return ((int64_t) (z + 32768) << 44) | ((int64_t) (y + (1 << 21)) << 22) | (int64_t) (x + (1 << 21));
}

static uint64_t mix(int64_t k) {
    uint64_t h = (uint64_t) k * 0x9E3779B97F4A7C15ull;
    return h ^ (h >> 29);
}

/* The light of cell (x, y, z) into *v; 0 when it was never given. */
static int light_of(const R3 *r, int x, int y, int z, double *v) {
    if (r->lcap == 0) return 0;
    int64_t k = cell_key(x, y, z);
    for (uint64_t i = mix(k) & (uint64_t) (r->lcap - 1);; i = (i + 1) & (uint64_t) (r->lcap - 1)) {
        if (r->lkey[i] == -1) return 0;
        if (r->lkey[i] == k) { *v = r->lval[i]; return 1; }
    }
}

static void put_light(R3 *r, int x, int y, int z, double v) {
    if (2 * (r->ln + 1) > r->lcap) {
        int ocap = r->lcap;
        int64_t *ok = r->lkey;
        double *ov = r->lval;
        r->lcap = ocap ? ocap * 2 : 4096;
        r->lkey = malloc((size_t) r->lcap * sizeof(int64_t));
        r->lval = malloc((size_t) r->lcap * sizeof(double));
        for (int i = 0; i < r->lcap; i++) r->lkey[i] = -1;
        r->ln = 0;
        for (int i = 0; i < ocap; i++)
            if (ok[i] != -1) {
                uint64_t j = mix(ok[i]) & (uint64_t) (r->lcap - 1);
                while (r->lkey[j] != -1) j = (j + 1) & (uint64_t) (r->lcap - 1);
                r->lkey[j] = ok[i];
                r->lval[j] = ov[i];
                r->ln++;
            }
        free(ok);
        free(ov);
    }
    int64_t k = cell_key(x, y, z);
    uint64_t i = mix(k) & (uint64_t) (r->lcap - 1);
    while (r->lkey[i] != -1 && r->lkey[i] != k) i = (i + 1) & (uint64_t) (r->lcap - 1);
    if (r->lkey[i] == -1) { r->lkey[i] = k; r->ln++; }
    r->lval[i] = v;
}

/* The light of the cells no tile is lit from any more, forgotten: the store kept only the cells
 * inside a tile's lit box, a cell round it to spare.  A triangle keeps the light its corners were
 * last given, and a tile built again is given its light before its faces, so nothing drawn
 * changes. */
static void sweep_lights(R3 *r) {
    if (r->lcap == 0) return;
    int ocap = r->lcap;
    int64_t *ok = r->lkey;
    double *ov = r->lval;
    r->lkey = NULL;
    r->lval = NULL;
    r->lcap = r->ln = 0;
    for (int i = 0; i < ocap; i++) {
        if (ok[i] == -1) continue;
        int x = (int) (ok[i] & ((1 << 22) - 1)) - (1 << 21);
        int y = (int) ((ok[i] >> 22) & ((1 << 22) - 1)) - (1 << 21);
        int z = (int) (ok[i] >> 44) - 32768;
        int keep = 0;
        for (int t = 0; t < r->ntiles && !keep; t++) {
            const Tile *tl = &r->tiles[t];
            keep = tl->lit && x >= tl->lx0 - 1 && x <= tl->lx1 + 1 && y >= tl->ly0 - 1 && y <= tl->ly1 + 1;
        }
        if (keep) put_light(r, x, y, z, ov[i]);
    }
    free(ok);
    free(ov);
}

/* Triangle i of a tile lit from its cell, when it has one and the cell's light was given: its three
 * corners' light set.  Answers whether that changed them. */
static int light_corners(const R3 *r, Tile *tl, int i) {
    if (tl->lz[i] == R3_NO_LIGHT) return 0;
    double v;
    if (!light_of(r, tl->lx[i], tl->ly[i], tl->lz[i], &v)) return 0;
    double l = v * tl->lk[i];
    Corner *c = &tl->c[i * 3];
    if (c[0].l == l && c[1].l == l && c[2].l == l) return 0;
    c[0].l = c[1].l = c[2].l = l;
    return 1;
}

/* --- The landscape, in tiles --- */

static int in_ints(const Ints *l, int v) {
    for (int i = 0; i < l->n; i++) if (l->v[i] == v) return 1;
    return 0;
}

static void push_int(Ints *l, int v) {
    if (l->n == l->cap) { l->cap = l->cap ? l->cap * 2 : 16; l->v = realloc(l->v, (size_t) l->cap * sizeof(int)); }
    l->v[l->n++] = v;
}

static int empty(R3Rect b) { return b.x0 >= b.x1 || b.y0 >= b.y1; }

static R3Rect join(R3Rect a, R3Rect b) {
    if (empty(a)) return b;
    if (empty(b)) return a;
    return (R3Rect) { min(a.x0, b.x0), min(a.y0, b.y0), max(a.x1, b.x1), max(a.y1, b.y1) };
}

/* Do a and b share a pixel?  An empty rectangle shares none, wherever it lies (an empty tile's box
 * is (0, 0, 0, 0): it has no bins to look in). */
static int meets(R3Rect a, R3Rect b) {
    return !empty(a) && !empty(b) && a.x0 < b.x1 && b.x0 < a.x1 && a.y0 < b.y1 && b.y0 < a.y1;
}

/* The index of tile `key`, or where it would go (and *found 0). */
static int tile_index(const R3 *r, int key, int *found) {
    int lo = 0, hi = r->ntiles;
    while (lo < hi) {
        int m = (lo + hi) / 2;
        if (r->tiles[m].key < key) lo = m + 1; else hi = m;
    }
    *found = lo < r->ntiles && r->tiles[lo].key == key;
    return lo;
}

void r3_tile(R3 *r, int key) {
    int found, i = tile_index(r, key, &found);
    if (!found) {
        if (r->ntiles == r->captiles) {
            r->captiles = r->captiles ? r->captiles * 2 : 64;
            r->tiles = realloc(r->tiles, (size_t) r->captiles * sizeof(Tile));
        }
        memmove(&r->tiles[i + 1], &r->tiles[i], (size_t) (r->ntiles - i) * sizeof(Tile));
        memset(&r->tiles[i], 0, sizeof(Tile));
        r->tiles[i].key = key;
        r->ntiles++;
    }
    Tile *t = &r->tiles[i];
    if (!t->changed) { t->was = t->box; t->changed = 1; }
    t->n = 0;
    t->tris.n = 0;
    t->box = (R3Rect) { 0, 0, 0, 0 };
    r->cur = i;
    r->pending = 1;
}

void r3_drop(R3 *r, int key) {
    int found, i = tile_index(r, key, &found);
    if (!found) return;
    Tile *t = &r->tiles[i];
    push_rect(&r->dirty, join(t->box, t->changed ? t->was : t->box));
    free(t->c); free(t->pic); free(t->grp); free(t->tris.v);
    free(t->lx); free(t->ly); free(t->lz); free(t->lk);
    free(t->bstart); free(t->bidx); free(t->mark);
    memmove(&r->tiles[i], &r->tiles[i + 1], (size_t) (r->ntiles - i - 1) * sizeof(Tile));
    r->ntiles--;
    r->cur = -1;
    r->pending = 1;
    r->forget = 1;
}

void r3_face(R3 *r, int pic, int n, const double *c, int group) {
    r3_face_lit(r, pic, n, c, group, 0, 0, R3_NO_LIGHT, 1.0);
}

void r3_face_lit(R3 *r, int pic, int n, const double *c, int group, int lx, int ly, int lz, double lk) {
    if (pic < 0 || pic >= r->npics || n < 3) return;
    if (r->cur < 0) r3_tile(r, 0);
    Tile *tl = &r->tiles[r->cur];
    const Corner *k = (const Corner *) c;
    for (int i = 1; i + 1 < n; i++) {
        if (tl->n == tl->cap) {
            tl->cap = tl->cap ? tl->cap * 2 : 64;
            tl->c = realloc(tl->c, (size_t) tl->cap * 3 * sizeof(Corner));
            tl->pic = realloc(tl->pic, (size_t) tl->cap * sizeof(int));
            tl->grp = realloc(tl->grp, (size_t) tl->cap * sizeof(int));
            tl->lx = realloc(tl->lx, (size_t) tl->cap * sizeof(int));
            tl->ly = realloc(tl->ly, (size_t) tl->cap * sizeof(int));
            tl->lz = realloc(tl->lz, (size_t) tl->cap * sizeof(int));
            tl->lk = realloc(tl->lk, (size_t) tl->cap * sizeof(double));
        }
        Corner *t = &tl->c[tl->n * 3];
        t[0] = k[0]; t[1] = k[i]; t[2] = k[i + 1];
        tl->pic[tl->n] = pic;
        tl->grp[tl->n] = group;
        tl->lx[tl->n] = lx;
        tl->ly[tl->n] = ly;
        tl->lz[tl->n] = lz;
        tl->lk[tl->n] = lk;
        tl->n++;
    }
}

/* A tile's triangles set up from its corners, at the depth's origin. */
static void set_up_tile(R3 *r, Tile *tl) {
    tl->tris.n = 0;
    tl->box = (R3Rect) { 0, 0, 0, 0 };
    for (int i = 0; i < tl->n; i++) {
        light_corners(r, tl, i);
        Tri *t = push_tri(&tl->tris);
        Corner *k = &tl->c[i * 3];
        if (!set_up(r, t, &k[0], &k[1], &k[2])) { tl->tris.n--; continue; }
        t->pic = tl->pic[i];
        t->group = tl->grp[i];
        t->src = i;
        t->hid = t->group >= 0 && in_ints(&r->hid, t->group);
        tl->box = join(tl->box, (R3Rect) { t->x0, t->y0, t->x1, t->y1 });
    }
    tl->lit = 0;
    for (int i = 0; i < tl->n; i++) {
        if (tl->lz[i] == R3_NO_LIGHT) continue;
        if (!tl->lit) { tl->lx0 = tl->lx1 = tl->lx[i]; tl->ly0 = tl->ly1 = tl->ly[i]; tl->lit = 1; }
        tl->lx0 = min(tl->lx0, tl->lx[i]); tl->lx1 = max(tl->lx1, tl->lx[i]);
        tl->ly0 = min(tl->ly0, tl->ly[i]); tl->ly1 = max(tl->ly1, tl->ly[i]);
    }
    free(tl->bstart); free(tl->bidx); free(tl->mark);
    tl->bstart = tl->bidx = tl->mark = NULL;
    tl->nbx = tl->nby = 0;
    int n = tl->tris.n;
    if (n == 0) return;
    tl->nbx = (tl->box.x1 - tl->box.x0 + BIN - 1) / BIN;
    tl->nby = (tl->box.y1 - tl->box.y0 + BIN - 1) / BIN;
    int nb = tl->nbx * tl->nby;
    tl->bstart = calloc((size_t) nb + 1, sizeof(int));
    for (int pass = 0; pass < 2; pass++) {
        int *fill = pass ? calloc((size_t) nb, sizeof(int)) : NULL;
        for (int i = 0; i < n; i++) {
            const Tri *t = &tl->tris.v[i];
            for (int by = (t->y0 - tl->box.y0) / BIN; by <= (t->y1 - 1 - tl->box.y0) / BIN; by++)
                for (int bx = (t->x0 - tl->box.x0) / BIN; bx <= (t->x1 - 1 - tl->box.x0) / BIN; bx++) {
                    int b = by * tl->nbx + bx;
                    if (pass == 0) tl->bstart[b + 1]++;
                    else tl->bidx[tl->bstart[b] + fill[b]++] = i;
                }
        }
        if (pass == 0) {
            for (int b = 0; b < nb; b++) tl->bstart[b + 1] += tl->bstart[b];
            tl->bidx = malloc((size_t) tl->bstart[nb] * sizeof(int) + 1);
        } else free(fill);
    }
    tl->mark = calloc((size_t) n, sizeof(int));
}

static int cmp_int(const void *a, const void *b) { return *(const int *) a - *(const int *) b; }

/* B drawn from scratch inside screen rectangle c: the tiles in order, each one's triangles in the
 * order they were given. */
static void draw_land(R3 *r, R3Rect c, uint16_t *col, uint16_t *dep) {
    c = clip_screen(c);
    if (c.x0 >= c.x1 || c.y0 >= c.y1) return;
    for (int x = c.x0; x < c.x1; x++) {
        uint16_t *cp = col + R3_AT(x, 0), *dp = dep + R3_AT(x, 0);
        for (int y = c.y0; y < c.y1; y++) { cp[y] = r->sky; dp[y] = 65535; }
    }
    r->cnt.b_px += (int64_t) (c.x1 - c.x0) * (c.y1 - c.y0);
    R3Rect w = { c.x0 + r->cx, c.y0 + r->cy, c.x1 + r->cx, c.y1 + r->cy };
    for (int i = 0; i < r->ntiles; i++) {
        Tile *tl = &r->tiles[i];
        if (!meets(tl->box, w)) continue;
        /* The triangles in the tile's bins under w, once each, in the order they were given. */
        if (r->cappick < tl->tris.n) { r->cappick = tl->tris.n; r->pick = realloc(r->pick, (size_t) r->cappick * sizeof(int)); }
        int bx0 = max(w.x0 - tl->box.x0, 0) / BIN, bx1 = min(w.x1 - 1 - tl->box.x0, tl->box.x1 - 1 - tl->box.x0) / BIN;
        int by0 = max(w.y0 - tl->box.y0, 0) / BIN, by1 = min(w.y1 - 1 - tl->box.y0, tl->box.y1 - 1 - tl->box.y0) / BIN;
        int np = 0;
        r->stamp++;
        for (int by = by0; by <= by1; by++)
            for (int bx = bx0; bx <= bx1; bx++) {
                int b = by * tl->nbx + bx;
                for (int k = tl->bstart[b]; k < tl->bstart[b + 1]; k++) {
                    int j = tl->bidx[k];
                    if (tl->mark[j] != r->stamp) { tl->mark[j] = r->stamp; r->pick[np++] = j; }
                }
            }
        qsort(r->pick, (size_t) np, sizeof(int), cmp_int);
        for (int k = 0; k < np; k++) {
            const Tri *t = &tl->tris.v[r->pick[k]];
            if (t->hid || !meets((R3Rect) { t->x0, t->y0, t->x1, t->y1 }, w)) continue;
            r->cnt.tris++;
            span(r, t, c, r->cx, r->cy, col, dep);
        }
    }
}

static void stale_both(R3 *r, R3Rect c) {
    for (int k = 0; k < 2; k++) if (!r->fb[k].all) push_rect(&r->fb[k].stale, c);
}

/* B drawn again over world rectangle w, and both framebuffers told. */
static void redraw(R3 *r, R3Rect w) {
    if (!r->placed || empty(w)) return;
    R3Rect s = clip_screen((R3Rect) { w.x0 - r->cx, w.y0 - r->cy, w.x1 - r->cx, w.y1 - r->cy });
    if (empty(s)) return;
    draw_land(r, s, r->b_col, r->b_dep);
    stale_both(r, s);
}

/* The tiles changed since the last time, set up, and B drawn again where they were and are. */
static void commit(R3 *r) {
    if (!r->pending) return;
    r->pending = 0;
    for (int i = 0; i < r->ntiles; i++) {
        Tile *tl = &r->tiles[i];
        if (!tl->changed) continue;
        set_up_tile(r, tl);
        push_rect(&r->dirty, join(tl->was, tl->box));
        tl->changed = 0;
    }
    r->cur = -1;
    if (r->forget) { sweep_lights(r); r->forget = 0; }
#ifndef R3_SKIP_DIRTY /* the check's control: a change B is not drawn again for */
    for (int i = 0; i < r->dirty.n; i++) redraw(r, r->dirty.v[i]);
#endif
    r->dirty.n = 0;
}

void r3_build(R3 *r, double ox, double oy, double oz) {
    static const R3Rect whole = { 0, 0, R3_SW, R3_SH };
    if (r->origin && ox == r->ox && oy == r->oy && oz == r->oz) { commit(r); return; }
    r->ox = ox; r->oy = oy; r->oz = oz;
    r->origin = 1;
    for (int i = 0; i < r->ntiles; i++) { set_up_tile(r, &r->tiles[i]); r->tiles[i].changed = 0; }
    r->pending = 0;
    r->dirty.n = 0;
    r->cur = -1;
    if (!r->placed) return;
    /* Every depth moved by the same whole number, so the picture is the same; drawn again all
     * the same, so that holds by construction rather than by arithmetic. */
    draw_land(r, whole, r->b_col, r->b_dep);
    r->fb[0].all = r->fb[1].all = 1;
}

static void shift(uint16_t *buf, int dx, int dy) {
    if (dx > 0) memmove(buf + R3_AT(dx, 0), buf, sizeof(uint16_t) * (size_t) (R3_SW - dx) * R3_SH);
    else if (dx < 0) memmove(buf, buf + R3_AT(-dx, 0), sizeof(uint16_t) * (size_t) (R3_SW + dx) * R3_SH);
    if (dy != 0)
        for (int x = 0; x < R3_SW; x++) {
            uint16_t *c = buf + R3_AT(x, 0);
            if (dy > 0) memmove(c + dy, c, sizeof(uint16_t) * (size_t) (R3_SH - dy));
            else memmove(c, c - dy, sizeof(uint16_t) * (size_t) (R3_SH + dy));
        }
}

void r3_camera(R3 *r, int px, int py) {
    static const R3Rect whole = { 0, 0, R3_SW, R3_SH };
    commit(r);
    if (r->placed && px == r->cx && py == r->cy) return;
    int dx = r->cx - px, dy = r->cy - py; /* what was at x is at x + dx */
    int was = r->placed;
    r->cx = px; r->cy = py; r->placed = 1;
    r->fb[0].all = r->fb[1].all = 1;
    if (!was || abs(dx) >= R3_SW || abs(dy) >= R3_SH) { draw_land(r, whole, r->b_col, r->b_dep); return; }
    shift(r->b_col, dx, dy);
    shift(r->b_dep, dx, dy);
    R3Rect cols = dx > 0 ? (R3Rect) { 0, 0, dx, R3_SH } : (R3Rect) { R3_SW + dx, 0, R3_SW, R3_SH };
    R3Rect rows = dy > 0 ? (R3Rect) { 0, 0, R3_SW, dy } : (R3Rect) { 0, R3_SH + dy, R3_SW, R3_SH };
    if (dx != 0) {
        draw_land(r, cols, r->b_col, r->b_dep);
        if (dx > 0) rows.x0 = dx; else rows.x1 = R3_SW + dx;
    }
    if (dy != 0) draw_land(r, rows, r->b_col, r->b_dep);
}

void r3_hide(R3 *r, int group, int hidden) {
    if (group < 0) return;
    commit(r);
    int now = in_ints(&r->hid, group);
    if (now == (hidden != 0)) return;
    if (hidden) push_int(&r->hid, group);
    else for (int i = 0; i < r->hid.n; i++) if (r->hid.v[i] == group) { r->hid.v[i] = r->hid.v[--r->hid.n]; break; }
    R3Rect u = { 0, 0, 0, 0 };
    for (int i = 0; i < r->ntiles; i++) {
        Tile *tl = &r->tiles[i];
        for (int j = 0; j < tl->tris.n; j++) {
            Tri *t = &tl->tris.v[j];
            if (t->group != group) continue;
            t->hid = hidden != 0;
            u = join(u, (R3Rect) { t->x0, t->y0, t->x1, t->y1 });
        }
    }
    redraw(r, u);
}

void r3_light(R3 *r, int z, int x0, int y0, int w, int h, const double *v) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) put_light(r, x0 + i, y0 + j, z, v[j * w + i]);
    /* The triangles lit from those cells, lit again where their light changed; B drawn again
     * where they lie, at the next commit. */
    for (int ti = 0; ti < r->ntiles; ti++) {
        Tile *tl = &r->tiles[ti];
        if (tl->changed) continue; /* set up whole at the commit, with the light as it is then */
        R3Rect u = { 0, 0, 0, 0 };
        for (int j = 0; j < tl->tris.n; j++) {
            Tri *t = &tl->tris.v[j];
            int s = t->src;
            if (tl->lz[s] != z || tl->lx[s] < x0 || tl->lx[s] >= x0 + w || tl->ly[s] < y0 || tl->ly[s] >= y0 + h) continue;
            if (!light_corners(r, tl, s)) continue;
            Tri keep = *t;
            Corner *k = &tl->c[s * 3];
            set_up(r, t, &k[0], &k[1], &k[2]);
            t->pic = keep.pic;
            t->group = keep.group;
            t->hid = keep.hid;
            t->src = s;
            u = join(u, (R3Rect) { t->x0, t->y0, t->x1, t->y1 });
        }
        if (!empty(u)) { push_rect(&r->dirty, u); r->pending = 1; }
    }
}

int r3_light_at(const R3 *r, int x, int y, int z, double *v) { return light_of(r, x, y, z, v); }

/* --- The actors --- */

void r3_actors_begin(R3 *r) { r->act.n = 0; r->acts.n = 0; }

/* sin and cos of d degrees, without libm: the turn reduced to -90 .. 90 and a Taylor series to the
 * 13th power (an error under 1e-9 there).  Every target computes the same. */
static void sin_cos(double d, double *sn, double *cs) {
    double t = d - 360.0 * floor_d(d / 360.0 + 0.5);   /* -180 .. 180 */
    double sign = 1.0;
    if (t > 90.0) { t = 180.0 - t; sign = -1.0; }
    else if (t < -90.0) { t = -180.0 - t; sign = -1.0; }
    double x = t * 3.14159265358979323846 / 180.0, x2 = x * x;
    double s = x, c = 1.0, term = x, cterm = 1.0;
    for (int k = 1; k <= 6; k++) {
        term *= -x2 / (double) ((2 * k) * (2 * k + 1));
        cterm *= -x2 / (double) ((2 * k - 1) * (2 * k));
        s += term;
        c += cterm;
    }
    *sn = s;
    *cs = c * sign; /* cos(180 - t) = -cos(t); sin(180 - t) = sin(t) */
}

static R3Rect actor_rect(const R3 *r, const Tri *t);

void r3_actor(R3 *r, int id, int pic, double x, double y, double z, double size, double light) {
    r3_actor_pose(r, id, pic, x, y, z, size, light, 0.0, 0.5, 1.0, 0);
}

void r3_actor_pose(R3 *r, int id, int pic, double x, double y, double z, double size, double light, double deg,
                   double pu, double pv, int mirror) {
    if (pic < 0 || pic >= r->npics) return;
    if (r->acts.n == r->acts.cap) {
        r->acts.cap = r->acts.cap ? r->acts.cap * 2 : 16;
        r->acts.v = realloc(r->acts.v, (size_t) r->acts.cap * sizeof(Shown));
    }
    Shown *s = &r->acts.v[r->acts.n++];
    *s = (Shown) { id, pic, x, y, z, size, light, deg, pu, pv, mirror != 0, { 0, 0, 0, 0 }, 0, 0 };
    int first = r->act.n;
    /* The picture's corners about its pivot (pu, pv), turned `deg` clockwise as the camera sees
     * it: the card's plane faces the camera, and a cell across and a layer up are the same
     * pixels, so a turn there is the turn on the screen. */
    double sn = 0.0, cs = 1.0;
    if (deg != 0.0) sin_cos(deg, &sn, &cs);
    double bx = x - size / 2 + pu * size, by = y + (1.0 - pv) * size;
    static const double uv[4][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
    Corner c[4];
    for (int k = 0; k < 4; k++) {
        double px = (uv[k][0] - pu) * size, py = (pv - uv[k][1]) * size;
        c[k] = (Corner) { bx + px * cs + py * sn, by - px * sn + py * cs, z, mirror ? 1.0 - uv[k][0] : uv[k][0], uv[k][1], light };
    }
    for (int i = 1; i <= 2; i++) {
        Tri *t = push_tri(&r->act);
        if (!set_up(r, t, &c[0], &c[i], &c[i + 1])) { r->act.n--; continue; }
        t->pic = pic;
        t->group = -1;
    }
    s->t0 = first;
    s->tn = r->act.n - first;
    for (int i = first; i < r->act.n; i++) {
        R3Rect b = actor_rect(r, &r->act.v[i]);
        if (i == first) s->rect = b;
        else {
            s->rect.x0 = min(s->rect.x0, b.x0); s->rect.y0 = min(s->rect.y0, b.y0);
            s->rect.x1 = max(s->rect.x1, b.x1); s->rect.y1 = max(s->rect.y1, b.y1);
        }
    }
}

static R3Rect actor_rect(const R3 *r, const Tri *t) {
    R3Rect b = { t->x0 - r->cx, t->y0 - r->cy, t->x1 - r->cx, t->y1 - r->cy };
    return clip_screen(b);
}

/* The overlay over `buf` inside c (screen pixels), where it is not clear. */
static void overlay(const R3 *r, uint16_t *buf, R3Rect c) {
    if (!r->ov) return;
    for (int x = c.x0; x < c.x1; x++) {
        int ox = x / r->os;
        if (ox >= r->ow) break;
        uint16_t *col = buf + R3_AT(x, 0);
        for (int y = c.y0; y < c.y1; y++) {
            int oy = y / r->os;
            if (oy >= r->oh) break;
            uint16_t o = r->ov[oy * r->ow + ox];
            if (o != R3_NONE) col[y] = o;
        }
    }
}

void r3_overlay(R3 *r, const uint16_t *pix, int w, int h, int scale) {
    static const R3Rect whole = { 0, 0, R3_SW, R3_SH };
    if (w <= 0 || h <= 0 || scale <= 0 || !pix) {
        if (r->ov) { free(r->ov); r->ov = NULL; stale_both(r, whole); }
        return;
    }
    if (!r->ov || w != r->ow || h != r->oh || scale != r->os) {
        free(r->ov);
        r->ov = malloc((size_t) w * h * sizeof(uint16_t));
        memcpy(r->ov, pix, (size_t) w * h * sizeof(uint16_t));
        r->ow = w; r->oh = h; r->os = scale;
        stale_both(r, whole);
        return;
    }
    /* Where it changed: drawn again there in both framebuffers. */
    int x0 = w, y0 = h, x1 = -1, y1 = -1;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (r->ov[y * w + x] != pix[y * w + x]) {
                if (x < x0) x0 = x;
                if (x > x1) x1 = x;
                if (y < y0) y0 = y;
                if (y > y1) y1 = y;
            }
    if (x1 < 0) return;
    memcpy(r->ov, pix, (size_t) w * h * sizeof(uint16_t));
#ifndef R3_SKIP_DIRTY /* the check's control: an overlay change not drawn again */
    stale_both(r, clip_screen((R3Rect) { x0 * scale, y0 * scale, (x1 + 1) * scale, (y1 + 1) * scale }));
#endif
}

/* The framebuffer made from B inside c, and the actors over it, and the overlay over them. */
static void compose(R3 *r, uint16_t *fb, R3Rect c) {
    c = clip_screen(c);
    if (c.x0 >= c.x1 || c.y0 >= c.y1) return;
    size_t n = (size_t) (c.y1 - c.y0);
    for (int x = c.x0; x < c.x1; x++) {
        memcpy(fb + R3_AT(x, c.y0), r->b_col + R3_AT(x, c.y0), n * sizeof(uint16_t));
        memcpy(r->f_dep + R3_AT(x, c.y0), r->b_dep + R3_AT(x, c.y0), n * sizeof(uint16_t));
    }
    r->cnt.fb_px += (int64_t) (c.x1 - c.x0) * (int64_t) n;
    r->cnt.rects++;
    push_rect(&r->made, c);
    for (int i = 0; i < r->act.n; i++) r->cnt.actor_px += span(r, &r->act.v[i], c, r->cx, r->cy, fb, r->f_dep);
    overlay(r, fb, c);
}

static int same_actor(const Shown *a, const Shown *b) {
    return a->id == b->id && a->pic == b->pic && a->x == b->x && a->y == b->y && a->z == b->z && a->size == b->size
           && a->light == b->light && a->deg == b->deg && a->pu == b->pu && a->pv == b->pv && a->mirror == b->mirror;
}

void r3_frame(R3 *r, uint16_t *fb, int which) {
    static const R3Rect whole = { 0, 0, R3_SW, R3_SH };
    commit(r);
    Buffer *b = &r->fb[which & 1];
    /* What changed for this buffer: the landscape drawn again since it was made, and each actor
     * that is not as this buffer drew it — where it was, and where it is. */
    Rects todo = { 0, 0, 0 };
    for (int i = 0; i < b->stale.n; i++) push_rect(&todo, b->stale.v[i]);
    for (int i = 0; i < r->acts.n; i++) {
        const Shown *a = &r->acts.v[i];
        int kept = 0;
        for (int j = 0; j < b->shown.n; j++)
            if (b->shown.v[j].id == a->id) {
                if (same_actor(a, &b->shown.v[j])) kept = 1;
                else push_rect(&todo, b->shown.v[j].rect);
                break;
            }
        if (!kept) push_rect(&todo, a->rect);
    }
    for (int j = 0; j < b->shown.n; j++) {
        int still = 0;
        for (int i = 0; i < r->acts.n && !still; i++) still = r->acts.v[i].id == b->shown.v[j].id;
        if (!still) push_rect(&todo, b->shown.v[j].rect);
    }
    int64_t area = 0;
    for (int i = 0; i < todo.n; i++) area += (int64_t) (todo.v[i].x1 - todo.v[i].x0) * (todo.v[i].y1 - todo.v[i].y0);
    r->made.n = 0;
    r->whole = b->all || area * 2 > (int64_t) R3_SW * R3_SH;
    if (r->whole) compose(r, fb, whole);
    else for (int i = 0; i < todo.n; i++) compose(r, fb, todo.v[i]);
    free(todo.v);
    b->all = 0;
    b->stale.n = 0;
    if (b->shown.cap < r->acts.n) {
        b->shown.cap = r->acts.n;
        b->shown.v = realloc(b->shown.v, (size_t) b->shown.cap * sizeof(Shown));
    }
    if (r->acts.n) memcpy(b->shown.v, r->acts.v, (size_t) r->acts.n * sizeof(Shown));
    b->shown.n = r->acts.n;
}

void r3_reference(R3 *r, uint16_t *col) {
    static const R3Rect whole = { 0, 0, R3_SW, R3_SH };
    commit(r);
    R3Count keep = r->cnt; /* the check is not the frame's work */
    draw_land(r, whole, col, r->r_dep);
    for (int i = 0; i < r->act.n; i++) span(r, &r->act.v[i], whole, r->cx, r->cy, col, r->r_dep);
    overlay(r, col, whole);
    r->cnt = keep;
}

R3Count r3_count(R3 *r) {
    R3Count c = r->cnt;
    c.uncovered = r->uncovered.n;
    c.lights = r->ln;
    memset(&r->cnt, 0, sizeof r->cnt);
    return c;
}

/* --- The hero is never hidden --- */

void r3_uncover(R3 *r, int id) {
    commit(r);
    int first = -1, count = 0;
    for (int i = 0; i < r->acts.n; i++)
        if (r->acts.v[i].id == id) { first = r->acts.v[i].t0; count = r->acts.v[i].tn; break; }
    R3Rect c = { 0, 0, 0, 0 };
    int near = 65535;
    for (int i = first; first >= 0 && i < first + count && i < r->act.n; i++) {
        const Tri *t = &r->act.v[i];
        c = join(c, (R3Rect) { t->tx0, t->ty0, t->tx1, t->ty1 });
        if (t->dmin < near) near = t->dmin;
    }
    /* The wall groups under the actor's rectangle (world pixels) that reach nearer than it. */
    Ints want = { 0, 0, 0 };
    if (!empty(c))
        for (int i = 0; i < r->ntiles; i++) {
            const Tile *tl = &r->tiles[i];
            if (!meets(tl->box, c)) continue;
            for (int j = 0; j < tl->tris.n; j++) {
                const Tri *t = &tl->tris.v[j];
                if (t->group < 0 || t->dmin >= near) continue;
                if (!meets((R3Rect) { t->tx0, t->ty0, t->tx1, t->ty1 }, c)) continue;
                if (!in_ints(&want, t->group)) push_int(&want, t->group);
            }
        }
    for (int i = 0; i < r->uncovered.n; i++)
        if (!in_ints(&want, r->uncovered.v[i])) r3_hide(r, r->uncovered.v[i], 0);
    for (int i = 0; i < want.n; i++) r3_hide(r, want.v[i], 1);
    free(r->uncovered.v);
    r->uncovered = want;
}

/* --- Handles, for loft --- */

typedef struct { R3 *r; uint16_t *fb; int fresh; } Handle;
static Handle *handles;
static int64_t nhandles;

static Handle *handle(int64_t h) { return h >= 1 && h <= nhandles && handles[h - 1].r ? &handles[h - 1] : NULL; }

int64_t r3h_new(int32_t sky) {
    int64_t i;
    for (i = 0; i < nhandles && handles[i].r; i++) {}
    if (i == nhandles) { handles = realloc(handles, (size_t) (nhandles + 1) * sizeof(Handle)); nhandles++; }
    handles[i].r = r3_new((uint16_t) sky);
    handles[i].fb = calloc((size_t) R3_SW * R3_SH, sizeof(uint16_t));
    handles[i].fresh = 1;
    return i + 1;
}

void r3h_free(int64_t h) {
    Handle *k = handle(h);
    if (!k) return;
    r3_free(k->r);
    free(k->fb);
    k->r = NULL;
}

int64_t r3h_picture(int64_t h, const int32_t *texels) {
    Handle *k = handle(h);
    if (!k) return -1;
    uint16_t t[R3_TS * R3_TS];
    for (int i = 0; i < R3_TS * R3_TS; i++) t[i] = (uint16_t) texels[i];
    return r3_picture(k->r, t);
}

/* Faces, one after another: picture, group, corners n, where the light comes from (x, y, z, k;
 * z R3_NO_LIGHT: none, the corners' own), then n times x y z u v light. */
void r3h_faces(int64_t h, const double *d, int64_t len) {
    Handle *k = handle(h);
    if (!k) return;
    int64_t i = 0;
    while (i + 7 <= len) {
        int pic = (int) d[i], group = (int) d[i + 1], n = (int) d[i + 2];
        if (n < 3 || n > 8 || i + 7 + n * 6 > len) return;
        r3_face_lit(k->r, pic, n, d + i + 7, group, (int) d[i + 3], (int) d[i + 4], (int) d[i + 5], d[i + 6]);
        i += 7 + n * 6;
    }
}

void r3h_light(int64_t h, const double *box, const double *v, int64_t len) {
    Handle *k = handle(h);
    if (!k) return;
    int z = (int) box[0], x0 = (int) box[1], y0 = (int) box[2], w = (int) box[3], hh = (int) box[4];
    if (w <= 0 || hh <= 0 || (int64_t) w * hh > len) return;
    r3_light(k->r, z, x0, y0, w, hh, v);
}

/* The overlay: w x hh pixels row by row (R3_NONE clear), each scale x scale on the screen. */
void r3h_overlay(int64_t h, const int32_t *pix, int64_t w, int64_t hh, int64_t scale) {
    Handle *k = handle(h);
    if (!k) return;
    if (w <= 0 || hh <= 0) { r3_overlay(k->r, NULL, 0, 0, 0); return; }
    uint16_t *t = malloc((size_t) (w * hh) * sizeof(uint16_t));
    for (int64_t i = 0; i < w * hh; i++) t[i] = (uint16_t) pix[i];
    r3_overlay(k->r, t, (int) w, (int) hh, (int) scale);
    free(t);
}

int64_t r3h_lights(int64_t h) {
    Handle *k = handle(h);
    return k ? k->r->ln : 0;
}

double r3h_light_at(int64_t h, int64_t x, int64_t y, int64_t z) {
    Handle *k = handle(h);
    double v;
    if (!k || !r3_light_at(k->r, (int) x, (int) y, (int) z, &v)) return -1;
    return v;
}

/* Tile `key`'s faces replaced by these (as r3h_faces takes them; none: the tile is empty). */
void r3h_tile(int64_t h, int64_t key, const double *d, int64_t len) {
    Handle *k = handle(h);
    if (!k) return;
    r3_tile(k->r, (int) key);
    r3h_faces(h, d, len);
}

void r3h_drop(int64_t h, int64_t key) {
    Handle *k = handle(h);
    if (k) r3_drop(k->r, (int) key);
}

void r3h_build(int64_t h, const double *origin) {
    Handle *k = handle(h);
    if (k) r3_build(k->r, origin[0], origin[1], origin[2]);
}

void r3h_camera(int64_t h, int64_t px, int64_t py) {
    Handle *k = handle(h);
    if (k) r3_camera(k->r, (int) px, (int) py);
}

/* Actors: id, picture, x, y, z, size, light, degrees, pivot u, pivot v, mirrored each; then the one
 * never hidden (-1: none). */
void r3h_actors(int64_t h, const double *d, int64_t len, int64_t keep_seen) {
    Handle *k = handle(h);
    if (!k) return;
    r3_actors_begin(k->r);
    for (int64_t i = 0; i + 11 <= len; i += 11)
        r3_actor_pose(k->r, (int) d[i], (int) d[i + 1], d[i + 2], d[i + 3], d[i + 4], d[i + 5], d[i + 6], d[i + 7], d[i + 8],
                      d[i + 9], d[i + 10] != 0.0);
    if (keep_seen >= 0) r3_uncover(k->r, (int) keep_seen);
}

/* The frame, into `out`: R3_SW x R3_SH RGB565 values row by row, as a screen takes them.  Only
 * what changed is written, so `out` must hold the last frame this handle made. */
void r3h_frame(int64_t h, int32_t *out) {
    Handle *k = handle(h);
    if (!k) return;
    R3 *r = k->r;
    r3_frame(r, k->fb, 0);
    if (k->fresh || r->whole) {
        for (int y = 0; y < R3_SH; y++)
            for (int x = 0; x < R3_SW; x++) out[y * R3_SW + x] = k->fb[R3_AT(x, y)];
        k->fresh = 0;
        return;
    }
    for (int i = 0; i < r->made.n; i++) {
        R3Rect c = r->made.v[i];
        for (int y = c.y0; y < c.y1; y++)
            for (int x = c.x0; x < c.x1; x++) out[y * R3_SW + x] = k->fb[R3_AT(x, y)];
    }
}
