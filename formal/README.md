<!--
Copyright (c) 2026 Jurjen Stellingwerff
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Formal rules

The game's named rules, one document per domain.  A rule is defined by a line `  (Name)  prose`
in a fenced block, and the code that enforces it cites it as `@FR-<Name>` in a comment, so "which
code keeps this rule?" is a search, and a renamed or removed rule cannot leave a citation behind.
The rules do not change to match the code; the code changes to match the rules.

- [hex-houses.md](hex-houses.md) — structures on the hex grid at any 30° angle: the cells, the
  walls drawn from them, the turn.

Check that every citation resolves and no rule is defined twice (`scripts/rule_tags.py` is loft's,
unchanged):

```sh
RULES_DIR=formal CITE_DIRS=.:tests:tools CITE_EXTS=.loft python3 scripts/rule_tags.py check
RULES_DIR=formal CITE_DIRS=.:tests:tools CITE_EXTS=.loft python3 scripts/rule_tags.py sites @FR-HT-Turn30
```
