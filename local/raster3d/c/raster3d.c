/* raster3d — see raster3d.h. */
#include "raster3d.h"

#include <stdlib.h>
#include <string.h>

#define SUB 16   /* subpixel steps a pixel */
#define FIX 24   /* fractional bits of a stepped attribute */
#define BIN 64   /* the side of a bin, in pixels */
#define NA 4     /* attributes: depth, u, v, light */

typedef struct {
    int32_t X[3], Y[3];          /* world subpixels */
    int64_t ea[3], eb[3], ec[3]; /* edge i: ea*px + eb*py + ec + bias >= 0 inside */
    int64_t base[NA], gx[NA], gy[NA];
    int x0, y0, x1, y1;          /* world pixels, half-open, a pixel round */
    int tx0, ty0, tx1, ty1;      /* the same, tight: the pixels the corners reach into */
    int dmin;                    /* its nearest depth */
    int pic, group;
} Tri;

typedef struct { Tri *v; int n, cap; } Tris;
typedef struct { R3Rect *v; int n, cap; } Rects;

/* An actor as drawn: what it was asked to be, and where that put it. */
typedef struct { int id, pic; double x, y, z, size, light; R3Rect rect; int t0, tn; } Shown;
typedef struct { Shown *v; int n, cap; } Showns;

typedef struct {
    int all;      /* to be made whole */
    Rects stale;  /* landscape drawn again since it was last made */
    Showns shown; /* its actors, as it last drew them */
} Buffer;

typedef struct { double x, y, z, u, v, l; } Corner;

struct R3 {
    uint16_t sky;
    Corner *pend;                /* the faces' triangles until r3_build: three corners each */
    int *pend_pic, *pend_group, npend, cappend;
    uint16_t (*pics)[R3_TS * R3_TS];
    int npics, cappics;
    Tris land, act;
    Showns acts;                 /* this frame's actors: act holds their triangles, two each */
    double ox, oy, oz;           /* the depth's origin */
    /* bins over the landscape's world pixels */
    int bx0, by0, nbx, nby;
    int **bin, *binn;
    int *mark, stamp, *pick;
    /* groups */
    int ngroups;
    int *gstart, *gtris;
    uint8_t *hidden;
    /* the camera and B */
    int cx, cy, placed;
    uint16_t *b_col, *b_dep, *f_dep, *r_dep;
    Buffer fb[2];
    R3Count cnt;
    Rects made;                  /* the rectangles the last frame made */
    int whole;                   /* ... or all of it */
    int *uncovered, nuncovered;  /* the groups r3_uncover hid */
    uint8_t *want;
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
    return r;
}

void r3_free(R3 *r) {
    if (!r) return;
    for (int i = 0; i < r->nbx * r->nby; i++) free(r->bin[i]);
    free(r->bin); free(r->binn); free(r->mark); free(r->pick);
    free(r->gstart); free(r->gtris); free(r->hidden); free(r->want); free(r->uncovered); free(r->made.v);
    free(r->land.v); free(r->act.v); free(r->acts.v); free(r->pics);
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

void r3_face(R3 *r, int pic, int n, const double *c, int group) {
    if (pic < 0 || pic >= r->npics || n < 3) return;
    const Corner *k = (const Corner *) c;
    for (int i = 1; i + 1 < n; i++) {
        if (r->npend == r->cappend) {
            r->cappend = r->cappend ? r->cappend * 2 : 1024;
            r->pend = realloc(r->pend, (size_t) r->cappend * 3 * sizeof(Corner));
            r->pend_pic = realloc(r->pend_pic, (size_t) r->cappend * sizeof(int));
            r->pend_group = realloc(r->pend_group, (size_t) r->cappend * sizeof(int));
        }
        Corner *t = &r->pend[r->npend * 3];
        t[0] = k[0]; t[1] = k[i]; t[2] = k[i + 1];
        r->pend_pic[r->npend] = pic;
        r->pend_group[r->npend] = group;
        r->npend++;
    }
}

void r3_build(R3 *r, double ox, double oy, double oz) {
    r->ox = ox; r->oy = oy; r->oz = oz;
    for (int i = 0; i < r->npend; i++) {
        Tri *t = push_tri(&r->land);
        Corner *k = &r->pend[i * 3];
        if (!set_up(r, t, &k[0], &k[1], &k[2])) { r->land.n--; continue; }
        t->pic = r->pend_pic[i];
        t->group = r->pend_group[i];
    }
    free(r->pend); free(r->pend_pic); free(r->pend_group);
    r->pend = NULL; r->pend_pic = r->pend_group = NULL; r->npend = r->cappend = 0;
    int n = r->land.n;
    /* Bins over the world pixels the landscape covers. */
    int x0 = 0, y0 = 0, x1 = 1, y1 = 1;
    for (int i = 0; i < n; i++) {
        Tri *t = &r->land.v[i];
        if (i == 0 || t->x0 < x0) x0 = t->x0;
        if (i == 0 || t->y0 < y0) y0 = t->y0;
        if (i == 0 || t->x1 > x1) x1 = t->x1;
        if (i == 0 || t->y1 > y1) y1 = t->y1;
    }
    r->bx0 = (int) floor_div(x0, BIN) * BIN;
    r->by0 = (int) floor_div(y0, BIN) * BIN;
    r->nbx = (x1 - r->bx0) / BIN + 1;
    r->nby = (y1 - r->by0) / BIN + 1;
    r->bin = calloc((size_t) (r->nbx * r->nby), sizeof(int *));
    r->binn = calloc((size_t) (r->nbx * r->nby), sizeof(int));
    int *cap = calloc((size_t) (r->nbx * r->nby), sizeof(int));
    for (int i = 0; i < n; i++) {
        Tri *t = &r->land.v[i];
        for (int by = (t->y0 - r->by0) / BIN; by <= (t->y1 - 1 - r->by0) / BIN; by++)
            for (int bx = (t->x0 - r->bx0) / BIN; bx <= (t->x1 - 1 - r->bx0) / BIN; bx++) {
                int b = by * r->nbx + bx;
                if (r->binn[b] == cap[b]) { cap[b] = cap[b] ? cap[b] * 2 : 8; r->bin[b] = realloc(r->bin[b], (size_t) cap[b] * sizeof(int)); }
                r->bin[b][r->binn[b]++] = i;
            }
    }
    free(cap);
    r->mark = calloc((size_t) n + 1, sizeof(int));
    r->pick = malloc(((size_t) n + 1) * sizeof(int));
    /* Groups: each group's triangles, in order. */
    r->ngroups = 0;
    for (int i = 0; i < n; i++) if (r->land.v[i].group + 1 > r->ngroups) r->ngroups = r->land.v[i].group + 1;
    r->gstart = calloc((size_t) r->ngroups + 1, sizeof(int));
    r->gtris = malloc(((size_t) n + 1) * sizeof(int));
    r->hidden = calloc((size_t) r->ngroups + 1, 1);
    r->want = calloc((size_t) r->ngroups + 1, 1);
    r->uncovered = malloc(((size_t) r->ngroups + 1) * sizeof(int));
    for (int i = 0; i < n; i++) if (r->land.v[i].group >= 0) r->gstart[r->land.v[i].group + 1]++;
    for (int g = 0; g < r->ngroups; g++) r->gstart[g + 1] += r->gstart[g];
    int *fill = calloc((size_t) r->ngroups + 1, sizeof(int));
    for (int i = 0; i < n; i++) {
        int g = r->land.v[i].group;
        if (g >= 0) r->gtris[r->gstart[g] + fill[g]++] = i;
    }
    free(fill);
}

static int cmp_int(const void *a, const void *b) { return *(const int *) a - *(const int *) b; }

/* B drawn from scratch inside screen rectangle c. */
static void draw_land(R3 *r, R3Rect c, uint16_t *col, uint16_t *dep) {
    c = clip_screen(c);
    if (c.x0 >= c.x1 || c.y0 >= c.y1) return;
    for (int x = c.x0; x < c.x1; x++) {
        uint16_t *cp = col + R3_AT(x, 0), *dp = dep + R3_AT(x, 0);
        for (int y = c.y0; y < c.y1; y++) { cp[y] = r->sky; dp[y] = 65535; }
    }
    r->cnt.b_px += (int64_t) (c.x1 - c.x0) * (c.y1 - c.y0);
    /* The triangles in the bins under it, once each, in the order they were made. */
    int wx0 = c.x0 + r->cx - r->bx0, wx1 = c.x1 - 1 + r->cx - r->bx0;
    int wy0 = c.y0 + r->cy - r->by0, wy1 = c.y1 - 1 + r->cy - r->by0;
    int bx0 = max(wx0 / BIN - (wx0 < 0), 0), bx1 = min(wx1 < 0 ? -1 : wx1 / BIN, r->nbx - 1);
    int by0 = max(wy0 / BIN - (wy0 < 0), 0), by1 = min(wy1 < 0 ? -1 : wy1 / BIN, r->nby - 1);
    r->stamp++;
    int np = 0;
    for (int by = by0; by <= by1; by++)
        for (int bx = bx0; bx <= bx1; bx++) {
            int b = by * r->nbx + bx;
            for (int j = 0; j < r->binn[b]; j++) {
                int i = r->bin[b][j];
                if (r->mark[i] != r->stamp) { r->mark[i] = r->stamp; r->pick[np++] = i; }
            }
        }
    qsort(r->pick, (size_t) np, sizeof(int), cmp_int);
    for (int j = 0; j < np; j++) {
        const Tri *t = &r->land.v[r->pick[j]];
        if (t->group >= 0 && r->hidden[t->group]) continue;
        r->cnt.tris++;
        span(r, t, c, r->cx, r->cy, col, dep);
    }
}

static void stale_both(R3 *r, R3Rect c) {
    for (int k = 0; k < 2; k++) if (!r->fb[k].all) push_rect(&r->fb[k].stale, c);
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
    if (group < 0 || group >= r->ngroups || r->hidden[group] == (hidden != 0)) return;
    r->hidden[group] = hidden != 0;
    R3Rect u = { 0, 0, 0, 0 };
    int any = 0;
    for (int j = r->gstart[group]; j < r->gstart[group + 1]; j++) {
        const Tri *t = &r->land.v[r->gtris[j]];
        R3Rect b = { t->x0 - r->cx, t->y0 - r->cy, t->x1 - r->cx, t->y1 - r->cy };
        if (!any) u = b;
        else { u.x0 = min(u.x0, b.x0); u.y0 = min(u.y0, b.y0); u.x1 = max(u.x1, b.x1); u.y1 = max(u.y1, b.y1); }
        any = 1;
    }
    if (!any || !r->placed) return;
    u = clip_screen(u);
    draw_land(r, u, r->b_col, r->b_dep);
    stale_both(r, u);
}

/* --- The actors --- */

void r3_actors_begin(R3 *r) { r->act.n = 0; r->acts.n = 0; }

static R3Rect actor_rect(const R3 *r, const Tri *t);

void r3_actor(R3 *r, int id, int pic, double x, double y, double z, double size, double light) {
    if (pic < 0 || pic >= r->npics) return;
    if (r->acts.n == r->acts.cap) {
        r->acts.cap = r->acts.cap ? r->acts.cap * 2 : 16;
        r->acts.v = realloc(r->acts.v, (size_t) r->acts.cap * sizeof(Shown));
    }
    Shown *s = &r->acts.v[r->acts.n++];
    *s = (Shown) { id, pic, x, y, z, size, light, { 0, 0, 0, 0 }, 0, 0 };
    int first = r->act.n;
    double h = size / 2;
    Corner c[4] = {
        { x - h, y + size, z, 0, 0, light }, { x + h, y + size, z, 1, 0, light },
        { x + h, y, z, 1, 1, light }, { x - h, y, z, 0, 1, light },
    };
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

/* The framebuffer made from B inside c, and the actors over it. */
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
}

static int same_actor(const Shown *a, const Shown *b) {
    return a->id == b->id && a->pic == b->pic && a->x == b->x && a->y == b->y && a->z == b->z && a->size == b->size
           && a->light == b->light;
}

void r3_frame(R3 *r, uint16_t *fb, int which) {
    static const R3Rect whole = { 0, 0, R3_SW, R3_SH };
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
    R3Count keep = r->cnt; /* the check is not the frame's work */
    draw_land(r, whole, col, r->r_dep);
    for (int i = 0; i < r->act.n; i++) span(r, &r->act.v[i], whole, r->cx, r->cy, col, r->r_dep);
    r->cnt = keep;
}

R3Count r3_count(R3 *r) {
    R3Count c = r->cnt;
    c.uncovered = r->nuncovered;
    memset(&r->cnt, 0, sizeof r->cnt);
    return c;
}

/* --- The hero is never hidden --- */

void r3_uncover(R3 *r, int id) {
    int first = -1, count = 0;
    for (int i = 0; i < r->acts.n; i++)
        if (r->acts.v[i].id == id) { first = r->acts.v[i].t0; count = r->acts.v[i].tn; break; }
    R3Rect c = { 0, 0, 0, 0 };
    int near = 65535;
    int found = 0;
    for (int i = first; first >= 0 && i < first + count && i < r->act.n; i++) {
        const Tri *t = &r->act.v[i];
        R3Rect b = { t->tx0 - r->cx, t->ty0 - r->cy, t->tx1 - r->cx, t->ty1 - r->cy };
        if (!found) c = b;
        else { c.x0 = min(c.x0, b.x0); c.y0 = min(c.y0, b.y0); c.x1 = max(c.x1, b.x1); c.y1 = max(c.y1, b.y1); }
        if (t->dmin < near) near = t->dmin;
        found = 1;
    }
    memset(r->want, 0, (size_t) r->ngroups + 1);
    if (found && r->nbx > 0) {
        /* The wall groups under the actor's rectangle that reach nearer than it. */
        int wx0 = c.x0 + r->cx - r->bx0, wx1 = c.x1 - 1 + r->cx - r->bx0;
        int wy0 = c.y0 + r->cy - r->by0, wy1 = c.y1 - 1 + r->cy - r->by0;
        int bx0 = max(wx0 < 0 ? -1 : wx0 / BIN, 0), bx1 = min(wx1 < 0 ? -1 : wx1 / BIN, r->nbx - 1);
        int by0 = max(wy0 < 0 ? -1 : wy0 / BIN, 0), by1 = min(wy1 < 0 ? -1 : wy1 / BIN, r->nby - 1);
        for (int by = by0; by <= by1; by++)
            for (int bx = bx0; bx <= bx1; bx++) {
                int b = by * r->nbx + bx;
                for (int j = 0; j < r->binn[b]; j++) {
                    const Tri *t = &r->land.v[r->bin[b][j]];
                    if (t->group < 0 || t->dmin >= near) continue;
                    if (t->tx1 - r->cx <= c.x0 || t->tx0 - r->cx >= c.x1 || t->ty1 - r->cy <= c.y0 || t->ty0 - r->cy >= c.y1) continue;
                    r->want[t->group] = 1;
                }
            }
    }
    int n = 0;
    for (int i = 0; i < r->nuncovered; i++) {
        int g = r->uncovered[i];
        if (!r->want[g]) r3_hide(r, g, 0);
    }
    for (int g = 0; g < r->ngroups; g++)
        if (r->want[g]) { r3_hide(r, g, 1); r->uncovered[n++] = g; }
    r->nuncovered = n;
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

/* Faces, one after another: picture, group, corners n, then n times x y z u v light. */
void r3h_faces(int64_t h, const double *d, int64_t len) {
    Handle *k = handle(h);
    if (!k) return;
    int64_t i = 0;
    while (i + 3 <= len) {
        int pic = (int) d[i], group = (int) d[i + 1], n = (int) d[i + 2];
        if (n < 3 || n > 8 || i + 3 + n * 6 > len) return;
        r3_face(k->r, pic, n, d + i + 3, group);
        i += 3 + n * 6;
    }
}

void r3h_build(int64_t h, const double *origin) {
    Handle *k = handle(h);
    if (k) r3_build(k->r, origin[0], origin[1], origin[2]);
}

void r3h_camera(int64_t h, int64_t px, int64_t py) {
    Handle *k = handle(h);
    if (k) r3_camera(k->r, (int) px, (int) py);
}

/* Actors: id, picture, x, y, z, size, light each; then the one never hidden (-1: none). */
void r3h_actors(int64_t h, const double *d, int64_t len, int64_t keep_seen) {
    Handle *k = handle(h);
    if (!k) return;
    r3_actors_begin(k->r);
    for (int64_t i = 0; i + 7 <= len; i += 7)
        r3_actor(k->r, (int) d[i], (int) d[i + 1], d[i + 2], d[i + 3], d[i + 4], d[i + 5], d[i + 6]);
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
