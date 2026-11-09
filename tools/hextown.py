# Write hextown.html from build/hextown.json (tools/hextown.loft): the town ported to the hex grid
# and a hill with a house at every angle, in 3D (three.js), to orbit round.
#
#   loft --lib libs tools/hextown.loft > build/hextown.json && python3 tools/hextown.py
#
# The ground is a smooth mesh: each cell a fan of six triangles from its centre, each corner at the
# mean height of the three cells round it, coloured by its surface.  A house's walls stand on its
# outline (the straight walls' corners) from the lowest ground under it to its eaves, as thick as
# hexsizes.json says, with room left for its doors (the ground storey) and windows (every storey),
# a mesh in each; a hipped roof on a rectangular house, a flat one on any other.
# Copyright (c) 2026 Jurjen Stellingwerff
# SPDX-License-Identifier: LGPL-3.0-or-later
import json, sys

src = sys.argv[1] if len(sys.argv) > 1 else "build/hextown.json"
out = sys.argv[2] if len(sys.argv) > 2 else "hextown.html"
data = json.load(open(src))

page = """<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Hex town</title>
<style>
  :root { --bg: #f6efe2; --panel: #fffaf1; --ink: #2b2230; --muted: #6b5f6e; --line: #e2d5bf; --accent: #c8642f; }
  @media (prefers-color-scheme: dark) {
    :root:not([data-theme="light"]) { --bg: #1d1922; --panel: #26212d; --ink: #efe7f2; --muted: #b2a6b8; --line: #3a3343; --accent: #e8834c; }
  }
  :root[data-theme="dark"] { --bg: #1d1922; --panel: #26212d; --ink: #efe7f2; --muted: #b2a6b8; --line: #3a3343; --accent: #e8834c; }
  html, body { margin: 0; height: 100%; background: var(--bg); color: var(--ink); font: 15px/1.5 system-ui, sans-serif; }
  header { padding: 10px 16px; display: flex; flex-wrap: wrap; gap: 8px 16px; align-items: center; border-bottom: 1px solid var(--line); }
  header h1 { font-size: 1.15rem; margin: 0; }
  header a { color: var(--accent); }
  button { font: inherit; padding: 4px 12px; border: 1px solid var(--line); border-radius: 6px; background: var(--panel); color: var(--ink); cursor: pointer; }
  button.on { border-color: var(--accent); color: var(--accent); }
  #note { color: var(--muted); font-size: 0.9rem; }
  #view { position: absolute; top: 56px; left: 0; right: 0; bottom: 0; }
  @media (max-width: 600px) { #view { top: 110px; } }
</style>
</head>
<body>
<header>
  <a href="/">← tile_world</a>
  <h1>Hex town</h1>
  <span id="buttons"></span>
  <span id="note">drag to turn, scroll to zoom, right-drag to move</span>
</header>
<div id="view"></div>
<script src="https://cdn.jsdelivr.net/npm/three@0.128.0/build/three.min.js"></script>
<script src="https://cdn.jsdelivr.net/npm/three@0.128.0/examples/js/controls/OrbitControls.js"></script>
<script>
const DATA = __DATA__;
const CK = [0, -1, -1, 0, 1, 1], CM = [2, 1, -1, -2, -1, 1];
const KIND = { 0: [0.42, 0.62, 0.30], 1: [0.78, 0.68, 0.48], 2: [0.50, 0.46, 0.42], 3: [0.62, 0.53, 0.40], 4: [0.33, 0.55, 0.78], 5: [0.62, 0.48, 0.32] };
const STOREY = DATA.storey, WALL = DATA.wall;

function groundMesh(g) {
  const corners = new Map(), pos = [], col = [], idx = [];
  const lk = (q, r) => 2 * q + (r & 1);
  const cellAt = (q, r) => (r - g.r0) * g.w + (q - g.q0);
  for (let r = g.r0; r < g.r0 + g.h; r++) for (let q = g.q0; q < g.q0 + g.w; q++) {
    const i = cellAt(q, r), h = g.heights[i], c = KIND[g.kinds[i]] || KIND[0], k = lk(q, r), m = 3 * r;
    for (let j = 0; j < 6; j++) {
      const key = (k + CK[j]) + "," + (m + CM[j]);
      let e = corners.get(key);
      if (!e) { e = { k: k + CK[j], m: m + CM[j], h: 0, c: [0, 0, 0], n: 0, v: -1 }; corners.set(key, e); }
      e.h += h; e.n += 1; e.c[0] += c[0]; e.c[1] += c[1]; e.c[2] += c[2];
    }
  }
  const vertex = (x, h, y, c) => { pos.push(x, h, -y); col.push(c[0], c[1], c[2]); return pos.length / 3 - 1; };
  for (const e of corners.values()) e.v = vertex(e.k * 0.75, e.h / e.n, e.m * 0.4330127, e.c.map(v => v / e.n));
  for (let r = g.r0; r < g.r0 + g.h; r++) for (let q = g.q0; q < g.q0 + g.w; q++) {
    const i = cellAt(q, r), k = lk(q, r), m = 3 * r;
    const shade = 0.9 + 0.1 * Math.min(1, Math.max(0, g.heights[i] / 10));
    const c = (KIND[g.kinds[i]] || KIND[0]).map(v => v * shade);
    const centre = vertex(k * 0.75, g.heights[i], m * 0.4330127, c);
    for (let j = 0; j < 6; j++) {
      const a = corners.get((k + CK[j]) + "," + (m + CM[j])), b = corners.get((k + CK[(j + 1) % 6]) + "," + (m + CM[(j + 1) % 6]));
      idx.push(centre, a.v, b.v);
    }
  }
  const geo = new THREE.BufferGeometry();
  geo.setAttribute("position", new THREE.Float32BufferAttribute(pos, 3));
  geo.setAttribute("color", new THREE.Float32BufferAttribute(col, 3));
  geo.setIndex(idx);
  geo.computeVertexNormals();
  return new THREE.Mesh(geo, new THREE.MeshLambertMaterial({ vertexColors: true }));
}

const MAT = {
  wall: new THREE.MeshLambertMaterial({ color: 0xe6dcc6 }),
  plinth: new THREE.MeshLambertMaterial({ color: 0x9c948a }),
  door: new THREE.MeshLambertMaterial({ color: 0x6b4226 }),
  glass: new THREE.MeshLambertMaterial({ color: 0x7fb8e6, transparent: true, opacity: 0.7 }),
  roof: new THREE.MeshLambertMaterial({ color: 0xa4553a, side: THREE.DoubleSide }),
  slate: new THREE.MeshLambertMaterial({ color: 0x59606c, side: THREE.DoubleSide }),
  trunk: new THREE.MeshLambertMaterial({ color: 0x6b4a2e }),
  pine: new THREE.MeshLambertMaterial({ color: 0x2f5d3a }),
  oak: new THREE.MeshLambertMaterial({ color: 0x4f8a3a }),
  autumn: new THREE.MeshLambertMaterial({ color: 0xc8782a }),
  poplar: new THREE.MeshLambertMaterial({ color: 0x5f9a40 }),
  stone: new THREE.MeshLambertMaterial({ color: 0x8f8a84 }),
  wood: new THREE.MeshLambertMaterial({ color: 0x8a6a44 }),
};
const BOX = new THREE.BoxGeometry(1, 1, 1);

// A box from s0 to s1 along the wall from a (direction u), from height z0 to z1, `t` thick.
function wallBox(group, a, u, s0, s1, z0, z1, t, mat) {
  if (s1 - s0 < 1e-3 || z1 - z0 < 1e-3) return;
  const m = new THREE.Mesh(BOX, mat);
  const s = (s0 + s1) / 2;
  m.position.set(a[0] + u[0] * s, (z0 + z1) / 2, -(a[1] + u[1] * s));
  m.scale.set(s1 - s0, z1 - z0, t);
  m.rotation.y = Math.atan2(u[1], u[0]);
  group.add(m);
}

function house(group, h) {
  const cs = h.corners, n = cs.length, storeys = Math.max(1, Math.round((h.top - h.floor) / STOREY));
  for (let i = 0; i < n; i++) {
    const a = cs[(i + n - 1) % n], b = cs[i], L = Math.hypot(b[0] - a[0], b[1] - a[1]);
    // run the wall on past each corner by half its thickness, so the corners close
    const u = [(b[0] - a[0]) / L, (b[1] - a[1]) / L], ext = WALL / 2;
    const a2 = [a[0] - u[0] * ext, a[1] - u[1] * ext], len = L + 2 * ext;
    wallBox(group, a2, u, 0, len, h.base, h.floor, WALL, MAT.plinth);
    const spans = h.openings.filter(o => o.wall === i).map(o => {
      const s0 = (o.a[0] - a2[0]) * u[0] + (o.a[1] - a2[1]) * u[1], s1 = (o.b[0] - a2[0]) * u[0] + (o.b[1] - a2[1]) * u[1];
      return { s0: Math.min(s0, s1), s1: Math.max(s0, s1), kind: o.kind };
    }).sort((p, q) => p.s0 - q.s0);
    let at = 0;
    for (const sp of spans) {
      wallBox(group, a2, u, at, sp.s0, h.floor, h.top, WALL, MAT.wall);
      // the column over the opening: wall round a hole in every storey
      let z = h.floor;
      for (let st = 0; st < storeys; st++) {
        const z0 = h.floor + st * STOREY, door = sp.kind === 1 && st === 0;
        const lo = door ? z0 : z0 + 0.9, hi = z0 + (door ? 2.1 : 2.0);
        wallBox(group, a2, u, sp.s0, sp.s1, z, lo, WALL, MAT.wall);
        wallBox(group, a2, u, sp.s0, sp.s1, lo, hi, door ? 0.12 : 0.06, door ? MAT.door : MAT.glass);
        z = hi;
      }
      wallBox(group, a2, u, sp.s0, sp.s1, z, h.top, WALL, MAT.wall);
      at = sp.s1;
    }
    wallBox(group, a2, u, at, len, h.floor, h.top, WALL, MAT.wall);
  }
  group.add(roof(h, storeys > 1 ? MAT.slate : MAT.roof));
}

// A hipped roof on four corners, a flat one on more; 0.4 m over the walls.
function roof(h, mat) {
  const cs = h.corners, n = cs.length, over = 0.4 + WALL / 2;
  const cx = cs.reduce((s, c) => s + c[0], 0) / n, cy = cs.reduce((s, c) => s + c[1], 0) / n;
  if (n !== 4) {
    const shape = new THREE.Shape(cs.map(c => new THREE.Vector2(c[0], c[1])));
    const geo = new THREE.ExtrudeGeometry(shape, { depth: 0.3, bevelEnabled: false });
    const m = new THREE.Mesh(geo, mat);
    m.rotation.x = -Math.PI / 2;
    m.position.y = h.top;
    return m;
  }
  const e0 = [cs[1][0] - cs[0][0], cs[1][1] - cs[0][1]], e1 = [cs[2][0] - cs[1][0], cs[2][1] - cs[1][1]];
  const l0 = Math.hypot(...e0), l1 = Math.hypot(...e1);
  const [la, lb, ua, ub] = l0 >= l1 ? [l0, l1, e0.map(v => v / l0), e1.map(v => v / l1)] : [l1, l0, e1.map(v => v / l1), e0.map(v => v / l0)];
  const A = la / 2 + over, B = lb / 2 + over, rise = 0.45 * lb, ridge = Math.max(0, A - B);
  const P = (s, t, z) => [cx + ua[0] * s + ub[0] * t, z, -(cy + ua[1] * s + ub[1] * t)];
  const c1 = P(-A, -B, h.top), c2 = P(A, -B, h.top), c3 = P(A, B, h.top), c4 = P(-A, B, h.top);
  const r1 = P(-ridge, 0, h.top + rise), r2 = P(ridge, 0, h.top + rise);
  const tri = [c1, c2, r2, c1, r2, r1, c3, c4, r1, c3, r1, r2, c2, c3, r2, c4, c1, r1].flat();
  const geo = new THREE.BufferGeometry();
  geo.setAttribute("position", new THREE.Float32BufferAttribute(tri, 3));
  geo.computeVertexNormals();
  return new THREE.Mesh(geo, mat);
}

function thing(group, t) {
  const at = (m, x, y, z) => { m.position.set(x, z, -y); group.add(m); return m; };
  const k = t.kind;
  if (k === "pine") {
    at(new THREE.Mesh(new THREE.CylinderGeometry(0.15, 0.2, 1.4, 6), MAT.trunk), t.x, t.y, t.z + 0.7);
    at(new THREE.Mesh(new THREE.ConeGeometry(1.3, 5, 8), MAT.pine), t.x, t.y, t.z + 3.6);
  } else if (k === "oak" || k === "autumn_oak") {
    at(new THREE.Mesh(new THREE.CylinderGeometry(0.2, 0.28, 2.6, 6), MAT.trunk), t.x, t.y, t.z + 1.3);
    at(new THREE.Mesh(new THREE.SphereGeometry(2.0, 10, 8), k === "oak" ? MAT.oak : MAT.autumn), t.x, t.y, t.z + 3.8);
  } else if (k === "poplar" || k === "autumn_poplar") {
    at(new THREE.Mesh(new THREE.CylinderGeometry(0.15, 0.2, 1.6, 6), MAT.trunk), t.x, t.y, t.z + 0.8);
    const m = at(new THREE.Mesh(new THREE.SphereGeometry(1, 10, 8), k === "poplar" ? MAT.poplar : MAT.autumn), t.x, t.y, t.z + 4.2);
    m.scale.set(0.9, 3.2, 0.9);
  } else if (k === "bush") {
    at(new THREE.Mesh(new THREE.SphereGeometry(0.9, 8, 6), MAT.oak), t.x, t.y, t.z + 0.6);
  } else if (k === "garden_fence") {
    const m = at(new THREE.Mesh(BOX, MAT.wood), t.x, t.y, t.z + 0.5); m.scale.set(1.5, 1.0, 0.12);
  } else if (k === "village_well") {
    at(new THREE.Mesh(new THREE.CylinderGeometry(0.9, 0.9, 1.0, 14), MAT.stone), t.x, t.y, t.z + 0.5);
  } else if (k === "bridge") {
    const m = at(new THREE.Mesh(BOX, MAT.wood), t.x, t.y, t.z - 0.15); m.scale.set(1.5, 0.3, 1.5);
  }
}

const view = document.getElementById("view");
const renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setPixelRatio(window.devicePixelRatio);
view.appendChild(renderer.domElement);
const camera = new THREE.PerspectiveCamera(45, 1, 0.5, 2000);
const controls = new THREE.OrbitControls(camera, renderer.domElement);
let current = null;
const scenes = DATA.scenes.map(s => {
  const scene = new THREE.Scene();
  scene.background = new THREE.Color(0xb9d6ec);
  scene.add(new THREE.HemisphereLight(0xe8f2ff, 0x5a5040, 0.75));
  const sun = new THREE.DirectionalLight(0xffffff, 0.75); sun.position.set(-60, 120, 80); scene.add(sun);
  const group = new THREE.Group();
  group.add(groundMesh(s.ground));
  for (const h of s.houses) house(group, h);
  for (const t of s.things) thing(group, t);
  scene.add(group);
  // look at the middle of the ground, from the south-east above
  const g = s.ground, lk = (q, r) => 2 * q + (r & 1);
  const mx = (lk(g.q0, 0) + lk(g.q0 + g.w, 0)) / 2 * 0.75, my = (g.r0 + g.h / 2) * 3 * 0.4330127;
  const span = Math.max(g.w * 1.5, g.h * 1.3);
  return { name: s.name, scene, target: new THREE.Vector3(mx, 3, -my), eye: new THREE.Vector3(mx + span * 0.35, span * 0.55, -my + span * 0.75) };
});
function show(i) {
  current = scenes[i];
  camera.position.copy(current.eye); controls.target.copy(current.target); controls.update();
  document.querySelectorAll("#buttons button").forEach((b, j) => b.classList.toggle("on", j === i));
}
const buttons = document.getElementById("buttons");
scenes.forEach((s, i) => { const b = document.createElement("button"); b.textContent = s.name; b.onclick = () => show(i); buttons.appendChild(b); });
function resize() {
  const w = view.clientWidth, h = view.clientHeight;
  renderer.setSize(w, h); camera.aspect = w / Math.max(1, h); camera.updateProjectionMatrix();
}
window.addEventListener("resize", resize);
resize();
show(Math.min(scenes.length - 1, Math.max(0, +(new URLSearchParams(location.search).get("scene") || 0))));
(function loop() { requestAnimationFrame(loop); renderer.render(current.scene, camera); })();
</script>
</body>
</html>
"""
open(out, "w").write(page.replace("__DATA__", json.dumps(data, separators=(",", ":"))))
print(out, len(page), "bytes of page")
