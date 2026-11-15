/* The fixed camera's frames, landscape and actors together (BUILD.md Phase E, E0 to E3): a
 * scripted walk through a place, drawn by raster3d as the game would draw it, every frame checked
 * against the whole scene drawn from scratch, and timed.
 *
 * The walk: the hero steps from cell to cell (a step every STEP frames, eased between), five
 * figures move about (two pacing, one wandering, two standing still), the walls in front of the
 * hero are cut away (`r3_uncover`: each cell's walls are a group), and the camera follows
 * in one of two ways:
 *   follow  it moves a whole cell when the hero comes within MARGIN cells of the edge (the 2D's);
 *   slide   it eases towards keeping the hero in the middle, a few whole pixels a frame.
 * Two framebuffers are made in turn, as the panel shows them.
 *
 *   bench/frame3d SCENE         (a scene written by tools/scene3d.loft; bench/run.sh frame3d)
 */
#include "../local/raster3d/c/raster3d.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STEP 8    /* frames a hero step takes */
#define MARGIN 3  /* cells from the edge before a follow camera moves */
#define FRAMES 600

static double now_us(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double) t.tv_sec * 1e6 + (double) t.tv_nsec / 1e3;
}

/* --- The scene --- */

typedef struct { int id, card, n; double c[8][6]; } Face;
static Face *faces;
static int nfaces;
typedef struct { int id; uint16_t t[R3_TS * R3_TS]; } Pic;
static Pic *pics;
static int npics;
static int spot[3];
static FILE *in;

static int rd_int(void) { int v; if (fscanf(in, "%d", &v) != 1) { fprintf(stderr, "a scene cut short\n"); exit(1); } return v; }
static double rd_dbl(void) { double v; if (fscanf(in, "%lf", &v) != 1) { fprintf(stderr, "a scene cut short\n"); exit(1); } return v; }

static void load(const char *path) {
    in = fopen(path, "r");
    if (!in) { fprintf(stderr, "cannot read %s\n", path); exit(1); }
    int version;
    if (fscanf(in, "scene3d %d spot %d %d %d", &version, &spot[0], &spot[1], &spot[2]) != 4) { fprintf(stderr, "not a scene\n"); exit(1); }
    int cf = 1024, cp = 64;
    faces = malloc((size_t) cf * sizeof(Face));
    pics = malloc((size_t) cp * sizeof(Pic));
    char tag[8];
    for (;;) {
        if (fscanf(in, "%7s", tag) != 1) { fprintf(stderr, "a scene cut short\n"); exit(1); }
        if (!strcmp(tag, "end")) break;
        if (!strcmp(tag, "f")) {
            if (nfaces == cf) { cf *= 2; faces = realloc(faces, (size_t) cf * sizeof(Face)); }
            Face *f = &faces[nfaces++];
            f->id = rd_int(); f->card = rd_int(); f->n = rd_int();
            if (f->n > 8) { fprintf(stderr, "a face of %d corners\n", f->n); exit(1); }
            for (int c = 0; c < f->n; c++) for (int k = 0; k < 6; k++) f->c[c][k] = rd_dbl();
        } else if (!strcmp(tag, "p")) {
            if (npics == cp) { cp *= 2; pics = realloc(pics, (size_t) cp * sizeof(Pic)); }
            pics[npics].id = rd_int();
            for (int t = 0; t < R3_TS * R3_TS; t++) pics[npics].t[t] = (uint16_t) rd_int();
            npics++;
        } else { fprintf(stderr, "a record '%s'\n", tag); exit(1); }
    }
    fclose(in);
}

static int pic_index(int id) {
    for (int i = 0; i < npics; i++) if (pics[i].id == id) return i;
    return -1;
}

/* The cells' wall faces are grouped, so the walls in front of the hero can be cut away: a face
 * that is not a card and stands up from its layer's floor belongs to its cell's group. */
#define GW 128
static int group_of(int cx, int cz) { return (cz + GW / 2) * GW + (cx + GW / 2); }

static int wall_group(const Face *f) {
    if (f->card) return -1;
    double x = 1e9, z = 1e9, y0 = 1e9, y1 = -1e9;
    for (int c = 0; c < f->n; c++) {
        if (f->c[c][0] < x) x = f->c[c][0];
        if (f->c[c][2] < z) z = f->c[c][2];
        if (f->c[c][1] < y0) y0 = f->c[c][1];
        if (f->c[c][1] > y1) y1 = f->c[c][1];
    }
    if (y1 - floor(y0 + 1e-6) < 0.5) return -1; /* a floor, not a wall */
    int cx = (int) floor(x + 1e-3) - spot[0], cz = (int) floor(z + 1e-3) - spot[1];
    if (cx < -GW / 2 || cx >= GW / 2 || cz < -GW / 2 || cz >= GW / 2) return -1;
    return group_of(cx, cz);
}

static R3 *scene(void) {
    R3 *r = r3_new((uint16_t) ((17 << 11) | (47 << 5) | 27));
    for (int i = 0; i < npics; i++) r3_picture(r, pics[i].t);
    for (int i = 0; i < nfaces; i++) r3_face(r, pic_index(faces[i].id), faces[i].n, &faces[i].c[0][0], wall_group(&faces[i]));
    r3_build(r, spot[0], spot[2], spot[1]);
    return r;
}

/* --- The walk --- */

static const int path[][2] = { /* the hero's steps, in cells */
    { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 }, { 0, -1 }, { 0, -1 }, { 0, -1 }, { 0, -1 },
    { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 }, { 0, 1 }, { 0, 1 },
    { 0, 1 }, { 0, 1 }, { 0, 1 }, { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 }, { 0, -1 },
};
#define NPATH ((int) (sizeof path / sizeof path[0]))

typedef struct { double x, z; } P2;

static P2 hero_at(int f) {
    P2 p = { spot[0] + 0.5, spot[1] + 0.5 };
    int s = f / STEP;
    for (int i = 0; i < s && i < NPATH; i++) { p.x += path[i][0]; p.z += path[i][1]; }
    if (s < NPATH) {
        double t = (double) (f % STEP) / STEP;
        p.x += path[s][0] * t;
        p.z += path[s][1] * t;
    }
    return p;
}

static int hero_pic, fig_pic[5];

static void actors(R3 *r, int f, int moved_control) {
    P2 h = hero_at(f);
    double y = spot[2];
    r3_actors_begin(r);
    r3_actor(r, 0, hero_pic, h.x + (moved_control ? 1.0 / 48 : 0), y, h.z, 0.85, 1.0);
    /* Two pacing three cells and back, one wandering, two standing. */
    double pace = fabs(fmod(f / 40.0, 6.0) - 3.0);
    r3_actor(r, 1, fig_pic[0], spot[0] - 3.5 + pace, y, spot[1] + 2.5, 0.85, 1.0);
    r3_actor(r, 2, fig_pic[1], spot[0] + 4.5, y, spot[1] - 1.5 + pace * 0.5, 0.85, 0.9);
    r3_actor(r, 3, fig_pic[2], spot[0] + 2.5 + sin(f / 25.0) * 2, y, spot[1] + 1.5 + cos(f / 31.0), 0.85, 1.0);
    r3_actor(r, 4, fig_pic[3], spot[0] - 2.5, y, spot[1] - 2.5, 0.85, 1.0);
    r3_actor(r, 5, fig_pic[4], spot[0] + 6.5, y, spot[1] + 3.5, 0.5, 0.8);
}

typedef struct { double mean_us, max_us; int differ_frames; int64_t differ_px, fb_px, b_px, actor_px, rects; int cut_frames; } Result;

static uint16_t fb[2][R3_SW * R3_SH], ref[R3_SW * R3_SH];

static Result run(int slide, int check, int control) {
    R3 *r = scene();
    int camx = 0, camy = 0;
    {
        P2 h = hero_at(0);
        camx = (int) lround(h.x * R3_PX) - R3_SW / 2;
        camy = (int) lround(h.z * R3_PZ - spot[2] * R3_PY) - R3_SH / 2;
    }
    Result res = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    r3_count(r);
    for (int f = 0; f < FRAMES; f++) {
        P2 h = hero_at(f);
        int hx = (int) lround(h.x * R3_PX), hy = (int) lround(h.z * R3_PZ - spot[2] * R3_PY);
        if (slide) {
            int tx = hx - R3_SW / 2, ty = hy - R3_SH / 2;
            int dx = tx - camx, dy = ty - camy;
            camx += dx > 6 ? 6 : dx < -6 ? -6 : dx;
            camy += dy > 5 ? 5 : dy < -5 ? -5 : dy;
        } else {
            while (hx < camx + MARGIN * R3_PX) camx -= R3_PX;
            while (hx > camx + R3_SW - MARGIN * R3_PX) camx += R3_PX;
            while (hy < camy + MARGIN * R3_PZ) camy -= R3_PZ;
            while (hy > camy + R3_SH - MARGIN * R3_PZ) camy += R3_PZ;
        }
        double t0 = now_us();
        r3_camera(r, camx, camy);
        actors(r, f, 0);
        r3_uncover(r, 0); /* the hero is never hidden */
        r3_frame(r, fb[f & 1], f & 1);
        double dt = now_us() - t0;
        res.mean_us += dt;
        if (dt > res.max_us) res.max_us = dt;
        R3Count c = r3_count(r);
        res.fb_px += c.fb_px; res.b_px += c.b_px; res.actor_px += c.actor_px; res.rects += c.rects;
        if (c.uncovered) res.cut_frames++;
        if (check) {
            if (control) actors(r, f, 1); /* the hero a pixel off: the check must see it */
            r3_reference(r, ref);
            int64_t d = 0;
            for (int i = 0; i < R3_SW * R3_SH; i++) d += fb[f & 1][i] != ref[i];
            if (d) { res.differ_frames++; res.differ_px += d; }
            if (control) actors(r, f, 0);
        }
    }
    res.mean_us /= FRAMES;
    r3_free(r);
    return res;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: frame3d SCENE\n"); return 1; }
    load(argv[1]);
    /* The figures' pictures: the scene's cards that stand alone, the first few. */
    int found = 0;
    for (int i = 0; i < nfaces && found < 6; i++) {
        if (!faces[i].card) continue;
        int p = pic_index(faces[i].id), dup = 0;
        for (int k = 0; k < found; k++) dup |= (k == 0 ? hero_pic : fig_pic[k - 1]) == p;
        if (dup || p < 0) continue;
        if (found == 0) hero_pic = p; else fig_pic[found - 1] = p;
        found++;
    }
    for (int k = found; k < 6; k++) { if (k == 0) hero_pic = 0; else fig_pic[k - 1] = hero_pic; }
    printf("scene %s: %d faces, %d pictures, round (%d, %d, %d); %d frames of %d x %d, 6 actors\n", argv[1], nfaces,
           npics, spot[0], spot[1], spot[2], FRAMES, R3_SW, R3_SH);
    printf("  %-8s %-10s %9s %9s %12s %12s %12s %8s\n", "camera", "", "mean us", "max us", "B px/frame",
           "copy px/fr", "actor px/fr", "rects");
    for (int slide = 0; slide < 2; slide++) {
        Result chk = run(slide, 1, 0);
        printf("  %-8s %-10s %9s %9s %12s %12s %12s %8s   %d of %d frames differ from scratch (%lld px); walls cut"
               " away in %d\n", slide ? "slide" : "follow", "checked", "", "", "", "", "", "", chk.differ_frames, FRAMES,
               (long long) chk.differ_px, chk.cut_frames);
        Result t = run(slide, 0, 0);
        printf("  %-8s %-10s %9.0f %9.0f %12lld %12lld %12lld %8.1f\n", slide ? "slide" : "follow", "timed", t.mean_us,
               t.max_us, (long long) (t.b_px / FRAMES), (long long) (t.fb_px / FRAMES), (long long) (t.actor_px / FRAMES),
               (double) t.rects / FRAMES);
    }
    Result ctl = run(0, 1, 1);
    printf("  control: the hero drawn a pixel off after the frame: %d of %d frames differ (must not be 0)\n",
           ctl.differ_frames, FRAMES);
    /* Drawing the whole scene, for scale. */
    R3 *r = scene();
    P2 h0 = hero_at(0);
    r3_camera(r, (int) lround(h0.x * R3_PX) - R3_SW / 2, (int) lround(h0.z * R3_PZ - spot[2] * R3_PY) - R3_SH / 2);
    actors(r, 0, 0);
    double t0 = now_us();
    for (int i = 0; i < 5; i++) r3_reference(r, ref);
    printf("  for scale: the whole scene drawn from scratch: %.0f us\n", (now_us() - t0) / 5);
    r3_free(r);
    return 0;
}
