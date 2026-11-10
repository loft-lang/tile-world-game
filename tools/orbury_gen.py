# Orbury's layout as code: writes levels/orbury.json's intent (the stream's course, the terraces'
# bending fronts, the roads, the houses placed between them, the inn, the foliage).  Run from the
# repository's root, then fill the place in from its intent:
#   python3 tools/orbury_gen.py levels/orbury.json && loft --lib libs tools/fill.loft orbury
# The intent in levels/orbury.json is what the game and the editor read; this is how it is made.
# Copyright (c) 2026 Jurjen Stellingwerff
# SPDX-License-Identifier: LGPL-3.0-or-later
import json, sys
W = 88   # the town, and Thornfield at its east edge (LOCATIONS.md § 1, @ST-Farm)
grounds, trails, houses, slopes, items, blobs, rooms, edges = [], [], [], [], [], [], [], []
def ground(x, y, w, h, z, what): grounds.append({"at": [x, y], "size": [w, h], "z": z, "as": what})
def earth(x, y, w, h, top):  # solid hill under a terrace on layer `top`
    for z in range(0, top): ground(x, y, w, h, z, "wall")
def terrace(x, y, w, h, z, what="grass"):
    earth(x, y, w, h, z); ground(x, y, w, h, z, what)
def house(x, y, w, h, z, floors, basement=False, door_at=-1, wall="wood_wall", roof="house_roof", dealt=False, door=None):
    e = {"at": [x, y], "size": [w, h], "z": z, "floors": floors, "wall": wall, "roof": roof,
         "door": door or ("house_door" if wall == "wood_wall" else "stone_door")}
    if basement: e["basement"] = True
    if door_at >= 0: e["door_at"] = door_at
    if dealt: e["dealt"] = True   # an ordinary house, dealt from the templates (HOUSES.md § The deal)
    houses.append(e)
def trail(points, z, what="path", width=1):
    t = {"points": points, "z": z, "as": what}
    if width != 1: t["width"] = width
    trails.append(t)
def slope(x, y, z, up): slopes.append({"at": [x, y], "z": z, "up": up})
def stairs(x, y, z):  # a stair road's step: up from layer z to the same cell on z + 1
    ground(x, y, 1, 1, z, "stairs_up"); ground(x, y, 1, 1, z + 1, "stairs_down")

# Every line in town bends (owner, 2026-11-08/09: no natural stream is straight; on a square grid
# the straight lines are broken up by offsetting the houses, moving them up and down the hill, and
# foliage finishing the job).  A profile is the row a line keeps, run by run: (first column, row).
def at(profile, x):
    r = profile[0][1]
    for c in profile:
        if c[0] <= x: r = c[1]
    return r
# A line `off` rows from a profile, stepping where it steps: the step in the column on the higher
# row, down to the lower one.  Its cells, in order.
def line_cells(profile, off, x0, x1):
    cells = []
    for x in range(x0, x1 + 1):
        y = at(profile, x) + off
        if cells and cells[-1][1] != y:
            px, py = cells[-1]
            cells.append((px, y) if py < y else (x, py))
        cells.append((x, y))
    return cells
def corners(cells):
    pts = [cells[0]]
    for i in range(1, len(cells) - 1):
        a, b, c = cells[i - 1], cells[i], cells[i + 1]
        if (b[0] - a[0], b[1] - a[1]) != (c[0] - b[0], c[1] - b[1]): pts.append(b)
    pts.append(cells[-1])
    return [list(p) for p in pts]
def line(profile, off, x0, x1): return corners(line_cells(profile, off, x0, x1))

# What stays clear of foliage: every way, house, door, slope, stair and thing, by layer.
clear = set()
def keep(x, y, z, w=1, h=1, margin=0):
    for yy in range(y - margin, y + h + margin):
        for xx in range(x - margin, x + w + margin): clear.add((xx, yy, z))
def keep_cells(cells, z, width=1):
    for c in cells: keep(c[0], c[1], z, width, width)
def trail_cells(points):
    out = [tuple(points[0])]
    for i in range(1, len(points)):
        (x0, y0), (x1, y1) = points[i - 1], points[i]
        sx = 1 if x1 > x0 else -1
        for x in range(x0 + sx, x1 + sx, sx) if x1 != x0 else []: out.append((x, y0))
        sy = 1 if y1 > y0 else -1
        for y in range(y0 + sy, y1 + sy, sy) if y1 != y0 else []: out.append((x1, y))
    return out
_trail, _ground, _house, _slope, _stairs = trail, ground, house, slope, stairs
def trail(points, z, what="path", width=1):
    keep_cells(trail_cells(points), z, width); _trail(points, z, what, width)
def ground(x, y, w, h, z, what):
    if what not in ("grass", "wall", "void"): keep(x, y, z, w, h)
    _ground(x, y, w, h, z, what)
def house(x, y, w, h, z, floors, basement=False, door_at=-1, wall="wood_wall", roof="house_roof", dealt=False, door=None):
    keep(x, y, z, w, h, 1)
    d = door_at if door_at >= 0 else w // 2
    keep(x + d, y + h, z, 1, 2)
    _house(x, y, w, h, z, floors, basement, door_at, wall, roof, dealt, door)
# The terraces' fronts slope (owner, 2026-11-09), so a ramp is the front itself: its cells are kept
# clear as before, but they no longer rise (three rising cells beside a sloping front would only
# stop the hero walking onto them from the side).
def slope(x, y, z, up):
    keep(x - 1, y - 1, z, 5, 3); keep(x - 1, y - 1, z + 1, 5, 3)
def stairs(x, y, z):
    keep(x, y - 1, z, 1, 3); keep(x, y - 1, z + 1, 1, 3); _stairs(x, y, z)

# The hill, in terraces rising north; each terrace's front bends.  The first row of the terrace
# below, by column — fixed where a road's slope or a stair road climbs it, where the inn and its
# vault stand, and under the lord's house.
HILL = [(0, 9), (6, 11), (13, 10), (38, 9), (47, 11), (56, 12)]                       # hilltop | top
TOP = [(0, 20), (8, 22), (22, 21), (40, 22), (45, 23), (51, 21), (59, 22)]           # top | market
MARKET = [(0, 33), (7, 35), (16, 34), (26, 33), (33, 34), (40, 33), (48, 35), (56, 34)]  # market | middle
MIDDLE = [(0, 45), (6, 47), (13, 46), (20, 45), (27, 46), (35, 47), (43, 45), (51, 46)]  # middle | foot
FRONTS = [HILL, TOP, MARKET, MIDDLE]
LAYER = [4, 3, 2, 1]
x = 0
while x < W:
    rows = [at(f, x) for f in FRONTS]
    x1 = x
    while x1 + 1 < W and [at(f, x1 + 1) for f in FRONTS] == rows: x1 += 1
    w = x1 - x + 1
    top = 0
    for z, bottom in zip(LAYER, rows):
        for zz in range(0, z): _ground(x, top, w, bottom - top, zz, "wall")
        _ground(x, top, w, bottom - top, z, "grass")
        top = bottom
    _ground(x, top, w, 68 - top, 0, "grass")   # the foot, and across the stream the dock's side
    x = x1 + 1
# The stream (owner, 2026-11-08: no natural stream is straight; the rails and the houses follow its
# banks): its bed a layer down, three cells wide, its north edge row run by run along the course;
# banks of earth, the far side with the dock.
COURSE = [(0, 57), (4, 58), (9, 57), (12, 56), (15, 55), (23, 56), (36, 57), (40, 58), (44, 59),
          (51, 58), (55, 57), (59, 56), (66, 58), (75, 59)]   # past the cemetery it swings south, for the farm's fields
def north(x): return at(COURSE, x)   # the bed's first row at column x
runs = [(COURSE[i][0], (COURSE[i + 1][0] if i + 1 < len(COURSE) else W) - COURSE[i][0], COURSE[i][1])
        for i in range(len(COURSE))]
ground(0, 50, W, 18, -1, "wall")
for x0, w, n in runs:
    ground(x0, n, w, 3, -1, "path"); ground(x0, n, w, 3, 0, "void")
    keep(x0, n - 1, 0, w, 5)   # the bank's edges stay open: the view down, the ramps' tops
# What the drought left of the water: shallow pools, waded through (owner, 2026-11-08).
blobs.append({"at": [5, 58], "size": [4, 3], "z": -1, "as": "shallows"})
blobs.append({"at": [51, 58], "size": [3, 3], "z": -1, "as": "shallows"})
# Bridges over the bed: the stone bridge at the main road's foot (still used), two footbridges.
ground(29, 56, 3, 3, 0, "bridge")
ground(13, 56, 1, 3, 0, "bridge")
ground(56, 57, 1, 1, 0, "bridge"); ground(56, 59, 1, 1, 0, "bridge")   # a plank gone
# Ramps out of the bed, along each bank (owner, 2026-11-08: the bed is no trap): up east under the
# town's bank to the lane by the miners' rows, up west under the far bank to the dock's side.
slope(16, 55, -1, "E")
slope(48, 61, -1, "W")
# The lane along the town's bank, the miners' rows facing it; it follows the bank's bends.
LANE = line_cells(COURSE, -2, 0, W - 1)
trail(corners(LANE), 0, "path")
# The loading dock across the stream: its floor along the bank, the rails from the mine coming in
# from the east along the far bank, round its bends.
for x0, w, n in runs:
    a, b = max(x0, 24), min(x0 + w, 40)
    if a < b: ground(a, n + 4, b - a, 3, 0, "path")
trail(line(COURSE, 5, 26, W - 1)[::-1], 0, "rail")
keep_cells(line_cells(COURSE, 5, 26, W - 1), 0, 1)
for c in line_cells(COURSE, 5, 26, W - 1): keep(c[0], c[1] - 1, 0, 1, 3)
items.append({"at": [33, 61], "z": 0, "stack": ["cart"]})
items.append({"at": [37, 62], "z": 0, "stack": ["cart"]})
# The main road: from the stone bridge up to the market and on to the lord's house, by slopes.  Each
# landing is the road's two cells wide, so the road's brush rests on the terrace, not over the street.
# The road over the stone bridge, not on it: the bridge keeps its own stones.
ground(30, 59, 2, 1, 0, "path"); trail([[30, 55], [30, 47]], 0, "path", 2)
slope(30, 46, 0, "E"); ground(33, 46, 2, 1, 0, "wall"); ground(33, 46, 2, 1, 1, "path")
trail([[33, 45], [33, 35]], 1, "path", 2)
slope(35, 34, 1, "E"); ground(38, 34, 2, 1, 1, "wall"); ground(38, 34, 2, 1, 2, "path")
trail([[38, 33], [38, 23]], 2, "path", 2)
slope(40, 22, 2, "E"); ground(43, 22, 1, 1, 2, "wall"); ground(43, 22, 1, 1, 3, "path")
trail([[43, 21], [43, 20], [29, 20]], 3, "path")   # to the lord's door
# Streets along the terraces' fronts, bending with them: the middle terrace's four rows up from its
# front, the foot's just below it.
STREET1 = line_cells(MIDDLE, -4, 1, W - 2)
STREET0 = line_cells(MIDDLE, 1, 0, W - 1)
trail(corners(STREET1), 1, "path")
trail(corners(STREET0), 0, "path")
def row_of(cells, x): return max(c[1] for c in cells if c[0] == x)
def first_row(cells, x): return min(c[1] for c in cells if c[0] == x)
# The stair roads through the poorer quarters: short, steep, for the hero alone.
stairs(17, 45, 0); trail([[17, 46], [17, north(17) - 2]], 0, "path"); trail([[17, 44], [17, row_of(STREET1, 17)]], 1, "path")
stairs(24, 33, 1); trail([[24, 34], [24, first_row(STREET1, 24)]], 1, "path")
# The market: a square of path, the well in it; a way east along it to the well-off houses' doors.
ground(26, 24, 14, 8, 2, "path")
ground(8, 30, 18, 3, 2, "path")
ground(31, 27, 1, 1, 2, "village_well")
trail([[39, 31], [56, 31]], 2, "path")
# A house set into its terrace, `back` rows down from the line behind it (moved up or down the hill,
# so no row of houses lines up), its door's way down to the street or lane in front: rows below
# `lim` (the line in front, by column) stay free, and so does a row behind for the slack.
def placed(x, w, h, z, behind, before, back, path_z, floors=1, basement=False, door_at=-1, front=False, **kw):
    lo = max(behind(c) for c in range(x, x + w))
    hi = min(before(c) for c in range(x, x + w)) - h   # the top row that still leaves the door a row
    y = max(lo, hi - back) if front else max(lo, min(lo + back, hi))
    d = door_at if door_at >= 0 else w // 2
    house(x, y, w, h, z, floors, basement, d, **kw)
    out = before(x + d)
    if out > y + h: trail([[x + d, y + h], [x + d, out]], path_z, "path")
    return y
# The houses.  The lord's, at the top, its back against the hilltop.
house(22, 10, 16, 10, 3, 2, True, door_at=7, wall="stone_wall", roof="house_slate")
# Round the market: the inn, Agnes's beside it, a well-off house.
# The inn (LOCATIONS.md § 1, owner: its basement chest; PUZZLES.md: the town's taste of what lies
# ahead, off the main track and seen from it).  Abandoned and closed with care: its door, south onto
# the market, barred from inside.  Neglect opened what the care closed: a window of its upper floor,
# on the west side, broken; part of its roof came down.  Its cellar runs on under the market into a
# vault, where the chest stands walled in by stones fallen from the vault's wall; a grate in the
# market's floor looks down on it.
inn = {"at": [8, 22], "size": [13, 8], "z": 2, "floors": 2, "basement": True, "front": False,
       "wall": "wood_wall", "roof": "house_roof", "door": "house_door",
       "openings": [{"at": [14, 29, 2], "as": "barred"}, {"at": [10, 29, 1]}, {"at": [8, 25, 3]}],
       "holes": [[15, 25], [16, 25]]}
houses.append(inn)
rooms.append({"x": 9, "y": 29, "w": 5, "h": 5, "z": 1, "words": []})
# The grate: an opening in the market's floor over the vault, framed so nobody walks into it.
ground(11, 30, 1, 2, 2, "void")
for e in ((11, 30, "N"), (11, 30, "W"), (11, 30, "E"), (11, 31, "S"), (11, 31, "W"), (11, 31, "E")):
    edges.append({"at": [e[0], e[1], 2], "side": e[2], "what": "window"})
# Under the broken window, against the west wall: two tables left out; a chair nearby.
items.append({"at": [7, 24], "z": 2, "stack": ["table"]})
items.append({"at": [7, 25], "z": 2, "stack": ["table"]})
items.append({"at": [4, 27], "z": 2, "stack": ["chair"]})
# Upstairs: the bow a traveller left.
items.append({"at": [18, 27], "z": 3, "stack": ["bow"]})
# The cellar's chair; the vault: the chest under the grate, the fallen stones round it.
items.append({"at": [12, 26], "z": 1, "stack": ["chair"]})
items.append({"at": [11, 30], "z": 1, "stack": ["relic_chest"]})
for p in ((10, 30), (12, 30), (10, 31), (11, 31), (12, 31)):
    items.append({"at": [p[0], p[1]], "z": 1, "stack": ["rock"]})
house(42, 23, 8, 7, 2, 2, False, door_at=3, wall="stone_wall", roof="house_slate")
house(52, 24, 8, 7, 2, 2, True, door_at=4, wall="stone_wall", roof="house_slate")
# The middle terrace: Hollis's, the smithy, the better cottages — between the market's front behind
# them and their street, each at its own depth.
behind1 = lambda c: at(MARKET, c)
before1 = lambda c: first_row(STREET1, c)
placed(6, 8, 7, 1, behind1, before1, 0, 1, floors=2, basement=True, door_at=4)          # Hollis's
placed(15, 5, 5, 1, behind1, before1, 2, 1, dealt=True)
placed(26, 5, 5, 1, behind1, before1, 0, 1, floors=2, dealt=True)   # two floors against the market's front: its upper floor opens on the market
placed(37, 5, 5, 1, lambda c: at(MARKET, c) + 2, before1, 1, 1, dealt=True)                      # clear of the road's slope
placed(46, 8, 6, 1, behind1, before1, 0, 1, door_at=3, wall="stone_wall")              # the smithy
placed(54, 6, 5, 1, behind1, before1, 0, 1, floors=2, dealt=True)   # the same
# The foot: the miners' rows, Thornfield's farmhouse and barn, the cemetery's fence and trees.
# The miners' rows face the lane along the bank, so they stand where the bank lets them — set back
# where the stream swings south, pressed against the street where it swings north — and each a
# step further up or down the slope than its neighbour.
behind0 = lambda c: row_of(STREET0, c) + 1
before0 = lambda c: first_row(LANE, c)
for x, w, h, back in ((1, 7, 5, 1), (9, 6, 4, 0), (19, 5, 5, 0), (25, 4, 4, 2), (33, 5, 5, 0), (39, 5, 4, 0), (45, 5, 5, 0)):
    placed(x, w, h, 0, behind0, before0, back, 0, floors=2 if x == 1 else 1, front=True, dealt=True)   # the widest of the row has two floors
ground(53, 48, 9, 1, 0, "garden_fence"); ground(53, 53, 9, 1, 0, "garden_fence")
ground(53, 48, 1, 6, 0, "garden_fence"); ground(61, 48, 1, 6, 0, "garden_fence")
ground(57, 53, 1, 1, 0, "grass")
for p in ((55, 50), (59, 50), (56, 52)): ground(p[0], p[1], 1, 1, 0, "oak")
keep(53, 48, 0, 9, 7, 0)
# Thornfield (@ST-Farm), the Dunns' farm at the village's edge, east of the cemetery on the town's
# side of the stream (owner): the farmhouse, lived in; the barn, of red planks, where the hero
# sleeps (@ST-Hero), its bed against the back wall; their fields of tilled soil to the east, a path
# between them down to the lane.
FARMHOUSE = placed(64, 6, 5, 0, behind0, before0, 1, 0, front=True)
BARN = placed(72, 7, 5, 0, behind0, before0, 0, 0, front=True, wall="barn_wall", door="barn_door")
items.append({"at": [73, BARN + 1], "z": 0, "stack": ["bed"]})
for fx in (80, 84): ground(fx, 49, 3, 5, 0, "field")
trail([[83, 49], [83, first_row(LANE, 83)]], 0, "path")
# Who lives there, and the bargain (@ST-Farm, @ST-Hero; BUILD.md C9, as the demo's farm has it): the
# hero rides in on his horse (the dock, where he starts) and can lead it here; Dunn lends him the
# barn for it, and the barn's door stays shut till then; Lark, the Dunns' youngest
# (@ST-YoungVillager), by the fields.  Lark's lines are drafts, the owner's to rewrite.
DUNN = [70, 53, 0]
LARK = [79, 52, 0]
HORSE = [31, 60, 0]
THORNFIELD = [["farmer"] + DUNN, ["youngster"] + LARK, ["horse"] + HORSE]
GATES = [[72 + 7 // 2, BARN + 4, 0, "bargain"]]   # the barn's door
THORNFIELD_TALKS = [
  {"at": HORSE, "name": "horse", "lines": [{"horse": "NEIGH.", "end": True, "follows": True}]},
  {"at": DUNN, "name": "dunn", "barks": ["TOO MANY MOUTHS, TOO LITTLE LAND."], "lines": [
    {"dunn": "A FINE HORSE. LEND IT TO THE FARM, AND THE BARN IS YOURS TO SLEEP IN.", "away": "no", "bump": "deal"},
    {"id": "deal", "dunn": "DONE. THE HORSE STAYS WITH US.", "then": "after", "end": True, "sets": "bargain", "keeps": "horse"},
    {"id": "no", "dunn": "THINK IT OVER.", "end": True},
    {"id": "after", "dunn": "THE HORSE IS DOING FINE.", "end": True}]},
  {"at": LARK, "name": "lark", "barks": ["THE HILLS HAVE PATHS NOBODY KNOWS."], "lines": [
    "I AM LARK. THE DUNNS ARE MY FAMILY.",
    {"hero": "YOU KNOW THE HILLS?"},
    {"lark": "PATHS NOBODY ELSE KNOWS. ASK ME ONE DAY.", "end": True}]}]


for it in items: keep(it["at"][0], it["at"][1], it["z"], 1, 1, 1)
keep(8, 22, 2, 13, 8, 1)   # the inn

# Foliage, where nothing else stands: clumps rather than a sprinkle (a coarse noise over 5x5 blocks,
# a fine one per cell), thick on the hilltop and along the banks, thin in town; on the terraces'
# front rows bushes, so the long edge of a terrace is broken where nobody walks.
def noise(x, y, k):
    h = (x * 73856093) ^ (y * 19349663) ^ (k * 83492791)
    h = (h ^ (h >> 13)) * 1274126177
    return ((h ^ (h >> 16)) & 0xffff) / 65536.0
def plant(x, y, z, density, kinds):
    if (x, y, z) in clear: return
    clump = noise(x // 5, y // 5, z + 7)
    if noise(x, y, z) < density * (0.3 + 1.4 * clump):
        ground(x, y, 1, 1, z, kinds[int(noise(x, y, z + 3) * len(kinds)) % len(kinds)])
TOWN = ["oak", "bush", "bush", "autumn_oak", "poplar"]
for x in range(W):
    for y in range(0, 68):
        if y < at(HILL, x): plant(x, y, 4, 0.45, ["pine", "pine", "poplar", "oak", "autumn_poplar"])
        elif y < at(TOP, x): plant(x, y, 3, 0.10 if y < at(TOP, x) - 1 else 0.35, TOWN)
        elif y < at(MARKET, x): plant(x, y, 2, 0.06 if y < at(MARKET, x) - 1 else 0.35, TOWN)
        elif y < at(MIDDLE, x): plant(x, y, 1, 0.07 if y < at(MIDDLE, x) - 1 else 0.35, TOWN)
        elif y < north(x) - 1: plant(x, y, 0, 0.08, TOWN)
        elif y > north(x) + 3: plant(x, y, 0, 0.16 if y < north(x) + 7 else 0.3, ["oak", "pine", "bush", "autumn_oak"])
# The terraces' fronts slope: the hero walks up and down them anywhere (owner, 2026-11-09).  Cliffs
# only round the big house on the top terrace: its front row over the market and the hill behind it.
cliffs = [{"at": [20, 8], "size": [20, 14]}]
# The founder's ghost in the cemetery, by night (STORY.md @ST-Founder), and his quest (@ST-Urn):
# the urn given to find Sibil's house by its cellar; brought back, kept a while; at rest in her
# cellar, where he says where his good pick lies.  The lines are drafts, the owner's to rewrite.
GHOST = ["ghost", 57, 50, 0]
GHOST_TALK = {"at": [57, 50, 0], "name": "ghost",
  "barks": [{"ghost": "NO REST HERE.", "when": "!haunting"}, {"ghost": "SO COLD, THIS GROUND.", "when": "!haunting"},
            {"ghost": "MY URN... YOU LEFT MY URN.", "when": "haunting"}, {"ghost": "BRING IT HOME, OR TO HER.", "when": "haunting"}],
  "gifts": {"urn": "back"},
  "facts": {"pick_told": "rested", "ghost_at_rest": "rest"},
  "lines": [
    "THIS GRAVE GIVES ME NO REST.",
    {"hero": "WHO ARE YOU?"},
    {"ghost": "I DUG THE FIRST SHAFT. THE LAKE TOOK ME."},
    {"ghost": "MY LOVE, SIBIL, LIVED IN THE TOWN."},
    {"ghost": "WHICH HOUSE? TIME TOOK THAT FROM ME."},
    {"id": "ask", "ghost": "TAKE MY URN TO THE CELLARS. IN HERS I WILL KNOW.", "give": "urn", "sets": "urn_given", "then": "seek", "end": True},
    {"id": "seek", "ghost": "IN HER CELLAR I WILL KNOW. SEEK ON.", "end": True},
    {"id": "back", "ghost": "BACK WITH ME A WHILE. ASK, AND IT IS YOURS AGAIN.", "take": True, "then": "ask", "end": True},
    {"id": "rest", "ghost": "SIBIL... THIS WAS HER HOUSE.", "take": True},
    {"ghost": "HERE I CAN REST. THANK YOU."},
    {"ghost": "MY GOOD PICK, I LEFT IT IN THE MINE.", "sets": "pick_told", "end": True},
    {"id": "rested", "ghost": "AT REST, AT LAST.", "end": True}]}
intent = {"seed": 1, "rooms": rooms, "corridors": [], "edges": edges, "grounds": grounds, "trails": trails, "blobs": blobs,
          "houses": houses, "slopes": slopes, "items": items, "fronts": "slope", "cliffs": cliffs}
level = {"name": "Orbury", "camera": "follow", "legend": {}, "ground": [], "chunks": [], "things": [],
         "tiles": [], "edges": [], "rises": [], "intent": intent, "adjusted": [],
         "serves": ["@ST-Setting"], "start": [30, 60, 0], "goal": {"type": ""},
         "bars": [[14, 29, 2, "N"]], "enemies": [["grey_rat", 17, 25, 1], GHOST] + THORNFIELD,
         "talks": [GHOST_TALK] + THORNFIELD_TALKS, "gates": GATES}
json.dump(level, open(sys.argv[1], "w"), indent=1)
print("farmhouse at y", FARMHOUSE, "barn at y", BARN, file=sys.stderr)
