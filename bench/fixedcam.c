/* The fixed camera from the south, drawn on a CPU, and what its movements cost (BUILD.md Phase E,
 * E0 and E2; RENDER_3D.md § The camera and what is seen).
 *
 * The landscape is drawn into a kept picture B, its colour and its depth, and a frame reuses it.
 * When the camera moves, B is shifted by the pixels the picture moved and only the strips that
 * came into view are drawn.  That is right only when every point of the landscape moves by the
 * same whole number of pixels, which is what this bench checks: each movement made by shifting
 * and drawing the strips, then the same picture drawn whole from scratch, and the two compared
 * pixel for pixel, colour and depth.
 *
 * Two projections:
 *   parallel     a cell is PX pixels wide, its ground PZ deep, a layer PY tall; the camera's place
 *                is a whole pixel offset, so a move is an exact shift (the 2D's own projection,
 *                with the ground a little shallower than a wall is tall);
 *   perspective  40 degrees down, a 25 degree field, a cell PX pixels wide at the view's middle
 *                (RENDER_3D.md § Defaults): near rows move more than far ones, so a shift is
 *                not the picture and the bench counts how far off it is.
 * The rasteriser is the same for both: triangles in 1/16 pixel, edge functions in integers with
 * a fill rule that gives a shared edge's pixels to one side only, texels sampled nearest, the
 * rules' light per corner, a 16-bit depth test.  Its answers depend only on where a pixel lies
 * relative to the triangle, so drawing a strip draws exactly what a whole picture has there.
 *
 * The scene is a place's landscape as `render3d` builds it, written by tools/scene3d.loft.
 * Buffers are laid out as the panel is (portrait: screen column x is panel row x), as in
 * bench/actors.c.
 *
 *   bench/fixedcam SCENE [OUT_DIR]     OUT_DIR: the pictures, as PPM
 * Build and run on this machine and under the emulator: bench/run.sh. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SW 800
#define SH 480
#define AT(x, y) ((x) * SH + (y))
#define SUB 16 /* subpixel steps a pixel */
#define TS 16  /* a picture's texels each way */
#define NONE 63519 /* a texel that is not there */

/* The parallel projection: pixels a cell across, a cell's ground deep, a layer tall. */
#define PX 48
#define PZ 40
#define PY 48
/* The perspective one. */
#define PITCH 40.0
#define FIELD 25.0

typedef struct { int sprite, card, n; double c[8][6]; } Face; /* corners: x y z u v light */
typedef struct { int id; uint16_t t[TS * TS]; } Pic;

static Pic *pics;
static int npics;
static Face *faces;
static int nfaces;
static int spot[3];

static uint16_t b_col[SW * SH], b_dep[SW * SH], r_col[SW * SH], r_dep[SW * SH], tmp[SW * SH];
static uint16_t sky;

typedef struct { int64_t tris, tested, written; } Count;
static Count cnt;

static double now_us(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double) t.tv_sec * 1e6 + (double) t.tv_nsec / 1e3;
}

static int min(int a, int b) { return a < b ? a : b; }
static int max(int a, int b) { return a > b ? a : b; }

/* --- The scene --- */

static int pic_of(int id) {
    for (int i = 0; i < npics; i++) if (pics[i].id == id) return i;
    return -1;
}

static FILE *in;
static int rd_int(void) { int v; if (fscanf(in, "%d", &v) != 1) { fprintf(stderr, "a scene cut short\n"); exit(1); } return v; }
static double rd_dbl(void) { double v; if (fscanf(in, "%lf", &v) != 1) { fprintf(stderr, "a scene cut short\n"); exit(1); } return v; }

static void load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "cannot read %s\n", path); exit(1); }
    in = f;
    int version;
    if (fscanf(f, "scene3d %d spot %d %d %d", &version, &spot[0], &spot[1], &spot[2]) != 4) {
        fprintf(stderr, "%s: not a scene\n", path);
        exit(1);
    }
    int cap_f = 1024, cap_p = 64;
    faces = malloc((size_t) cap_f * sizeof(Face));
    pics = malloc((size_t) cap_p * sizeof(Pic));
    char tag[8];
    for (;;) {
        if (fscanf(f, "%7s", tag) != 1) { fprintf(stderr, "%s: cut short\n", path); exit(1); }
        if (strcmp(tag, "end") == 0) break;
        if (strcmp(tag, "f") == 0) {
            if (nfaces == cap_f) { cap_f *= 2; faces = realloc(faces, (size_t) cap_f * sizeof(Face)); }
            Face *fc = &faces[nfaces++];
            fc->sprite = rd_int(); /* the picture's id until every picture is read */
            fc->card = rd_int();
            fc->n = rd_int();
            if (fc->n > 8) { fprintf(stderr, "a face of %d corners\n", fc->n); exit(1); }
            for (int c = 0; c < fc->n; c++)
                for (int k = 0; k < 6; k++) fc->c[c][k] = rd_dbl();
        } else if (strcmp(tag, "p") == 0) {
            if (npics == cap_p) { cap_p *= 2; pics = realloc(pics, (size_t) cap_p * sizeof(Pic)); }
            Pic *pc = &pics[npics++];
            pc->id = rd_int();
            for (int t = 0; t < TS * TS; t++) pc->t[t] = (uint16_t) rd_int();
        } else {
            fprintf(stderr, "%s: a record '%s'\n", path, tag);
            exit(1);
        }
    }
    fclose(f);
    for (int i = 0; i < nfaces; i++) faces[i].sprite = pic_of(faces[i].sprite);
}

/* --- The cameras --- */

typedef struct {
    int persp;
    int ox, oy;         /* parallel: the world's origin at these pixels, negated */
    double tx, ty, tz;  /* perspective: the point the view's middle looks at */
} Cam;

typedef struct { int32_t x, y; double iw, depth; } Vert; /* screen place in 1/16 pixel; 1/w; depth */

static double focal(void) { return (SH / 2.0) / tan(FIELD / 2.0 * M_PI / 180.0); }

static int project(const Cam *cam, const double *p, Vert *v) {
    if (!cam->persp) {
        /* Rounded before the camera's whole pixels come off, so a moved camera moves every
         * vertex by exactly the same number of steps. */
        v->x = (int32_t) lround(p[0] * PX * SUB) - cam->ox * SUB;
        v->y = (int32_t) lround((p[2] * PZ - p[1] * PY) * SUB) - cam->oy * SUB;
        v->iw = 1.0;
        /* Nearer is smaller: south and up come towards the camera. */
        double d = (p[1] - spot[2]) * PZ + (p[2] - spot[1]) * PY;
        v->depth = 32768.0 - 16.0 * d;
        return 1;
    }
    double s = sin(PITCH * M_PI / 180.0), c = cos(PITCH * M_PI / 180.0);
    double dist = focal() / PX;
    double ex = cam->tx, ey = cam->ty + dist * s, ez = cam->tz + dist * c;
    double dx = p[0] - ex, dy = p[1] - ey, dz = p[2] - ez;
    double vx = dx, vy = dy * c - dz * s, vz = -dy * s - dz * c; /* right, up, forward */
    if (vz < 0.5) return 0;
    double f = focal();
    v->x = (int32_t) lround((SW / 2.0 + f * vx / vz) * SUB);
    v->y = (int32_t) lround((SH / 2.0 - f * vy / vz) * SUB);
    v->iw = 1.0 / vz;
    v->depth = 65535.0 * (1.0 - 0.5 / vz);
    return 1;
}

/* --- The rasteriser --- */

static int64_t edge(const Vert *a, const Vert *b, int64_t px, int64_t py) {
    return (int64_t) (b->x - a->x) * (py - a->y) - (int64_t) (b->y - a->y) * (px - a->x);
}

/* A shared edge's pixels go to one of its two triangles: the one that has it running one way. */
static int64_t bias(const Vert *a, const Vert *b) {
    int dy = b->y - a->y, dx = b->x - a->x;
    return (dy > 0 || (dy == 0 && dx > 0)) ? 0 : -1;
}

static uint16_t lit(uint16_t t, double l) {
    if (l >= 1.0) return t;
    if (l < 0.0) l = 0.0;
    int r = (int) ((t >> 11) * l), g = (int) (((t >> 5) & 63) * l), b = (int) ((t & 31) * l);
    return (uint16_t) ((r << 11) | (g << 5) | b);
}

typedef struct { int x0, y0, x1, y1; } Rect; /* half-open */

static void triangle(const Face *fc, const Vert *v, const int *ci, Rect clip, uint16_t *col, uint16_t *dep) {
    Vert a = v[ci[0]], b = v[ci[1]], c = v[ci[2]];
    const double *pa = fc->c[ci[0]], *pb = fc->c[ci[1]], *pc = fc->c[ci[2]];
    int64_t area = edge(&a, &b, c.x, c.y);
    if (area == 0) return;
    if (area < 0) { Vert t = b; b = c; c = t; const double *tp = pb; pb = pc; pc = tp; area = -area; }
    int x0 = max(clip.x0, (min(a.x, min(b.x, c.x))) / SUB - 1), x1 = min(clip.x1, max(a.x, max(b.x, c.x)) / SUB + 2);
    int y0 = max(clip.y0, (min(a.y, min(b.y, c.y))) / SUB - 1), y1 = min(clip.y1, max(a.y, max(b.y, c.y)) / SUB + 2);
    if (x0 >= x1 || y0 >= y1) return;
    cnt.tris++;
    int64_t b0 = bias(&b, &c), b1 = bias(&c, &a), b2 = bias(&a, &b);
    const Pic *pic = &pics[fc->sprite];
    double inv = 1.0 / (double) area;
    /* What is interpolated: 1/w, u/w, v/w, light/w, depth. */
    double A[5][3] = {
        { a.iw, b.iw, c.iw },
        { pa[3] * a.iw, pb[3] * b.iw, pc[3] * c.iw },
        { pa[4] * a.iw, pb[4] * b.iw, pc[4] * c.iw },
        { pa[5] * a.iw, pb[5] * b.iw, pc[5] * c.iw },
        { a.depth, b.depth, c.depth },
    };
    for (int x = x0; x < x1; x++) {
        int64_t px = (int64_t) x * SUB + SUB / 2;
        for (int y = y0; y < y1; y++) {
            int64_t py = (int64_t) y * SUB + SUB / 2;
            int64_t w0 = edge(&b, &c, px, py), w1 = edge(&c, &a, px, py), w2 = edge(&a, &b, px, py);
            cnt.tested++;
            if (w0 + b0 < 0 || w1 + b1 < 0 || w2 + b2 < 0) continue;
            double l0 = (double) w0 * inv, l1 = (double) w1 * inv, l2 = (double) w2 * inv;
            double d = A[4][0] * l0 + A[4][1] * l1 + A[4][2] * l2;
            int dk = (int) d;
            if (dk < 0) dk = 0;
            if (dk > 65534) dk = 65534;
            int at = AT(x, y);
            if (dk >= dep[at]) continue;
            double iw = A[0][0] * l0 + A[0][1] * l1 + A[0][2] * l2;
            double u = (A[1][0] * l0 + A[1][1] * l1 + A[1][2] * l2) / iw;
            double vv = (A[2][0] * l0 + A[2][1] * l1 + A[2][2] * l2) / iw;
            double l = (A[3][0] * l0 + A[3][1] * l1 + A[3][2] * l2) / iw;
            int tu = (int) floor(u * TS), tv = (int) floor(vv * TS);
            tu = tu < 0 ? 0 : tu > TS - 1 ? TS - 1 : tu;
            tv = tv < 0 ? 0 : tv > TS - 1 ? TS - 1 : tv;
            uint16_t t = pic->t[tv * TS + tu];
            if (t == NONE) continue;
            col[at] = lit(t, l);
            dep[at] = (uint16_t) dk;
            cnt.written++;
        }
    }
}

static Vert *verts; /* every face's corners, projected for the camera last drawn with */

static void prepare(const Cam *cam) {
    for (int i = 0; i < nfaces; i++)
        for (int c = 0; c < faces[i].n; c++)
            if (!project(cam, faces[i].c[c], &verts[i * 8 + c])) verts[i * 8 + c].iw = -1.0;
}

/* The landscape drawn into `clip` of (col, dep), from scratch there. */
static void draw(const Cam *cam, Rect clip, uint16_t *col, uint16_t *dep) {
    if (clip.x0 >= clip.x1 || clip.y0 >= clip.y1) return;
    for (int x = clip.x0; x < clip.x1; x++)
        for (int y = clip.y0; y < clip.y1; y++) { col[AT(x, y)] = sky; dep[AT(x, y)] = 65535; }
    prepare(cam);
    for (int i = 0; i < nfaces; i++) {
        const Face *fc = &faces[i];
        if (fc->sprite < 0) continue;
        const Vert *v = &verts[i * 8];
        int behind = 0;
        for (int c = 0; c < fc->n; c++) if (v[c].iw < 0) behind = 1;
        if (behind) continue;
        for (int t = 1; t + 1 < fc->n; t++) {
            int ci[3] = { 0, t, t + 1 };
            triangle(fc, v, ci, clip, col, dep);
        }
    }
}

static const Rect WHOLE = { 0, 0, SW, SH };

/* B moved by (dx, dy) pixels on the screen (what was at x is at x + dx), the strips that came into
 * view drawn for the camera now. */
static void shift(uint16_t *buf, int dx, int dy, uint16_t fill) {
    memcpy(tmp, buf, sizeof tmp);
    for (int x = 0; x < SW; x++)
        for (int y = 0; y < SH; y++) {
            int sx = x - dx, sy = y - dy;
            buf[AT(x, y)] = (sx >= 0 && sx < SW && sy >= 0 && sy < SH) ? tmp[AT(sx, sy)] : fill;
        }
}

static void moved(const Cam *cam, int dx, int dy) {
    shift(b_col, dx, dy, sky);
    shift(b_dep, dx, dy, 65535);
    Rect cols = dx > 0 ? (Rect) { 0, 0, min(dx, SW), SH } : (Rect) { max(SW + dx, 0), 0, SW, SH };
    Rect rows = dy > 0 ? (Rect) { 0, 0, SW, min(dy, SH) } : (Rect) { 0, max(SH + dy, 0), SW, SH };
    if (dx != 0) draw(cam, cols, b_col, b_dep);
    if (dy != 0) {
        /* The corner both strips hold is drawn once. */
        if (dx > 0) rows.x0 = min(dx, SW); else if (dx < 0) rows.x1 = max(SW + dx, 0);
        draw(cam, rows, b_col, b_dep);
    }
}

typedef struct { int64_t col, dep; } Diff;

static Diff compare(void) {
    Diff d = { 0, 0 };
    for (int i = 0; i < SW * SH; i++) {
        if (b_col[i] != r_col[i]) d.col++;
        if (b_dep[i] != r_dep[i]) d.dep++;
    }
    return d;
}

static void save(const char *dir, const char *name, const uint16_t *col) {
    if (!dir) return;
    char path[512];
    snprintf(path, sizeof path, "%s/%s.ppm", dir, name);
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", SW, SH);
    for (int y = 0; y < SH; y++)
        for (int x = 0; x < SW; x++) {
            uint16_t p = col[AT(x, y)];
            unsigned char rgb[3] = { (unsigned char) ((p >> 11) * 255 / 31), (unsigned char) (((p >> 5) & 63) * 255 / 63),
                                     (unsigned char) ((p & 31) * 255 / 31) };
            fwrite(rgb, 1, 3, f);
        }
    fclose(f);
}

/* --- The movements --- */

typedef struct { const char *name; int cx, cy, cl; } Move; /* cells east, south, layers up */

static Cam base(int persp) {
    Cam c = { persp, 0, 0, 0, 0, 0 };
    /* The spot's cell in the middle of the view. */
    c.ox = (int) lround((spot[0] + 0.5) * PX) - SW / 2;
    c.oy = (int) lround((spot[1] + 0.5) * PZ - spot[2] * PY) - SH / 2;
    c.tx = spot[0] + 0.5;
    c.ty = spot[2];
    c.tz = spot[1] + 0.5;
    return c;
}

static void step(Cam *c, int cx, int cy, int cl, int *dx, int *dy) {
    if (!c->persp) {
        int mx = cx * PX, my = cy * PZ - cl * PY;
        c->ox += mx;
        c->oy += my;
        *dx = -mx;
        *dy = -my;
        return;
    }
    /* The shift the view's middle moves by: right there, and only there, a shift is exact. */
    double p[3] = { c->tx, c->ty, c->tz };
    Vert a, b;
    project(c, p, &a);
    c->tx += cx;
    c->tz += cy;
    c->ty += cl;
    project(c, p, &b);
    *dx = (int) lround((double) (b.x - a.x) / SUB);
    *dy = (int) lround((double) (b.y - a.y) / SUB);
}

static double whole_us; /* one picture drawn whole, for the cost column */
static int64_t whole_px;

/* Each of `moves` from the base camera: B drawn there, moved, and compared with a picture drawn
 * whole where the camera went.  `chain`: the moves one after another, B never drawn whole again. */
static void run(int persp, const Move *moves, int n, int chain, const char *dir, const char *title) {
    Cam cam = base(persp);
    draw(&cam, WHOLE, b_col, b_dep);
    printf("\n%s, %s\n", persp ? "perspective" : "parallel", title);
    printf("  %-26s %8s %10s %10s %10s %8s\n", "move", "shift", "drawn px", "differ", "depth off", "cost");
    int64_t total_drawn = 0, worst = 0;
    for (int i = 0; i < n; i++) {
        if (!chain) { cam = base(persp); draw(&cam, WHOLE, b_col, b_dep); }
        int dx, dy;
        step(&cam, moves[i].cx, moves[i].cy, moves[i].cl, &dx, &dy);
        cnt = (Count) { 0, 0, 0 };
        double t0 = now_us();
        moved(&cam, dx, dy);
        double t1 = now_us();
        int64_t drawn = (int64_t) (abs(dx) * SH) + (int64_t) (abs(dy) * (SW - abs(dx)));
        draw(&cam, WHOLE, r_col, r_dep);
        Diff d = compare();
        total_drawn += drawn;
        if (d.col > worst) worst = d.col;
        char sh[32];
        snprintf(sh, sizeof sh, "%d,%d", dx, dy);
        printf("  %-26s %8s %10lld %9.2f%% %9.2f%% %7.0f%%\n", moves[i].name, sh, (long long) drawn,
               100.0 * (double) d.col / (SW * SH), 100.0 * (double) d.dep / (SW * SH), 100.0 * (t1 - t0) / whole_us);
        if (dir && i == 0 && !chain) {
            char name[64];
            snprintf(name, sizeof name, "%s-moved", persp ? "persp" : "parallel");
            save(dir, name, b_col);
            snprintf(name, sizeof name, "%s-whole", persp ? "persp" : "parallel");
            save(dir, name, r_col);
            for (int k = 0; k < SW * SH; k++) tmp[k] = b_col[k] == r_col[k] ? (uint16_t) ((r_col[k] >> 2) & 0x39e7) : 0xf800;
            snprintf(name, sizeof name, "%s-differ", persp ? "persp" : "parallel");
            save(dir, name, tmp);
        }
    }
    if (chain)
        printf("  over the walk: %lld pixels drawn against %lld drawing each picture whole; worst %.2f%% differ\n",
               (long long) total_drawn, (long long) (whole_px * n), 100.0 * (double) worst / (SW * SH));
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: fixedcam SCENE [OUT_DIR]\n"); return 1; }
    load(argv[1]);
    const char *dir = argc > 2 ? argv[2] : NULL;
    verts = calloc((size_t) nfaces * 8, sizeof(Vert));
    sky = (uint16_t) (((int) (0.58 * 31) << 11) | ((int) (0.75 * 63) << 5) | (int) (0.9 * 31));
    printf("scene %s: %d faces, %d pictures, round (%d, %d, %d); the screen %d x %d\n", argv[1], nfaces, npics,
           spot[0], spot[1], spot[2], SW, SH);

    for (int persp = 0; persp < 2; persp++) {
        Cam cam = base(persp);
        cnt = (Count) { 0, 0, 0 };
        int reps = 3;
        double t0 = now_us();
        for (int r = 0; r < reps; r++) draw(&cam, WHOLE, b_col, b_dep);
        whole_us = (now_us() - t0) / reps;
        whole_px = SW * SH;
        printf("%s, drawn whole: %.0f us; per picture %lld triangles touched, %lld pixels tested, %lld written\n",
               persp ? "perspective" : "parallel", whole_us, (long long) (cnt.tris / reps),
               (long long) (cnt.tested / reps), (long long) (cnt.written / reps));
        save(dir, persp ? "persp" : "parallel", b_col);

        /* The camera's own movements, each from the same place. */
        static const Move single[] = {
            { "a cell east", 1, 0, 0 }, { "a cell west", -1, 0, 0 }, { "a cell north", 0, -1, 0 },
            { "a cell south", 0, 1, 0 }, { "a cell north-east", 1, -1, 0 }, { "two cells west", -2, 0, 0 },
            { "a layer up", 0, 0, 1 }, { "a layer down", 0, 0, -1 },
        };
        run(persp, single, (int) (sizeof single / sizeof single[0]), 0, dir, "one movement from the same place");
        if (!persp) {
            /* The control: a shift a pixel short must differ, or the zeros above prove nothing. */
            Cam c = base(0);
            draw(&c, WHOLE, b_col, b_dep);
            int dx, dy;
            step(&c, 1, 0, 0, &dx, &dy);
            moved(&c, dx + 1, dy);
            draw(&c, WHOLE, r_col, r_dep);
            Diff d = compare();
            printf("  %-26s %8s %10s %9.2f%% %9.2f%%   (the control: must not be 0)\n", "a cell east, a pixel short",
                   "", "", 100.0 * (double) d.col / (SW * SH), 100.0 * (double) d.dep / (SW * SH));
        }

        /* A walk: the follow camera moving a cell at a time, B shifted each time, never redrawn. */
        static const Move walk[] = {
            { "east", 1, 0, 0 }, { "east", 1, 0, 0 }, { "east", 1, 0, 0 }, { "north", 0, -1, 0 },
            { "north", 0, -1, 0 }, { "west", -1, 0, 0 }, { "west", -1, 0, 0 }, { "south-west", -1, 1, 0 },
            { "south", 0, 1, 0 }, { "east", 1, 0, 0 },
        };
        run(persp, walk, (int) (sizeof walk / sizeof walk[0]), 1, NULL, "a walk of ten follow steps, one after another");
    }

    /* Smooth scrolling, parallel only: the camera slides a cell in whole pixels a frame. */
    {
        Cam cam = base(0);
        draw(&cam, WHOLE, b_col, b_dep);
        int worst = 0;
        int64_t drawn = 0;
        int frames = 0;
        static const int slide[][2] = { { 6, 0 }, { 0, 5 }, { -6, -5 }, { 4, 0 } }; /* px a frame */
        for (int s = 0; s < 4; s++)
            for (int f = 0; f < 8; f++) {
                cam.ox += slide[s][0];
                cam.oy += slide[s][1];
                moved(&cam, -slide[s][0], -slide[s][1]);
                drawn += abs(slide[s][0]) * SH + abs(slide[s][1]) * SW;
                draw(&cam, WHOLE, r_col, r_dep);
                Diff d = compare();
                if (d.col + d.dep > worst) worst = (int) (d.col + d.dep);
                frames++;
            }
        printf("\nparallel, a smooth slide: %d frames of a few pixels each, a cell east, south, back north-west and"
               " east again: %d pixels differ at worst; %lld pixels drawn, %.1f%% of drawing every frame whole\n",
               frames, worst, (long long) drawn, 100.0 * (double) drawn / ((double) frames * SW * SH));
    }
    return 0;
}
