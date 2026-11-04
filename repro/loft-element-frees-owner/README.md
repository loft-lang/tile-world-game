# A vector element's field, unwrapped, frees the struct that holds the vector

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
