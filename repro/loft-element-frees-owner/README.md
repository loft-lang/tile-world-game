# A vector element's field, unwrapped, frees the struct that holds the vector

**Fixed as loft#2404** (commit 6eb649238 on loft branch `tuxedo-pr2400-next`, not on `main` yet,
2026-11-04).  The cause is declaration ORDER, not the Level's shape: `Visited` is declared below
`Game`, the struct holding the vector; pass 1 typed the `!` discharge's temporary as an unknown
type, pass 2 kept that dep-less type, so the temporary read as an owner and its scope-end free
released the whole Game store.  Also hit `?? panic` / `?? return` on a vector as an assignment
source or a const argument.  Minimal repro (both backends, LOFT_STRICT_STORES=1):

```loft
struct Game { world: Lv, places: vector<Visited> }
struct Visited { place: text, level: Lv }
struct Lv { name: text }
fn go(g: Game, j: integer) { g.world = g.places[j]!.level; }
```

Until the fix reaches the loft the game is built with, keep the loop form below (or declare
`Visited` above `Game`).

Seen 2026-11-04 on loft `tuxedo-pln204-int32` @ 4b1cd77d4 (release build, interpreter), while
building doors between places (`game.loft` `go_through`).  Not yet reduced without the game's
libraries: two library-free reductions (a vector of structs holding a nested hash, the same
stash / switch / read sequence) do not reproduce it.

Inside `go_through(g: Game, i: integer)`, where `g.places: vector<Visited>` and
`Visited { place: text, level: rules::Level }`:

```loft
rules::carry_hero(g.places[from]!.level, g.world);   // (1) as a call argument
if j >= 0 { g.world = g.places[j]!.level; }          // (2) as the source of a field copy
```

Each one, alone, leaves the `Game` store freed: under `LOFT_STRICT_STORES=1` every later read of
`g` is `USE AFTER FREE (read) store #28 type=Game rec=1`; without it, a later step panics with
`Store access out of bounds ... the reference is corrupt`.  Reading the element through a loop
(`for v in g.places { if v.place == to { g.world = v.level; } }`) is clean, and so is passing
values instead of the element (`rules::carried` / `rules::carry_in`, what the game does now).

Reproduce: put (1) or (2) back in `go_through`, then

```sh
LOFT_STRICT_STORES=1 loft --lib libs checks/game.loft
```

`test_a_house_door_leads_in_and_back_out` reaches (1) on the way in and (2) on the way out.
