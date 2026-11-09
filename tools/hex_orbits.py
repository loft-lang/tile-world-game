# How far a house's boundary edges (the thin wall the rules use) stray from its continuous sides,
# at each of the twelve 30-degree turns (WORLD.md § One game, square or hex: houses at every 30°).
# Each edge belongs to the side it lies furthest outside.  Run: python3 tools/hex_orbits.py  Lattice as hex_grid: centre(q,r) = (2q + (r&1), 3r) in (k, m),
# x = k*sqrt3/2, y = m/2; hex step sqrt3 world units (1.5 m), circumradius 1 (0.866 m).
import math
S3 = math.sqrt(3)
def centre(q, r): return ((2*q + (r & 1)) * S3/2, 3*r/2)
def nbrs(q, r):
    if r & 1: d = [(1,0),(-1,0),(0,-1),(1,-1),(0,1),(1,1)]
    else:     d = [(1,0),(-1,0),(-1,-1),(0,-1),(-1,1),(0,1)]
    return [(q+a, r+b) for a,b in d]
def run(wid, dep, rot):
    th = math.radians(30*rot); co, si = math.cos(th), math.sin(th)
    hw, hd = wid*S3/2, dep*S3/2
    inside = set()
    for q in range(-20, 21):
        for r in range(-20, 21):
            x, y = centre(q, r)
            u, v = x*co + y*si, -x*si + y*co
            if abs(u) <= hw + 1e-9 and abs(v) <= hd + 1e-9: inside.add((q, r))
    sides = {k: [] for k in ("+u","-u","+v","-v")}
    for c in inside:
        for n in nbrs(*c):
            if n in inside: continue
            (x1,y1),(x2,y2) = centre(*c), centre(*n)
            mx, my = (x1+x2)/2, (y1+y2)/2
            dx, dy = x2-x1, y2-y1
            du, dv = dx*co + dy*si, -dx*si + dy*co
            u, v = mx*co + my*si, -mx*si + my*co
            k, d = max((("+u", u - hw), ("-u", -u - hw), ("+v", v - hd), ("-v", -v - hd)), key=lambda t: t[1])
            sides[k].append(d)
    out = []
    for k, ds in sides.items():
        m = sum(ds)/len(ds)
        out.append((k, len(ds), m, max(ds)-min(ds), max(abs(d-m) for d in ds)))
    return len(inside), out
for wid, dep in ((5, 4), (3, 3), (8, 5)):
    print(f"box {wid} x {dep} hex steps ({wid*1.5:.1f} x {dep*1.5:.1f} m); offsets in m (1 world unit = 0.866 m)")
    for rot in range(12):
        n, out = run(wid, dep, rot)
        worst = max(o[4] for o in out); spread = max(o[3] for o in out)
        means = " ".join(f"{o[0]}:{o[2]*0.866:+.2f}" for o in out)
        print(f"  rot {rot:2d} ({30*rot:3d} deg, {'edge  ' if rot%2==0 else 'vertex'}) cells {n:3d}  edges straying from their mean line: at most {worst*0.866:.2f} m (spread {spread*0.866:.2f})  mean line from the side: {means}")
