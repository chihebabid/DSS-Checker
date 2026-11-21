#!/usr/bin/env python3
"""
Compares the running times of DSS-Checker (run_mc.sh) and LTSmin (verif_all.sh)
from the outputs of these scripts (this script does not run any verification).

Usage:
  ./compare_times.py --case LABEL RUN_MC_OUTPUT VERIF_ALL_OUTPUT [--case ...] [-o courbes]

  RUN_MC_OUTPUT    : either
                     - the table printed by run_mc.sh, saved to a file
                       (e.g. ./run_mc.sh M.pn P.ltl | tee dss_A2_m1.txt)
                     - or the full log mc_<model>_<props>_<date>.log written by run_mc.sh
  VERIF_ALL_OUTPUT : resume.txt written by verif_all.sh (or its folder resultats/<...>/),
                     or the output of verif_all.sh saved with tee.

Example:
  ./compare_times.py \
      --case A2-m1 dss_A2_m1.txt resultats/SharedMemory5_A2_p_m1_onthefly/resume.txt \
      --case A3-m1 mc_SharedMemory5_A3_3M_sharedmemory5_3M_m1_20261117_180000.log \
                   resultats/SharedMemory5_A3_p_m1_onthefly \
      -o courbes

Outputs (in the -o folder, default: courbes):
  - <case>_par_propriete.png: checking time of each property for both tools,
    log scale. Green = property verified, red = violated; black circle = the two
    tools give different verdicts.
  - <case>_cumul.png: cumulative time (DSS construction included; LTSmin state
    space construction included in "full" (complet) mode).
  - comparaison_cas.png (at least two cases): total time per case.
  - resultats.csv: all measurements.
  - state_space.csv: size of the DSS (meta-states, SCCs, sync arcs, total and stored
    states) next to the size of the LTSmin state space, one row per case. The same
    table is printed. LTSmin gives the full state space only in "full" (complet) mode;
    in "onthefly" mode, the largest explored product is reported instead.

The i-th DSS-Checker property (#i-1) is paired with LTSmin prop<i>
(ltl2action.sh numbers the non-empty lines of the .ltl file in order).
"""
import argparse
import csv
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent

# Colours: green/red status (verdict), neutral ink for everything else.
GREEN = "#0ca30c"      # property verified
RED = "#d03b3b"        # property violated
GRAY = "#8a8984"       # unknown verdict / error
INK = "#0b0b0b"
INK_2 = "#52514e"
GRID = "#e1e0d9"
AXIS = "#c3c2b7"
SURFACE = "#fcfcfb"

# Tool identity: marker shape + line style (colour carries the verdict).
TOOLS = {
    "dss": dict(label="DSS-Checker", marker="o", linestyle="-", line=INK_2),
    "ltsmin": dict(label="LTSmin", marker="s", linestyle="--", line="#9a9890"),
}


@dataclass
class Prop:
    index: int                      # 1-based
    formula: str = ""
    module: str = ""
    dss_time: float | None = None   # s
    dss_ok: bool | None = None      # True = verified, False = violated, None = unknown
    lts_time: float | None = None
    lts_ok: bool | None = None
    lts_states: str = ""
    lts_arcs: str = ""
    lts_mem: str = ""


# DSS statistics (Total row of the "DSS statistics" table printed by distributedstatespace)
DSS_STATS = ["metastates", "sccs", "sync_arcs", "local_arcs", "total_states", "stored_states", "local_graphs"]

# LTSmin --state=table stores at most 2^24 states: a count at this value is a lower bound
LTSMIN_TABLE_CAP = 2 ** 24


@dataclass
class Case:
    label: str
    props: list[Prop] = field(default_factory=list)
    dss_build: float | None = None  # s
    lts_build: float | None = None  # s (full mode only)
    dss_stats: dict | None = None   # see DSS_STATS
    lts_states: str = ""            # full state space (full mode only)
    lts_arcs: str = ""


# ----------------------------------------------------------------- parsing

def parse_dss(path):
    """run_mc.sh output: printed summary table, or full distributedstatespace log.
    Returns (properties, construction time in s, DSS statistics or None)."""
    text = Path(path).read_text(errors="replace")
    stats = parse_dss_stats(text)
    if "***** Property#" in text or re.search(r"^Property #\d+", text, re.M):
        return (*parse_dss_full_log(text), stats)
    return (*parse_dss_summary(text), stats)


def parse_dss_stats(text):
    """Total row of the "DSS statistics" table (full log or run_mc.sh output):
         Total |  637 |  2562 |  6379 |  20614 |  9580 |  1353 |  143"""
    m = re.search(r"^\s*Total\s*\|(.*)$", text, re.M)
    if not m:
        return None
    values = [c.strip() for c in m.group(1).split("|")]
    if len(values) != len(DSS_STATS) or not all(v.isdigit() for v in values):
        return None
    return dict(zip(DSS_STATS, map(int, values)))


def parse_dss_full_log(text):
    """Full distributedstatespace log, current format (Property #i, Result: ...) or the
    former one (***** Property#i, Property is verified ...)."""
    props, build, cur = [], None, None
    for line in text.splitlines():
        if m := re.search(r"(?:DSS building duration:|^DSS built in)\s*([\d.]+)\s*ms", line):
            build = float(m.group(1)) / 1000.0
        elif m := re.search(r"^(?:\*\*\*\*\* Property#|Property #)(\d+)", line):
            cur = Prop(index=int(m.group(1)) + 1)
            props.append(cur)
        elif cur is None:
            continue
        elif m := re.search(r"(?:Checking formula:|^\s+Formula\s*:)\s*(.*)", line):
            cur.formula = m.group(1).strip()
        elif m := re.search(r"(?:checked against module #|^\s+Module\s*:\s*)(\d+)", line):
            cur.module = m.group(1)
        elif "Property is verified" in line or re.search(r"^\s+Result\s*:\s*VERIFIED", line):
            cur.dss_ok = True
        elif "Property is violated" in line or re.search(r"^\s+Result\s*:\s*VIOLATED", line):
            cur.dss_ok = False
        elif m := re.search(r"(?:Verification duration:|^\s+Verification time\s*:)\s*([\d.eE+-]+)", line):
            cur.dss_time = float(m.group(1))
    return props, build


def parse_dss_summary(text):
    """Table printed by run_mc.sh (English labels, or French ones for former outputs):
         DSS construction : 684 ms                (Construction du DSS : 684 ms)
         #     Module  Result     Time (s)       Formula
         0     0       VERIFIED   0.000223       !End_Own_Acc_5_5 W ...
    (the formula is truncated to 60 characters)."""
    props, build = [], None
    verdicts = {"VERIFIED": True, "VIOLATED": False, "VERIFIEE": True, "VIOLEE": False}
    for line in text.splitlines():
        if m := re.search(r"(?:Construction du DSS|DSS construction)\s*:\s*([\d.]+)\s*ms", line):
            build = float(m.group(1)) / 1000.0
            continue
        m = re.match(r"\s*(\d+)\s+(\S+)\s+(VERIFIED|VIOLATED|SKIPPED|VERIFIEE|VIOLEE|\?\?)\s+(\S+)\s*(.*)$", line)
        if not m:
            continue
        p = Prop(index=int(m.group(1)) + 1, module=m.group(2), formula=m.group(5).strip())
        p.dss_ok = verdicts.get(m.group(3))
        try:
            p.dss_time = float(m.group(4))
        except ValueError:
            p.dss_time = None
        props.append(p)
    if not props:
        sys.exit(f"No property found in the DSS-Checker output ({len(text)} characters read)")
    return props, build


def parse_ltsmin_resume(path):
    """resume.txt written by verif_all.sh."""
    path = Path(path)
    if path.is_dir():
        path = path / "resume.txt"
    rows, build, full = {}, None, ("", "")
    for line in path.read_text(errors="replace").splitlines():
        if m := re.search(r"(?:Construction complète|Full construction)\s*:.*?,\s*([\d.]+)\s*s", line):
            build = float(m.group(1))
            if n := re.search(r":\s*(\S+)\s+(?:états|states),\s*(\S+)\s+arcs", line):
                full = (n.group(1), n.group(2))
            continue
        cols = [c.strip() for c in line.split("|")]
        if len(cols) < 6 or not (m := re.fullmatch(r"prop(\d+)", cols[0])):
            continue
        res = cols[5].upper()
        ok = None if res.startswith(("ERREUR", "ERROR")) else (not res.startswith(("NON", "NOT")))
        try:
            t = float(cols[1])
        except ValueError:
            t = None
        rows[int(m.group(1))] = dict(time=t, ok=ok, states=cols[2], arcs=cols[3], mem=cols[4])
    if not rows:
        sys.exit(f"No property found in the LTSmin output {path}")
    return rows, build, full


def build_case(label, dss_log, lts_resume):
    props, dss_build, dss_stats = parse_dss(dss_log)
    rows, lts_build, (lts_states, lts_arcs) = parse_ltsmin_resume(lts_resume)
    by_idx = {p.index: p for p in props}
    for i, r in rows.items():
        p = by_idx.setdefault(i, Prop(index=i))
        p.lts_time, p.lts_ok = r["time"], r["ok"]
        p.lts_states, p.lts_arcs, p.lts_mem = r["states"], r["arcs"], r["mem"]
    case = Case(label, sorted(by_idx.values(), key=lambda p: p.index), dss_build, lts_build,
                dss_stats, lts_states, lts_arcs)
    if len(props) != len(rows):
        print(f"[{label}] warning: {len(props)} DSS-Checker properties, {len(rows)} LTSmin", file=sys.stderr)
    return case


# ----------------------------------------------------------------- plotting

def style_axes(ax):
    ax.set_facecolor(SURFACE)
    for s in ("top", "right"):
        ax.spines[s].set_visible(False)
    for s in ("left", "bottom"):
        ax.spines[s].set_color(AXIS)
    ax.tick_params(colors=INK_2, labelsize=9)
    ax.yaxis.grid(True, color=GRID, linewidth=0.8)
    ax.xaxis.grid(False)
    ax.set_axisbelow(True)


def verdict_color(ok):
    return GREEN if ok is True else RED if ok is False else GRAY


def legend_handles(plt, with_mismatch):
    from matplotlib.lines import Line2D
    h = [Line2D([], [], color=t["line"], linestyle=t["linestyle"], marker=t["marker"],
                markerfacecolor="white", markeredgecolor=INK_2, markersize=8, linewidth=2,
                label=t["label"]) for t in TOOLS.values()]
    h += [Line2D([], [], linestyle="", marker="o", markersize=8, markerfacecolor=GREEN,
                 markeredgecolor="white", label="Property verified"),
          Line2D([], [], linestyle="", marker="o", markersize=8, markerfacecolor=RED,
                 markeredgecolor="white", label="Property violated")]
    if with_mismatch:
        h.append(Line2D([], [], linestyle="", marker="o", markersize=13, markerfacecolor="none",
                        markeredgecolor=INK, markeredgewidth=1.5, label="Different verdicts"))
    return h


def plot_series(ax, xs, ys, oks, tool):
    t = TOOLS[tool]
    pts = [(x, y, ok) for x, y, ok in zip(xs, ys, oks) if y is not None]
    if not pts:
        return
    px, py, pok = zip(*pts)
    ax.plot(px, py, color=t["line"], linestyle=t["linestyle"], linewidth=2, zorder=2)
    ax.scatter(px, py, marker=t["marker"], s=64, c=[verdict_color(o) for o in pok],
               edgecolors="white", linewidths=1.5, zorder=3)   # light ring where points overlap


def floor_times(values):
    """Log scale: replaces 0 by a visible floor."""
    pos = [v for v in values if v]
    floor = (min(pos) / 2) if pos else 1e-6
    return [None if v is None else max(v, floor) for v in values]


def plot_case(plt, case, outdir):
    xs = [p.index for p in case.props]
    dss_t = floor_times([p.dss_time for p in case.props])
    lts_t = floor_times([p.lts_time for p in case.props])
    mism = [p for p in case.props if p.dss_ok is not None and p.lts_ok is not None and p.dss_ok != p.lts_ok]

    # --- time per property
    fig, ax = plt.subplots(figsize=(max(8, 0.45 * len(xs) + 3), 5), facecolor=SURFACE)
    style_axes(ax)
    plot_series(ax, xs, dss_t, [p.dss_ok for p in case.props], "dss")
    plot_series(ax, xs, lts_t, [p.lts_ok for p in case.props], "ltsmin")
    for p in mism:
        for y in (dss_t[xs.index(p.index)], lts_t[xs.index(p.index)]):
            if y is not None:
                ax.scatter([p.index], [y], s=230, facecolors="none", edgecolors=INK, linewidths=1.5, zorder=4)
    ax.set_yscale("log")
    ax.set_xticks(xs)
    ax.set_xlabel("Property", color=INK_2)
    ax.set_ylabel("Checking time (s, log scale)", color=INK_2)
    title = f"{display(case.label)}: checking time per property"
    if case.dss_build is not None:
        title += f"\nDSS construction: {case.dss_build:.3f} s (not included)"
    ax.set_title(title, color=INK, fontsize=12, loc="left")
    ax.legend(handles=legend_handles(plt, bool(mism)), frameon=False, fontsize=9,
              loc="upper left", bbox_to_anchor=(1.01, 1))
    fig.tight_layout()
    f1 = outdir / f"{safe(case.label)}_par_propriete.png"
    fig.savefig(f1, dpi=150)
    plt.close(fig)

    # --- cumulative time
    def cumul(start, vals):
        acc, out = start or 0.0, []
        for v in vals:
            acc += v or 0.0
            out.append(acc)
        return out

    fig, ax = plt.subplots(figsize=(max(8, 0.45 * len(xs) + 3), 5), facecolor=SURFACE)
    style_axes(ax)
    plot_series(ax, xs, cumul(case.dss_build, [p.dss_time for p in case.props]),
                [p.dss_ok for p in case.props], "dss")
    plot_series(ax, xs, cumul(case.lts_build, [p.lts_time for p in case.props]),
                [p.lts_ok for p in case.props], "ltsmin")
    ax.set_xticks(xs)
    ax.set_xlabel("Number of properties checked", color=INK_2)
    ax.set_ylabel("Cumulative time (s)", color=INK_2)
    note = "DSS-Checker: DSS construction included"
    if case.lts_build is not None:
        note += "; LTSmin: state space construction included"
    ax.set_title(f"{display(case.label)}: cumulative time\n{note}", color=INK, fontsize=12, loc="left")
    ax.legend(handles=legend_handles(plt, False), frameon=False, fontsize=9,
              loc="upper left", bbox_to_anchor=(1.01, 1))
    fig.tight_layout()
    f2 = outdir / f"{safe(case.label)}_cumul.png"
    fig.savefig(f2, dpi=150)
    plt.close(fig)
    return f1, f2, mism


def plot_cases(plt, cases, outdir):
    labels = [display(c.label) for c in cases]
    xs = list(range(len(cases)))
    dss = [(c.dss_build or 0) + sum(p.dss_time or 0 for p in c.props) for c in cases]
    lts = [(c.lts_build or 0) + sum(p.lts_time or 0 for p in c.props) for c in cases]
    fig, ax = plt.subplots(figsize=(max(7, 1.2 * len(cases) + 3), 5), facecolor=SURFACE)
    style_axes(ax)
    for tool, ys in (("dss", dss), ("ltsmin", lts)):
        t = TOOLS[tool]
        ax.plot(xs, ys, color=t["line"], linestyle=t["linestyle"], marker=t["marker"], markersize=8,
                markerfacecolor="white", markeredgecolor=INK_2, linewidth=2, label=t["label"])
    ax.set_yscale("log")
    ax.set_xticks(xs, labels, rotation=30 if len(cases) > 5 else 0, ha="right" if len(cases) > 5 else "center")
    ax.set_ylabel("Total time (s, log scale)", color=INK_2)
    ax.set_title("Total time per case (construction + all properties)", color=INK, fontsize=12, loc="left")
    ax.legend(frameon=False, fontsize=9, loc="upper left", bbox_to_anchor=(1.01, 1))
    fig.tight_layout()
    f = outdir / "comparaison_cas.png"
    fig.savefig(f, dpi=150)
    plt.close(fig)
    return f


def display(label):
    """Case label as shown on figures: French LTSmin mode names in English."""
    return re.sub(r"\b(volee|volée)\b", "onthefly", re.sub(r"\bcomplet\b", "full", label))


def safe(s):
    return re.sub(r"[^\w.-]+", "_", s)


def write_csv(cases, path):
    with open(path, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["cas", "propriete", "module", "formule", "dss_temps_s", "dss_verdict",
                    "ltsmin_temps_s", "ltsmin_verdict", "ltsmin_etats", "ltsmin_arcs",
                    "ltsmin_mem_mo", "dss_construction_s", "ltsmin_construction_s"]
                   + [f"dss_{k}" for k in DSS_STATS] + ["ltsmin_full_states", "ltsmin_full_arcs"])
        v = lambda ok: "" if ok is None else ("verifiee" if ok else "violee")
        for c in cases:
            stats = [(c.dss_stats or {}).get(k, "") for k in DSS_STATS]
            for p in c.props:
                w.writerow([c.label, p.index, p.module, p.formula, p.dss_time, v(p.dss_ok),
                            p.lts_time, v(p.lts_ok), p.lts_states, p.lts_arcs, p.lts_mem,
                            c.dss_build, c.lts_build] + stats + [c.lts_states, c.lts_arcs])


# ----------------------------------------------------------------- state space size

def lts_size(c):
    """LTSmin state space size of a case: (states, arcs, kind). Full mode: the full state space;
    onthefly mode: the largest product explored by one property."""
    if c.lts_states:
        return c.lts_states, c.lts_arcs, "full"
    explored = [(int(p.lts_states), p.lts_arcs) for p in c.props if p.lts_states.isdigit()]
    if explored:
        s, a = max(explored)
        return str(s), a, "max explored"
    return "", "", ""


def lower_bound(n):
    """'>= n' when n reaches the LTSmin table capacity (2^24 states): the count is truncated."""
    return f">= {n}" if n.isdigit() and int(n) >= LTSMIN_TABLE_CAP else n


def ratio(a, b):
    return f"{a / b:.3g}" if a and b else ""


def write_state_space(cases, path):
    """Prints and saves the size of the DSS next to the size of the LTSmin state space."""
    head = ["case", "dss_metastates", "dss_sccs", "dss_sync_arcs", "dss_local_arcs", "dss_total_states",
            "dss_stored_states", "ltsmin_states", "ltsmin_arcs", "ltsmin_kind", "ltsmin_states/dss_stored_states"]
    rows = []
    for c in cases:
        st = c.dss_stats or {}
        ls, la, kind = lts_size(c)
        r = ratio(int(ls), st.get("stored_states")) if ls.isdigit() else ""
        rows.append([c.label, st.get("metastates", ""), st.get("sccs", ""), st.get("sync_arcs", ""),
                     st.get("local_arcs", ""), st.get("total_states", ""), st.get("stored_states", ""),
                     lower_bound(ls), lower_bound(la) if la else la, kind, r])
    with open(path, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(head)
        w.writerows(rows)

    cols = ["Case", "Meta-states", "SCCs", "Sync arcs", "Total states", "Stored states",
            "LTSmin states", "LTSmin arcs", "LTSmin kind", "Ratio"]
    table = [[r[0], r[1], r[2], r[3], r[5], r[6], r[7], r[8], r[9], r[10]] for r in rows]
    widths = [max(len(str(x)) for x in col) for col in zip(cols, *table)]
    fmt = lambda xs: "  " + " | ".join(str(x).ljust(w) if i == 0 else str(x).rjust(w)
                                       for i, (x, w) in enumerate(zip(xs, widths)))
    print("\nState space size: DSS-Checker vs LTSmin")
    print(fmt(cols))
    print("  " + "-+-".join("-" * w for w in widths))
    for t in table:
        print(fmt(t))
    print("  Total states : markings summed over the meta-states; Stored states: markings kept in memory")
    print("  LTSmin kind  : full = full state space (complet mode); max explored = largest product explored")
    print("                 by one property (onthefly mode). '>=': truncated at the 2^24 table capacity")
    print("  Ratio        : LTSmin states / DSS stored states")


# ----------------------------------------------------------------- main

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--case", nargs=3, action="append", required=True,
                    metavar=("LABEL", "RUN_MC_OUTPUT", "VERIF_ALL_OUTPUT"),
                    help="a case to compare (repeat the option for several cases)")
    ap.add_argument("-o", "--outdir", default="courbes", help="output folder (default: courbes)")
    args = ap.parse_args()

    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        sys.exit("matplotlib missing: sudo apt install python3-matplotlib  (or pip install matplotlib)")

    for label, dss_out, lts_out in args.case:
        for f in (dss_out, lts_out):
            if not Path(f).exists():
                sys.exit(f"[{label}] file not found: {f}")

    outdir = Path(args.outdir).resolve()
    outdir.mkdir(parents=True, exist_ok=True)
    cases = [build_case(*c) for c in args.case]

    for c in cases:
        f1, f2, mism = plot_case(plt, c, outdir)
        ok_d = sum(p.dss_ok is True for p in c.props)
        ko_d = sum(p.dss_ok is False for p in c.props)
        ok_l = sum(p.lts_ok is True for p in c.props)
        ko_l = sum(p.lts_ok is False for p in c.props)
        print(f"[{c.label}] DSS-Checker {ok_d} V / {ko_d} X  |  LTSmin {ok_l} V / {ko_l} X  |  "
              f"disagreements: {[p.index for p in mism] or 'none'}")
        print(f"   {f1}\n   {f2}")
    if len(cases) >= 2:
        print(f"   {plot_cases(plt, cases, outdir)}")
    write_csv(cases, outdir / "resultats.csv")
    print(f"   {outdir / 'resultats.csv'}")
    write_state_space(cases, outdir / "state_space.csv")
    print(f"   {outdir / 'state_space.csv'}")


if __name__ == "__main__":
    main()
