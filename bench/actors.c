/* What the actors cost in a frame of the 3D view on the Tanmatsu (BUILD.md Phase E, E0 and E3).
 *
 * The landscape is a kept picture B, its colour and its depth, drawn only when it changes.  Each
 * frame the actors (the hero, the people, the monsters, the things in motion) are drawn over it
 * as sprites: a figure's picture scaled to its size on the screen, its pixels drawn only where
 * it is nearer than B's depth.  A framebuffer keeps what it showed, so a frame puts back from B
 * only the rectangles where something changed and draws the actors there again; two
 * framebuffers are flipped at the refresh, nothing copied.
 *
 * Measured here, for quick animations (walking, a thrown bomb: a change every frame) and slow
 * ones (an idle figure breathing: a change every few frames), in three ways of making a frame:
 *   reuse       the changed rectangles only, two framebuffers flipped (E3);
 *   reuse+copy  the same in one buffer, then the whole of it copied to the panel's buffer, as
 *               `screen` does today (one framebuffer, a copy every frame);
 *   scratch     the whole of B copied and every actor drawn: what each frame costs when B itself
 *               moved (the fixed camera sliding, the picture zoomed or turned while walking).
 * Every frame of `reuse` is checked against the same frame drawn from scratch.
 *
 * Buffers are laid out as the panel is: portrait, 480 wide, 800 rows, so a screen column is one
 * panel row of 480 pixels in a row in memory.  On the device B, its depth and the framebuffers
 * are in PSRAM (768,000 bytes each), the figures' pictures small enough for SRAM.  The counts
 * of bytes are printed per frame, so one run on the device turns them into a time.
 *
 * Under an emulator the times say which way of making a frame does less work, not what the
 * device takes: memory is the PC's.  Build and run on both: bench/run.sh. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SW 800 /* the screen, as the player sees it */
#define SH 480
#define AT(x, y) ((x) * SH + (y)) /* screen column x is panel row x */
#define TW 32 /* a figure's picture: 32 x 48 texels, column by column, 0 transparent */
#define TH 48
#define NPIC 8
#define MAXA 16
#define MAXR (4 * MAXA)

static uint16_t b_col[SW * SH], b_dep[SW * SH], fb[2][SW * SH], panel[SW * SH], ref[SW * SH];
static uint16_t pics[NPIC][TW * TH];

typedef struct { int x0, y0, x1, y1; } Rect; /* half-open, clipped to the screen */

typedef struct {
    int x, y, h;    /* where it starts and its height on the screen; its width is h * TW / TH */
    uint16_t depth; /* smaller is nearer */
    int vx;         /* pixels moved each frame, across the screen and round again */
    int period;     /* frames between changes of picture; 0: it never changes */
    int phase;
    /* this frame */
    Rect r;
    int pic, cx;
} Actor;

typedef struct { Rect r; int pic; int valid; } Shown; /* what a framebuffer last showed of an actor */

typedef struct {
    int64_t restored, visited, written; /* pixels put back from B, sprite pixels tested, written */
    int64_t psram_read, psram_write;    /* bytes */
} Count;

static Count cnt;
static volatile uint32_t sink; /* read from the panel's buffer, so its copy is not optimised away */

static uint32_t seed = 12345u;
static uint32_t next(void) { seed = seed * 1103515245u + 12345u; return seed >> 8; }

static double now_us(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double) t.tv_sec * 1e6 + (double) t.tv_nsec / 1e3;
}

static int empty(Rect r) { return r.x0 >= r.x1 || r.y0 >= r.y1; }
static int same(Rect a, Rect b) { return a.x0 == b.x0 && a.y0 == b.y0 && a.x1 == b.x1 && a.y1 == b.y1; }
static int min(int a, int b) { return a < b ? a : b; }
static int max(int a, int b) { return a > b ? a : b; }
static Rect cut(Rect a, Rect b) { Rect r = { max(a.x0, b.x0), max(a.y0, b.y0), min(a.x1, b.x1), min(a.y1, b.y1) }; return r; }
static Rect hull(Rect a, Rect b) { Rect r = { min(a.x0, b.x0), min(a.y0, b.y0), max(a.x1, b.x1), max(a.y1, b.y1) }; return r; }
static int meet(Rect a, Rect b) { return a.x0 < b.x1 && b.x0 < a.x1 && a.y0 < b.y1 && b.y0 < a.y1; }
static int64_t area(Rect r) { return empty(r) ? 0 : (int64_t) (r.x1 - r.x0) * (r.y1 - r.y0); }

/* A landscape: colour bands, depth growing toward the horizon, and pillars standing nearer, so
 * the actors pass behind some and in front of others. */
static void make_landscape(void) {
    for (int x = 0; x < SW; x++) {
        int pillar = (x / 40) % 5 == 2;
        for (int y = 0; y < SH; y++) {
            b_col[AT(x, y)] = (uint16_t) (((x >> 3) << 11) ^ ((y >> 2) << 5) ^ (x * y));
            uint16_t d = (uint16_t) (y < 160 ? 60000 : 60000 - (y - 160) * 150);
            if (pillar && y > 60) d = 20000;
            b_dep[AT(x, y)] = d;
        }
    }
}

/* Figures: an oval body whose shape and colours shift a little from picture to picture. */
static void make_pictures(void) {
    for (int p = 0; p < NPIC; p++)
        for (int tx = 0; tx < TW; tx++)
            for (int ty = 0; ty < TH; ty++) {
                int dx = 2 * tx - TW + 1, dy = 2 * ty - TH + 1 + (p & 3);
                int in = dx * dx * TH * TH + dy * dy * TW * TW < TW * TW * TH * TH;
                pics[p][tx * TH + ty] = in ? (uint16_t) (1 + ((next() & 0x7FFF) | (p << 12))) : 0;
            }
}

static int width_of(const Actor *a) { return a->h * TW / TH; }

static void place(Actor *a, int f) {
    int w = width_of(a);
    int span = SW + w;
    int x = a->x + a->vx * f;
    x = ((x + w) % span + span) % span - w;
    a->cx = x;
    a->pic = a->period ? (a->phase + f / a->period) % NPIC : a->phase % NPIC;
    Rect r = { x, a->y, x + w, a->y + a->h }, s = { 0, 0, SW, SH };
    a->r = cut(r, s);
}

/* The sprite, scaled to its size (nearest texel), depth-tested against B, clipped to c. */
static void blit(uint16_t *dst, const Actor *a, Rect c) {
    Rect r = cut(a->r, c);
    if (empty(r)) return;
    int w = width_of(a);
    uint32_t sx = ((uint32_t) TW << 16) / (uint32_t) w, sy = ((uint32_t) TH << 16) / (uint32_t) a->h;
    const uint16_t *src = pics[a->pic];
    uint16_t depth = a->depth;
    int64_t written = 0;
    for (int x = r.x0; x < r.x1; x++) {
        const uint16_t *col = src + ((uint32_t) (x - a->cx) * sx >> 16) * TH;
        uint32_t fy = (uint32_t) (r.y0 - a->y) * sy;
        uint16_t *d = dst + AT(x, r.y0);
        const uint16_t *z = b_dep + AT(x, r.y0);
        for (int y = r.y0; y < r.y1; y++) {
            uint16_t t = col[fy >> 16];
            if (t && depth < *z) { *d = t; written++; }
            d++;
            z++;
            fy += sy;
        }
    }
    int64_t n = area(r);
    cnt.visited += n;
    cnt.written += written;
    cnt.psram_read += 2 * n;        /* B's depth; the picture is in SRAM */
    cnt.psram_write += 2 * written;
}

static void restore(uint16_t *dst, Rect r) {
    if (empty(r)) return;
    for (int x = r.x0; x < r.x1; x++)
        memcpy(dst + AT(x, r.y0), b_col + AT(x, r.y0), (size_t) (r.y1 - r.y0) * 2);
    int64_t n = area(r);
    cnt.restored += n;
    cnt.psram_read += 2 * n;
    cnt.psram_write += 2 * n;
}

/* The actors far to near, so a nearer one is drawn over a farther one. */
static int order[MAXA];
static Actor *sort_a;
static int by_depth(const void *p, const void *q) {
    return (int) sort_a[*(const int *) q].depth - (int) sort_a[*(const int *) p].depth;
}

static void scratch(uint16_t *dst, Actor *as, int n) {
    memcpy(dst, b_col, sizeof b_col);
    cnt.restored += SW * SH;
    cnt.psram_read += 2 * SW * SH;
    cnt.psram_write += 2 * SW * SH;
    Rect s = { 0, 0, SW, SH };
    for (int i = 0; i < n; i++) blit(dst, &as[order[i]], s);
}

/* The changed rectangles of this framebuffer: where a changed actor was and where it is now,
 * overlapping ones joined.  Put back from B, every actor that meets one drawn again, clipped. */
static int reuse(uint16_t *dst, Shown *shown, Actor *as, int n) {
    Rect rs[MAXR];
    int nr = 0;
    for (int i = 0; i < n; i++) {
        Actor *a = &as[i];
        Shown *s = &shown[i];
        if (s->valid && same(s->r, a->r) && s->pic == a->pic) continue;
        if (s->valid && !empty(s->r)) rs[nr++] = s->r;
        if (!empty(a->r)) rs[nr++] = a->r;
        s->r = a->r;
        s->pic = a->pic;
        s->valid = 1;
    }
    for (int joined = 1; joined;) {
        joined = 0;
        for (int i = 0; i < nr && !joined; i++)
            for (int j = i + 1; j < nr && !joined; j++)
                if (meet(rs[i], rs[j])) { rs[i] = hull(rs[i], rs[j]); rs[j] = rs[--nr]; joined = 1; }
    }
    for (int k = 0; k < nr; k++) {
        restore(dst, rs[k]);
        for (int i = 0; i < n; i++) blit(dst, &as[order[i]], rs[k]);
    }
    return nr;
}

typedef struct { const char *name; int n; Actor a[MAXA]; } Scene;

enum { REUSE, REUSE_COPY, SCRATCH };
static const char *mode_name[] = { "reuse", "reuse+copy", "scratch" };

#define FRAMES 600

static void run(Scene *sc, int mode, int verify) {
    Shown shown[2][MAXA];
    memset(shown, 0, sizeof shown);
    memset(fb, 0, sizeof fb);
    Actor *as = sc->a;
    int n = sc->n;
    for (int i = 0; i < n; i++) order[i] = i;
    sort_a = as;
    qsort(order, (size_t) n, sizeof order[0], by_depth);
    /* The first two frames fill both framebuffers; they are not counted. */
    for (int f = 0; f < 2; f++) {
        for (int i = 0; i < n; i++) place(&as[i], f);
        scratch(fb[f], as, n);
        for (int i = 0; i < n; i++) { shown[f][i].r = as[i].r; shown[f][i].pic = as[i].pic; shown[f][i].valid = 1; }
    }
    memset(&cnt, 0, sizeof cnt);
    double total = 0, worst = 0;
    int64_t rects = 0, bad = 0;
    for (int f = 2; f < FRAMES + 2; f++) {
        for (int i = 0; i < n; i++) place(&as[i], f);
        int k = mode == REUSE ? f & 1 : 0;
        double t0 = now_us();
        if (mode == SCRATCH) scratch(fb[k], as, n);
        else rects += reuse(fb[k], shown[k], as, n);
        if (mode == REUSE_COPY) {
            memcpy(panel, fb[0], sizeof panel);
            sink += panel[(f * 7919) % (SW * SH)];
            cnt.psram_read += 2 * SW * SH;
            cnt.psram_write += 2 * SW * SH;
        }
        double t = now_us() - t0;
        total += t;
        if (t > worst) worst = t;
        if (verify) {
            Count keep = cnt;
            scratch(ref, as, n);
            cnt = keep;
            if (memcmp(ref, fb[k], sizeof ref) != 0) bad++;
        }
    }
    printf("  %-11s %8.0f %8.0f   %6.1f %6.1f %6.1f  %7.0f %7.0f",
           mode_name[mode], total / FRAMES, worst,
           (double) cnt.restored / FRAMES / 1000, (double) cnt.visited / FRAMES / 1000,
           (double) cnt.written / FRAMES / 1000,
           (double) cnt.psram_read / FRAMES / 1024, (double) cnt.psram_write / FRAMES / 1024);
    if (mode != SCRATCH) printf("  %4.1f", (double) rects / FRAMES);
    if (verify) printf("  %s", bad ? "WRONG" : "ok");
    if (bad) printf(" (%lld frames differ)", (long long) bad);
    printf("\n");
}

static Actor walker(int x, int y, int h, uint16_t depth, int vx, int period, int phase) {
    Actor a = { x, y, h, depth, vx, period, phase, { 0, 0, 0, 0 }, 0, 0 };
    return a;
}

int main(void) {
    make_landscape();
    make_pictures();
    static Scene scenes[] = {
        { "still: 6 figures, none moving", 6, { { 0 } } },
        { "slow: 6 idle figures, each a new picture every 12 frames", 6, { { 0 } } },
        { "quick: the hero walking (200 px tall) among 5 idle figures", 6, { { 0 } } },
        { "busy: hero + 3 walkers + a thrown bomb, 6 idle", 11, { { 0 } } },
        { "near: 4 walkers close by (320 px tall)", 4, { { 0 } } },
    };
    for (int i = 0; i < 6; i++) {
        Actor still = walker(60 + i * 120, 180 + (i % 3) * 40, 90 + (i % 3) * 30, (uint16_t) (40000 - i * 2000), 0, 0, i);
        Actor idle = still;
        idle.period = 12;
        idle.phase = i * 2; /* staggered, so they do not all change in one frame */
        scenes[0].a[i] = still;
        scenes[1].a[i] = idle;
        if (i > 0) scenes[2].a[i] = idle;
        scenes[3].a[5 + i] = idle;
    }
    scenes[2].a[0] = walker(0, 250, 200, 10000, 4, 4, 0);
    scenes[3].a[0] = walker(0, 250, 200, 10000, 4, 4, 0);
    scenes[3].a[1] = walker(300, 200, 140, 15000, -3, 4, 1);
    scenes[3].a[2] = walker(500, 170, 100, 30000, 2, 4, 2);
    scenes[3].a[3] = walker(100, 150, 60, 45000, 3, 4, 3);
    scenes[3].a[4] = walker(0, 120, 24, 12000, 12, 2, 0); /* the bomb */
    for (int i = 0; i < 4; i++) scenes[4].a[i] = walker(i * 220, 140, 320, (uint16_t) (8000 + i * 500), i & 1 ? -4 : 4, 4, i);

    printf("Per frame, %d frames each.  us: this machine's time (mean, worst).  kpx: thousands of\n"
           "pixels put back from B, sprite pixels tested, sprite pixels written.  KB: PSRAM read and\n"
           "written on the device.  rects: changed rectangles.  check: each frame against scratch.\n\n",
           FRAMES);
    printf("  %-11s %8s %8s   %6s %6s %6s  %7s %7s  %5s %s\n", "", "us mean", "worst", "kpx bk", "tested", "wrote",
           "KB rd", "KB wr", "rects", "check");
    for (size_t s = 0; s < sizeof scenes / sizeof scenes[0]; s++) {
        printf("%s\n", scenes[s].name);
        for (int m = 0; m < 3; m++) run(&scenes[s], m, m == REUSE);
    }

    printf("\nOne quick figure (moving 4 px and a new picture every 4 frames) by its height, reuse:\n");
    for (int h = 30; h <= 480; h *= 2) {
        Scene one = { "", 1, { walker(0, SH - h > 0 ? (SH - h) / 2 : 0, h, 10000, 4, 4, 0) } };
        printf("  %3d px tall", h);
        run(&one, REUSE, 0);
    }
    return 0;
}
