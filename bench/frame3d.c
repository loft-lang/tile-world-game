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
 * The landscape is given in tiles of TILE x TILE cells, as the game gives it; in the "changes" runs
 * the landscape changes under the walk as well: the tile the hero stands in given again with
 * another light every so often, the one east of it dropped and given back, once the depth's
 * origin moved, and every 23 frames the light of the cells round the hero (`r3_light`: each face
 * is lit from its cell, and its triangles are lit again, no face given again); and an overlay over
 * it all, a status line and a bubble over the hero (`r3_overlay`).  A build with -DR3_SKIP_DIRTY (the control) does not draw a change again, and the
 * check must see it.
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

typedef struct { int id, card, n; float c[8][6]; } Face;
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
            for (int c = 0; c < f->n; c++) for (int k = 0; k < 6; k++) f->c[c][k] = (float) rd_dbl();
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
    float x = 1e9f, z = 1e9f, y0 = 1e9f, y1 = -1e9f;
    for (int c = 0; c < f->n; c++) {
        if (f->c[c][0] < x) x = f->c[c][0];
        if (f->c[c][2] < z) z = f->c[c][2];
        if (f->c[c][1] < y0) y0 = f->c[c][1];
        if (f->c[c][1] > y1) y1 = f->c[c][1];
    }
    if (y1 - floorf(y0 + 1e-6f) < 0.5f) return -1; /* a floor, not a wall */
    int cx = (int) floorf(x + 1e-3f) - spot[0], cz = (int) floorf(z + 1e-3f) - spot[1];
    if (cx < -GW / 2 || cx >= GW / 2 || cz < -GW / 2 || cz >= GW / 2) return -1;
    return group_of(cx, cz);
}

#define TILE 8
static int floor_div8(int v) { return v >= 0 ? v / TILE : -((TILE - 1 - v) / TILE); }
static int tile_key(int tx, int tz) { return (tz + 512) * 1024 + (tx + 512); }

/* The tile a face is given in: the one holding its corners' west-north-most cell. */
static int face_tile(const Face *f) {
    float x = 1e9f, z = 1e9f;
    for (int c = 0; c < f->n; c++) {
        if (f->c[c][0] < x) x = f->c[c][0];
        if (f->c[c][2] < z) z = f->c[c][2];
    }
    return tile_key(floor_div8((int) floorf(x + 1e-3f)), floor_div8((int) floorf(z + 1e-3f)));
}

/* The cell a face is lit from: its corners' west-north-most cell, on the layer of its lowest. */
static void face_cell(const Face *f, int *x, int *y, int *z) {
    float mx = 1e9f, mz = 1e9f, my = 1e9f;
    for (int c = 0; c < f->n; c++) {
        if (f->c[c][0] < mx) mx = f->c[c][0];
        if (f->c[c][2] < mz) mz = f->c[c][2];
        if (f->c[c][1] < my) my = f->c[c][1];
    }
    *x = (int) floorf(mx + 1e-3f);
    *y = (int) floorf(mz + 1e-3f);
    *z = (int) floorf(my + 1e-3f);
}

/* Tile `key` given (again), its faces' light times `light`, each lit from its cell (until the
 * cell's light is given, by its own corners' light). */
static void give_tile(R3 *r, int key, float light) {
    r3_tile(r, key);
    for (int i = 0; i < nfaces; i++) {
        if (face_tile(&faces[i]) != key) continue;
        float c[8][6];
        memcpy(c, faces[i].c, sizeof c);
        for (int k = 0; k < faces[i].n; k++) c[k][5] *= light;
        int lx, ly, lz;
        face_cell(&faces[i], &lx, &ly, &lz);
        r3_face_lit(r, pic_index(faces[i].id), faces[i].n, &c[0][0], wall_group(&faces[i]), lx, ly, lz, 0.9);
    }
}

static R3 *scene(void) {
    R3 *r = r3_new((uint16_t) ((17 << 11) | (47 << 5) | 27));
    for (int i = 0; i < npics; i++) r3_picture(r, pics[i].t);
    int *keys = malloc((size_t) nfaces * sizeof(int)), nk = 0;
    for (int i = 0; i < nfaces; i++) {
        int k = face_tile(&faces[i]), dup = 0;
        for (int j = 0; j < nk && !dup; j++) dup = keys[j] == k;
        if (!dup) keys[nk++] = k;
    }
    for (int j = 0; j < nk; j++) give_tile(r, keys[j], 1.0);
    free(keys);
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

typedef struct { float x, z; } P2;

static P2 hero_at(int f) {
    P2 p = { spot[0] + 0.5, spot[1] + 0.5 };
    int s = f / STEP;
    for (int i = 0; i < s && i < NPATH; i++) { p.x += path[i][0]; p.z += path[i][1]; }
    if (s < NPATH) {
        float t = (float) (f % STEP) / STEP;
        p.x += path[s][0] * t;
        p.z += path[s][1] * t;
    }
    return p;
}

static int hero_pic, fig_pic[5];

static void actors(R3 *r, int f, int moved_control) {
    P2 h = hero_at(f);
    float y = (float) spot[2];
    r3_actors_begin(r);
    /* The hero posed as motion.loft poses it: leaning the way it walks, mirrored facing west, a
     * wobble as it steps; what it holds swung about its grip. */
    int s = f / STEP < NPATH ? f / STEP : NPATH - 1;
    int west = path[s][0] < 0;
    float lean = path[s][0] * 6.0f + 4.0f * sinf(f * 0.7f);
    r3_actor_pose(r, 0, hero_pic, h.x + (moved_control ? 1.0f / 48 : 0), y, h.z, 0.85f, 1.0f, lean, 8.5f / 16, 15.5f / 16, west);
    r3_actor_pose(r, 6, fig_pic[4], h.x + 0.3f, y + 0.25f, h.z + 0.02f, 0.85f, 1.0f, fmodf(f * 9.0f, 360.0f), 0.5f, 0.9f, 0);
    /* Two pacing three cells and back, one wandering, two standing. */
    float pace = fabsf(fmodf(f / 40.0f, 6.0f) - 3.0f);
    r3_actor(r, 1, fig_pic[0], spot[0] - 3.5f + pace, y, spot[1] + 2.5f, 0.85f, 1.0f);
    r3_actor(r, 2, fig_pic[1], spot[0] + 4.5f, y, spot[1] - 1.5f + pace * 0.5f, 0.85f, 0.9f);
    r3_actor(r, 3, fig_pic[2], spot[0] + 2.5f + sinf(f / 25.0f) * 2, y, spot[1] + 1.5f + cosf(f / 31.0f), 0.85f, 1.0f);
    r3_actor(r, 4, fig_pic[3], spot[0] - 2.5f, y, spot[1] - 2.5f, 0.85f, 1.0f);
    r3_actor(r, 5, fig_pic[4], spot[0] + 6.5f, y, spot[1] + 3.5f, 0.5f, 0.8f);
}

typedef struct { double mean_us, max_us; int differ_frames; int64_t differ_px, fb_px, b_px, actor_px, rects; int cut_frames, changes; } Result;

/* The overlay, as the game gives it: a status line that grows every 30 frames along the bottom and
 * a bubble over the hero, a third of the frame's size drawn three times larger. */
#define OW 267
#define OH 160
static uint16_t ovl[OW * OH];

static void give_overlay(R3 *r, int f, int camx, int camy) {
    for (int i = 0; i < OW * OH; i++) ovl[i] = R3_NONE;
    int bar = 20 + (f / 30) % 10 * 20;
    for (int y = OH - 12; y < OH - 2; y++) for (int x = 2; x < bar && x < OW; x++) ovl[y * OW + x] = 0x2104;
    P2 h = hero_at(f);
    int bx = ((int) lround(h.x * R3_PX) - camx) / 3 - 20, by = ((int) lround(h.z * R3_PZ - spot[2] * R3_PY) - camy) / 3 - 30;
    for (int y = by; y < by + 12; y++)
        for (int x = bx; x < bx + 40; x++)
            if (x >= 0 && x < OW && y >= 0 && y < OH) ovl[y * OW + x] = (x == bx || y == by) ? 0xFFE0 : 0x1082;
    r3_overlay(r, ovl, OW, OH, 3);
}

/* The landscape changing under the walk, before frame f. */
static int change(R3 *r, int f) {
    P2 h = hero_at(f);
    int tx = floor_div8((int) floor(h.x)), tz = floor_div8((int) floor(h.z));
    if (f % 37 == 20) { give_tile(r, tile_key(tx, tz), (f / 37) % 2 ? 0.6 : 1.0); return 1; }
    if (f % 97 == 10) { r3_drop(r, tile_key(tx + 1, tz)); return 1; }
    if (f % 97 == 60) { give_tile(r, tile_key(tx + 1, tz), 1.0); return 1; }
    if (f == 300) { r3_build(r, spot[0] + 3, spot[2], spot[1] - 2); return 1; }
    /* The light of the cells round the hero, as a torch carried and flickering gives it. */
    if (f % 23 == 5) {
        float v[9 * 9];
        for (int i = 0; i < 81; i++) v[i] = (f / 23) % 2 ? 0.35 + 0.05 * (i % 9) : 1.0;
        for (int z = spot[2] - 2; z <= spot[2] + 1; z++) r3_light(r, z, (int) h.x - 4, (int) h.z - 4, 9, 9, v);
        return 1;
    }
    return 0;
}

static uint16_t fb[2][R3_SW * R3_SH], ref[R3_SW * R3_SH];

static Result run(int slide, int check, int control, int changes) {
    R3 *r = scene();
    int camx = 0, camy = 0;
    {
        P2 h = hero_at(0);
        camx = (int) lround(h.x * R3_PX) - R3_SW / 2;
        camy = (int) lround(h.z * R3_PZ - spot[2] * R3_PY) - R3_SH / 2;
    }
    Result res = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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
        if (changes) res.changes += change(r, f);
        r3_camera(r, camx, camy);
        if (changes) give_overlay(r, f, camx, camy);
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
    printf("scene %s: %d faces, %d pictures, round (%d, %d, %d); %d frames of %d x %d, 7 actors (the hero and what it holds posed)\n", argv[1], nfaces,
           npics, spot[0], spot[1], spot[2], FRAMES, R3_SW, R3_SH);
    printf("  %-8s %-10s %9s %9s %12s %12s %12s %8s\n", "camera", "", "mean us", "max us", "B px/frame",
           "copy px/fr", "actor px/fr", "rects");
    int skip = argc > 2 && !strcmp(argv[2], "control");
    if (skip) { /* built with -DR3_SKIP_DIRTY */
        Result c = run(1, 1, 0, 1);
        printf("  control: a change not drawn again: %d of %d frames differ (must not be 0)\n", c.differ_frames, FRAMES);
        return 0;
    }
    for (int slide = 0; slide < 2; slide++) {
        Result chk = run(slide, 1, 0, 0);
        printf("  %-8s %-10s %9s %9s %12s %12s %12s %8s   %d of %d frames differ from scratch (%lld px); walls cut"
               " away in %d\n", slide ? "slide" : "follow", "checked", "", "", "", "", "", "", chk.differ_frames, FRAMES,
               (long long) chk.differ_px, chk.cut_frames);
        Result t = run(slide, 0, 0, 0);
        printf("  %-8s %-10s %9.0f %9.0f %12lld %12lld %12lld %8.1f\n", slide ? "slide" : "follow", "timed", t.mean_us,
               t.max_us, (long long) (t.b_px / FRAMES), (long long) (t.fb_px / FRAMES), (long long) (t.actor_px / FRAMES),
               (double) t.rects / FRAMES);
        Result cc = run(slide, 1, 0, 1);
        printf("  %-8s %-10s %9s %9s %12s %12s %12s %8s   %d of %d frames differ, with %d changes of the landscape\n",
               slide ? "slide" : "follow", "changes", "", "", "", "", "", "", cc.differ_frames, FRAMES, cc.changes);
        Result ct = run(slide, 0, 0, 1);
        printf("  %-8s %-10s %9.0f %9.0f %12lld %12lld %12lld %8.1f\n", slide ? "slide" : "follow", "timed", ct.mean_us,
               ct.max_us, (long long) (ct.b_px / FRAMES), (long long) (ct.fb_px / FRAMES), (long long) (ct.actor_px / FRAMES),
               (double) ct.rects / FRAMES);
    }
    Result ctl = run(0, 1, 1, 0);
    printf("  control: the hero drawn a pixel off after the frame: %d of %d frames differ (must not be 0)\n",
           ctl.differ_frames, FRAMES);
    /* An empty tile (an 8 x 8 of void: no faces, so no box and no bins) with the camera over the
     * world's origin, where its empty box lies: drawn round, not into. */
    {
        R3 *e = scene();
        r3_tile(e, tile_key(0, 0) + 7777);
        r3_camera(e, -R3_SW / 2, -R3_SH / 2);
        actors(e, 0, 0);
        r3_frame(e, fb[0], 0);
        r3_reference(e, ref);
        int64_t d = 0;
        for (int i = 0; i < R3_SW * R3_SH; i++) d += fb[0][i] != ref[i];
        printf("  an empty tile, the camera over the origin: drawn, %lld px differ from scratch\n", (long long) d);
        r3_free(e);
    }
    /* The light of dropped tiles forgotten: every cell round the place lit, half the tiles dropped,
     * one given back — the store keeps what the kept tiles are lit from, and every frame is still
     * the scene drawn from scratch. */
    {
        R3 *e = scene();
        P2 h = hero_at(0);
        r3_camera(e, (int) lround(h.x * R3_PX) - R3_SW / 2, (int) lround(h.z * R3_PZ - spot[2] * R3_PY) - R3_SH / 2);
        static float lv[81 * 81];
        for (int i = 0; i < 81 * 81; i++) lv[i] = 0.4f + 0.6f * (float) ((i * 7) % 10) / 10.0f;
        for (int z = spot[2] - 3; z <= spot[2] + 1; z++) r3_light(e, z, spot[0] - 40, spot[1] - 40, 81, 81, lv);
        actors(e, 0, 0);
        r3_frame(e, fb[0], 0);
        int all = r3_count(e).lights;
        /* Drop the tiles west of the hero's. */
        int dropped = -1, ndrop = 0;
        for (int tz = floor_div8(spot[1]) - 6; tz <= floor_div8(spot[1]) + 6; tz++)
            for (int tx = floor_div8(spot[0]) - 6; tx < floor_div8(spot[0]); tx++) {
                r3_drop(e, tile_key(tx, tz));
                if (dropped < 0) dropped = tile_key(tx, tz);
                ndrop++;
            }
        r3_frame(e, fb[0], 0);
        int kept = r3_count(e).lights;
        r3_reference(e, ref);
        int64_t d = 0;
        for (int i = 0; i < R3_SW * R3_SH; i++) d += fb[0][i] != ref[i];
        /* One given back, its cells' light forgotten: lit by its own corners again. */
        give_tile(e, tile_key(floor_div8(spot[0]) - 1, floor_div8(spot[1])), 1.0);
        r3_frame(e, fb[0], 0);
        r3_reference(e, ref);
        int64_t d2 = 0;
        for (int i = 0; i < R3_SW * R3_SH; i++) d2 += fb[0][i] != ref[i];
        printf("  the light of dropped tiles: %d cells lit, %d tile keys dropped, %d cells kept; %lld px differ from scratch,"
               " %lld after one given back\n", all, ndrop, kept, (long long) d, (long long) d2);
        r3_free(e);
    }
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
