#!/usr/bin/env python3
"""
Compares the peak memory (RSS) of DSS-Checker and LTSmin, formula by formula.

Usage:
  ./compare_memory.py --case LABEL MEM_DIR LTSMIN_ONTHEFLY LTSMIN_COMPLET [--case ...] [-o DIR] [--no-merge]

  LABEL           : "<model>-m<k>" (e.g. SLB2C3sb-m1); the modules of a model are also merged
                    into "<model>-all" (formulas renumbered m1 1..20, m2 21..40, m3 41..60)
  MEM_DIR         : folder mem_<model>_<props>/ written by mem_mc.sh (DSS-Checker)
  LTSMIN_ONTHEFLY : folder resultats/<...>_onthefly/ written by verif_all.sh ("-" if none)
  LTSMIN_COMPLET  : folder resultats/<...>_complet/ written by verif_all.sh ("-" if none)

Every value is the peak RSS of one process (/usr/bin/time %M):
  - DSS-Checker: one process per formula, DSS construction included;
  - LTSmin onthefly: one pnml2lts-seq process per formula;
  - LTSmin full: one etf2lts-seq process per formula (state space read from the ETF file);
    the construction of the state space (pnml2lts-sym) is a separate process.
The construction peaks (DSS-Checker without formula, pnml2lts-sym) are drawn as dashed lines.

Outputs in the -o folder (default: courbes/memoire), PDF + PNG:
  memory-plots/<model>  : peak memory of each formula for every tool
  memoire.csv           : all the measurements
"""
import argparse
import csv
import re
import sys
from pathlib import Path

# Same tool colours as plot_results.py (tools in alphabetical order: DSS-Checker, LTSmin full, LTSmin onthefly)
TOOL_COLORS = {"DSS-Checker": "#1f77b4", "LTSmin full": "#ff7f0e", "LTSmin onthefly": "#2ca02c"}
VERDICT = {"verifiee": "T", "violee": "F"}


def read_mem(path):
    """Peak RSS in MB from a /usr/bin/time %M file (KB), None if missing."""
    try:
        return int(Path(path).read_text().split()[-1]) / 1024
    except (OSError, ValueError, IndexError):
        return None


def load_case(label, mem_dir, onthefly, complet):
    rows = list(csv.DictReader(open(Path(mem_dir) / "mem.csv")))
    case = {"label": label, "formulas": {}, "build": {}}
    for r in rows:
        i = int(r["propriete"])
        if i == 0:
            case["build"]["DSS-Checker"] = float(r["dss_mem_mo"])
            continue
        case["formulas"][i] = {"verdict": VERDICT.get(r["verdict"], "U"),
                               "mem": {"DSS-Checker": float(r["dss_mem_mo"]) if r["dss_mem_mo"] else None}}
    for tool, folder in (("LTSmin onthefly", onthefly), ("LTSmin full", complet)):
        if folder == "-":
            continue
        logs = Path(folder) / "logs"
        for i, f in case["formulas"].items():
            f["mem"][tool] = read_mem(logs / f"prop{i}.mem")
        if tool == "LTSmin full":
            case["build"]["LTSmin full"] = read_mem(logs / "construction.mem")
    return case


def merge(cases):
    """<model>-m<k> cases of a same model -> <model>-all, formulas renumbered in module order."""
    groups = {}
    for c in cases:
        if m := re.fullmatch(r"(.+)-m(\d+)", c["label"]):
            groups.setdefault(m.group(1), []).append((int(m.group(2)), c))
    merged = []
    for base, members in groups.items():
        if len(members) < 2:
            continue
        members.sort(key=lambda x: x[0])
        all_case = {"label": f"{base}-all", "formulas": {}, "build": {}, "modules": []}
        n = 0
        for k, c in members:
            first = n + 1
            for i in sorted(c["formulas"]):
                n += 1
                all_case["formulas"][n] = c["formulas"][i]
            all_case["modules"].append((k, first, n))
            for tool, v in c["build"].items():   # same model: keep the largest construction peak
                if v is not None:
                    all_case["build"][tool] = max(v, all_case["build"].get(tool, 0))
        merged.append(all_case)
    return merged


def plot(plt, case, outdir):
    from matplotlib.lines import Line2D
    formulas = case["formulas"]
    xs = sorted(formulas)
    tools = [t for t in TOOL_COLORS if any(formulas[x]["mem"].get(t) is not None for x in xs)]
    fig, ax = plt.subplots(figsize=(max(8, min(16, 0.22 * len(xs) + 4)), 5))
    for tool in tools:
        pts = [(x, formulas[x]["mem"][tool]) for x in xs if formulas[x]["mem"].get(tool) is not None]
        ax.scatter([p[0] for p in pts], [p[1] for p in pts], marker="x", color=TOOL_COLORS[tool],
                   label=tool, zorder=3)
    handles, labels = ax.get_legend_handles_labels()
    build_names = {"DSS-Checker": "DSS-Checker: construction only", "LTSmin full": "LTSmin full: construction (pnml2lts-sym)"}
    for tool, v in case["build"].items():
        if v is not None and tool in tools:
            ax.axhline(v, color=TOOL_COLORS[tool], linestyle="--", linewidth=1, zorder=2)
            handles.append(Line2D([], [], color=TOOL_COLORS[tool], linestyle="--", linewidth=1))
            labels.append(build_names[tool])
    for k, first, last in case.get("modules", []):
        if first > 1:
            ax.axvline(first - 0.5, color="gray", linestyle="--", linewidth=1)
        ax.text((first + last) / 2, 1.0, f"module {k}", transform=ax.get_xaxis_transform(),
                ha="center", va="top", fontsize=9, color="gray")
    ax.set_ylim(bottom=0, top=ax.get_ylim()[1] * 1.08)
    ax.set_xlim(xs[0] - 1, xs[-1] + 1)
    ticks = [1] + [first for _, first, _ in case.get("modules", []) if first > 1] + [xs[-1]] \
        if case.get("modules") else [x for x in xs if x == 1 or x % 5 == 0]
    ax.set_xticks(sorted(set(ticks)))
    ax.set_xlabel("formula")
    ax.set_ylabel("peak memory (MB)")
    ax.set_title(case["label"])
    ax.grid(axis="y", color="#e5e5e5", linewidth=0.8, zorder=0)
    ax.legend(handles, labels, title="tool", frameon=False, loc="upper left", bbox_to_anchor=(1.02, 1))
    path = outdir / "memory-plots" / case["label"]
    path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(path.with_suffix(".pdf"), bbox_inches="tight")
    fig.savefig(path.with_suffix(".png"), dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"   {path}.pdf / .png")


def summary(case):
    formulas = case["formulas"]
    parts = []
    for tool in TOOL_COLORS:
        vals = [f["mem"].get(tool) for f in formulas.values() if f["mem"].get(tool) is not None]
        if vals:
            parts.append(f"{tool}: {min(vals):.1f}-{max(vals):.1f} MB (mean {sum(vals) / len(vals):.1f})")
    builds = ", ".join(f"{t} {v:.1f} MB" for t, v in case["build"].items() if v is not None)
    print(f"[{case['label']}] {len(formulas)} formulas | " + " | ".join(parts) + f" | construction: {builds}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--case", nargs=4, action="append", required=True,
                    metavar=("LABEL", "MEM_DIR", "LTSMIN_ONTHEFLY", "LTSMIN_COMPLET"))
    ap.add_argument("-o", "--outdir", default="courbes/memoire")
    ap.add_argument("--no-merge", action="store_true", help="do not merge the modules of a model")
    args = ap.parse_args()

    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    cases = [load_case(*c) for c in args.case]
    if not args.no_merge:
        cases += merge(cases)
    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)
    with open(outdir / "memoire.csv", "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["cas", "propriete", "verdict", "dss_mem_mo", "ltsmin_onthefly_mem_mo", "ltsmin_complet_mem_mo",
                    "dss_construction_mem_mo", "ltsmin_construction_mem_mo"])
        for c in cases:
            for i, fm in sorted(c["formulas"].items()):
                m = fm["mem"]
                w.writerow([c["label"], i, fm["verdict"], m.get("DSS-Checker"), m.get("LTSmin onthefly"),
                            m.get("LTSmin full"), c["build"].get("DSS-Checker"), c["build"].get("LTSmin full")])
    for c in cases:
        summary(c)
        plot(plt, c, outdir)
    print(f"   {outdir / 'memoire.csv'}")


if __name__ == "__main__":
    sys.exit(main())
