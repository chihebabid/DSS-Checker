#!/usr/bin/env python3
"""
Makes state-based LTL formulas ("place op number") checkable on an ETF file.

In the ETF written by pnml2lts-sym, the places are enum state variables without a
value table: LTSmin rejects "place <= 3" (enum vs integer) and compares "place == "3""
as strings, which is meaningless. This script adds to the ETF one boolean state label
(sort "guard", values "false"/"true") per comparison found in the formulas, and rewrites
each comparison into (label == "true").

Usage: ./etf_state_labels.py IN.etf OUT.etf PROPS_DIR OUT_PROPS_DIR
  Every prop*.ltl of PROPS_DIR is rewritten into OUT_PROPS_DIR (same file names).
  Exit status 0 if at least one comparison was found, 2 otherwise (nothing written).
"""
import operator
import re
import sys
from pathlib import Path

OPS = {"==": operator.eq, "!=": operator.ne, "<=": operator.le, ">=": operator.ge,
       "<": operator.lt, ">": operator.gt}
NAMES = {"==": "eq", "!=": "ne", "<=": "le", ">=": "ge", "<": "lt", ">": "gt"}
# "place op number" outside double quotes (action == "t" never matches: its RHS is quoted)
COMPARISON = re.compile(r'("[^"]*")|\b([A-Za-z_][A-Za-z0-9_]*)\s*(==|!=|<=|>=|<|>)\s*([0-9]+)\b')


def read_etf(path):
    lines = path.read_text().splitlines()
    start = lines.index("begin state")
    slots = [v.split(":")[0] for v in lines[start + 1].split()]
    return lines, slots


def slot_values(lines, nslots):
    """Values taken by each state variable (init and transitions)."""
    values = [set() for _ in range(nslots)]
    section = None
    for line in lines:
        if line.startswith("begin "):
            section = line.split()[1]
            continue
        if line.startswith("end "):
            section = None
            continue
        if section == "init":
            for i, v in enumerate(line.split()):
                values[i].add(int(v))
        elif section == "trans":
            for i, cell in enumerate(line.split()[:nslots]):
                if cell != "*":
                    for v in cell.split("/"):
                        values[i].add(int(v))
    return values


def main():
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    etf_in, etf_out, props_in, props_out = map(Path, sys.argv[1:])
    lines, slots = read_etf(etf_in)
    if "guard" not in {l.split()[2] for l in lines if l.startswith("begin sort")}:
        sys.exit("No 'guard' sort in the ETF file")
    index = {name: i for i, name in enumerate(slots)}
    values = slot_values(lines, len(slots))

    labels = {}  # (place, op, n) -> label name

    def rewrite(match):
        quoted, place, op, n = match.groups()
        if quoted is not None:
            return quoted
        if place not in index:
            sys.exit(f"Unknown place in a formula: {place}")
        key = (place, op, int(n))
        labels.setdefault(key, f"sp_{place}_{NAMES[op]}_{n}")
        return f'({labels[key]} == "true")'

    rewritten = {}
    for f in sorted(props_in.glob("prop*.ltl")):
        rewritten[f.name] = COMPARISON.sub(rewrite, f.read_text())
    if not labels:
        return 2

    maps = []
    for (place, op, n), name in labels.items():
        slot = index[place]
        maps.append(f"begin map {name}:guard")
        for v in sorted(values[slot]):
            cells = ["*"] * len(slots)
            cells[slot] = str(v)
            maps.append(" ".join(cells) + " " + ("1" if OPS[op](v, n) else "0"))
        maps.append("end map")
    first_sort = next(i for i, l in enumerate(lines) if l.startswith("begin sort"))
    etf_out.write_text("\n".join(lines[:first_sort] + maps + lines[first_sort:]) + "\n")

    props_out.mkdir(parents=True, exist_ok=True)
    for name, text in rewritten.items():
        (props_out / name).write_text(text)
    print(f"{len(labels)} state label(s) added to {etf_out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
