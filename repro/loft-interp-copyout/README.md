# A loft interpreter memory corruption, not yet reduced

Found 2026-10-30 while building rolling carts, on loft main @ 7e4917203 (dev build).

`rules.loft` here is the game's rules with the first version of `link` (rolling trains meeting),
which holds views of `l.rolls[ri]` / `l.rolls[oi]` (loft copies them out: its advice says so) while
it rewrites those elements' `cells` and removes elements.  With it in place of the game's
`rules.loft`, `tests/rolling.loft::test_carts_that_meet_link` on the interpreter answers wrong and
differently from run to run — carts on x 12 and x 13 (a wall cell), or "Store access out of bounds
… the reference is corrupt".  `--native` and `LOFT_STRICT_STORES=1` pass.  The game's `link` now
reads the trains into plain locals first and passes every run.

Reducing it to a library-free program has not reproduced it yet (probes in the session scratch:
the same struct shape, the copy-out, a copied-out caller — all correct).  Its sibling there,
`--native` E0425 on a loop over a parameter's element field, is loft#2265.

To reproduce: copy `rules.loft` over the game's, run
`loft --lib libs --tests tests/rolling.loft` a few times on the interpreter, then restore the
game's (`git checkout` is not needed: keep a copy first).
