# Write hexhouses.html from build/hexhouses.json (tools/hexhouses.loft): the terrain's hex grid,
# each house's cells, the thin walls the rules use (its boundary edges) and its straight walls as
# hexhouse.loft solves them (tops walls and sides walls), as one SVG on a page of its own.
#
#   loft --lib libs tools/hexhouses.loft > build/hexhouses.json && python3 tools/hexhouses.py
#
# Copyright (c) 2026 Jurjen Stellingwerff
# SPDX-License-Identifier: LGPL-3.0-or-later
import json, math, sys

S3 = math.sqrt(3)
CK = [0, -1, -1, 0, 1, 1]          # hex_field's corner order, doubled coordinates
CM = [2, 1, -1, -2, -1, 1]
BAND = S3 / 2                       # a drawn wall's thickness, ROUNDTRIP §6.2 (0.75 m)

src = sys.argv[1] if len(sys.argv) > 1 else "build/hexhouses.json"
out = sys.argv[2] if len(sys.argv) > 2 else "hexhouses.html"
d = json.load(open(src))
q0, r0, w, h = d["terrain"]
x_max = (2 * (q0 + w) + 1) * S3 / 2 + 1
y_max = 3 * (r0 + h - 1) / 2 + 2

def pt(x, y):                       # world -> SVG, y up
    return f"{x:.3f},{y_max - y:.3f}"

def hexagon(q, r):
    k, m = 2 * q + (r & 1), 3 * r
    return " ".join(pt((k + CK[i]) * S3 / 2, (m + CM[i]) / 2) for i in range(6))

grid = "".join(f'<polygon points="{hexagon(q, r)}"/>' for r in range(r0, r0 + h) for q in range(q0, q0 + w))
floors, thin, tops, sides, skew, labels, rows = [], [], [], [], [], [], []
for hs in d["houses"]:
    turn = hs["turn"]
    axes = {(turn + 3 * i) % 12 for i in range(4)}
    floors += [f'<polygon points="{hexagon(q, r)}"/>' for q, r in hs["cells"]]
    thin += [f'<line x1="{ak * S3 / 2:.3f}" y1="{y_max - am / 2:.3f}" x2="{bk * S3 / 2:.3f}" y2="{y_max - bm / 2:.3f}"/>'
             for ak, am, bk, bm in hs["edges"]]
    cs = hs["corners"]
    on_axis = 0
    for i in range(len(cs)):
        # wall i runs from corner i-1 to corner i (corner i is where wall i meets wall i+1)
        line = f'<line x1="{cs[i - 1][0]:.3f}" y1="{y_max - cs[i - 1][1]:.3f}" x2="{cs[i][0]:.3f}" y2="{y_max - cs[i][1]:.3f}"/>'
        if hs["exact"][i] in axes:
            (tops if hs["exact"][i] % 2 == 0 else sides).append(line)
            on_axis += 1
        else:
            skew.append(line)
    # The label just above the house's highest corner, over its middle.
    mid_x = sum(c[0] for c in cs) / len(cs)
    top = max(c[1] for c in cs)
    labels.append(f'<text x="{mid_x:.3f}" y="{y_max - top - 1.2:.3f}">{30 * turn}°</text>')
    family = "edge" if turn % 2 == 0 else "vertex"
    rows.append(f"<tr><td>{30 * turn}°</td><td>{family}</td><td>{len(hs['cells'])}</td><td>{len(hs['edges'])}</td>"
                f"<td>{on_axis} of {len(cs)}</td></tr>")

svg = (f'<svg viewBox="0 0 {x_max:.2f} {y_max:.2f}" role="img" aria-label="Twelve L-shaped houses on a hex grid">'
       f'<g class="grid">{grid}</g><g class="floor">{"".join(floors)}</g>'
       f'<g class="tops" stroke-width="{BAND:.4f}">{"".join(tops)}</g>'
       f'<g class="sides" stroke-width="{BAND:.4f}">{"".join(sides)}</g>'
       f'<g class="skew" stroke-width="{BAND:.4f}">{"".join(skew)}</g>'
       f'<g class="thin">{"".join(thin)}</g><g class="label">{"".join(labels)}</g></svg>')

page = f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Hex houses</title>
<style>
  :root {{
    --bg: #f6efe2; --panel: #fffaf1; --ink: #2b2230; --muted: #6b5f6e; --line: #e2d5bf;
    --accent: #c8642f; --grass: #d6e4c4; --grass-line: #aabd9e; --floor: #ecd6aa;
    --floor-line: #c4ac80; --thin: #c81e1e; --tops: rgba(50, 60, 120, 0.7); --sides: rgba(40, 110, 70, 0.75);
    --skew: rgba(235, 130, 20, 0.85);
  }}
  @media (prefers-color-scheme: dark) {{
    :root:not([data-theme="light"]) {{
      --bg: #1d1922; --panel: #26212d; --ink: #efe7f2; --muted: #b2a6b8; --line: #3a3343;
      --accent: #e8834c; --grass: #2f3b2a; --grass-line: #46563f; --floor: #6b5a3c;
      --floor-line: #8a7550; --thin: #ff6b5b; --tops: rgba(150, 170, 255, 0.7); --sides: rgba(120, 220, 150, 0.7);
      --skew: rgba(255, 160, 60, 0.9);
    }}
  }}
  :root[data-theme="dark"] {{
    --bg: #1d1922; --panel: #26212d; --ink: #efe7f2; --muted: #b2a6b8; --line: #3a3343;
    --accent: #e8834c; --grass: #2f3b2a; --grass-line: #46563f; --floor: #6b5a3c;
    --floor-line: #8a7550; --thin: #ff6b5b; --tops: rgba(150, 170, 255, 0.7); --sides: rgba(120, 220, 150, 0.7);
      --skew: rgba(255, 160, 60, 0.9);
  }}
  body {{ margin: 0; background: var(--bg); color: var(--ink); font: 16px/1.55 system-ui, sans-serif; }}
  main {{ max-width: 1100px; margin: 0 auto; padding: 24px 16px 48px; }}
  h1 {{ margin: 0 0 4px; font-size: 1.7rem; }}
  .lead {{ color: var(--muted); margin: 0 0 16px; }}
  a {{ color: var(--accent); }}
  figure {{ margin: 16px 0; background: var(--panel); border: 1px solid var(--line); border-radius: 8px; padding: 8px; }}
  svg {{ display: block; width: 100%; height: auto; }}
  .grid polygon {{ fill: var(--grass); stroke: var(--grass-line); stroke-width: 0.05; }}
  .floor polygon {{ fill: var(--floor); stroke: var(--floor-line); stroke-width: 0.05; }}
  .tops line {{ stroke: var(--tops); stroke-linecap: square; }}
  .sides line {{ stroke: var(--sides); stroke-linecap: square; }}
  .skew line {{ stroke: var(--skew); stroke-linecap: square; }}
  .thin line {{ stroke: var(--thin); stroke-width: 0.14; }}
  .label text {{ fill: var(--ink); font-size: 2px; text-anchor: middle; }}
  ul.key {{ list-style: none; padding: 0; display: flex; flex-wrap: wrap; gap: 8px 20px; color: var(--muted); }}
  .sw {{ display: inline-block; width: 14px; height: 10px; margin-right: 6px; vertical-align: middle; border-radius: 2px; }}
  table {{ border-collapse: collapse; width: 100%; max-width: 560px; font-variant-numeric: tabular-nums; }}
  th, td {{ text-align: left; padding: 4px 10px; border-bottom: 1px solid var(--line); }}
  th {{ color: var(--muted); font-weight: 600; }}
  .scroll {{ overflow-x: auto; }}
</style>
</head>
<body>
<main>
  <p><a href="/">← tile_world</a></p>
  <h1>Hex houses</h1>
  <p class="lead">One L-shaped house at all twelve 30° angles on one empty terrain, with the hex grid
  showing.  The house is its cells; the walls follow from them (WORLD.md § One game, square or hex).</p>
  <ul class="key">
    <li><span class="sw" style="background:var(--grass)"></span>terrain cells</li>
    <li><span class="sw" style="background:var(--floor)"></span>the house's cells</li>
    <li><span class="sw" style="background:var(--thin)"></span>the thin walls the rules use: the edges between inside and outside</li>
    <li><span class="sw" style="background:var(--tops)"></span>a tops wall: the wobble across the tops of a row of hexes</li>
    <li><span class="sw" style="background:var(--sides)"></span>a sides wall: along the sides of hexes under each other</li>
    <li><span class="sw" style="background:var(--skew)"></span>a wall off the house's axes (none should be)</li>
  </ul>
  <figure>{svg}</figure>
  <p>Even angles are the grid's own edge directions, odd angles its corner directions: a 30° turn is no
  symmetry of the hex grid, so the two families give different cells.  A house's two axes are always one
  of each, so every house has both kinds of straight wall.  Each is solved from the wall edges alone
  (<code>hexhouse::solve_tops</code>, <code>solve_sides</code>): turned to its own frame, the line runs through
  the midpoints its pattern puts on it (every edge across the tops; every slanted connector along the
  sides), along the wall's heading exactly, mitred where two walls meet.</p>
  <div class="scroll"><table>
    <thead><tr><th>Angle</th><th>Family</th><th>Cells</th><th>Edges</th><th>Walls on the axes</th></tr></thead>
    <tbody>{"".join(rows)}</tbody>
  </table></div>
  <p class="lead">Made by <code>tools/hexhouses.loft</code> and <code>tools/hexhouses.py</code> from <code>hexhouse.loft</code>.</p>
</main>
</body>
</html>
"""
open(out, "w").write(page)
print(out, len(page), "bytes")
