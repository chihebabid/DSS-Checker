#!/usr/bin/env python3
"""
Figures comparing formula checking durations, after plot-results.py (pmc-sog project),
built from the resultats.csv files written by compare_times.py.

Usage :
  ./plot_results.py courbes/A2_m1/resultats.csv [autre/resultats.csv ...] [-o courbes/figures]

Each CSV row "cas" is read as "<model> (LTSmin <mode>)"; the tools compared are
DSS-Checker and LTSmin <mode> (one per mode found: onthefly, complet), shown on the
figures as "LTSmin onthefly" and "LTSmin full" (the former name volee is read as onthefly).

Models named "<base>-m<k>" (one per module, e.g. A2-m1, A2-m2, A2-m3) are also merged
into a model "<base>-all" whose formulas are renumbered in module order (m1 1..20,
m2 21..40, m3 41..60); its figures mark the module of each formula
(dashed separators, or marker shape on the log-log figures).

LTSmin runs whose explored states reach the capacity of its state table (2^24 states
with --state=table, as used by verif_all.sh) stop exploring without any error: their
time and explored states are lower bounds. Such points get an arrow pointing in the
direction of the true value.

Outputs (PDF + PNG) in the -o folder:
  time-plots/<model>          : time of each formula for every tool (as create_figure)
  explored-states/<model>     : LTSmin explored states per formula, mean per verdict
                                (as create_figure_explored_states)
  compare_tools/<model>/<model>-<x>-VS-<y>-log :
                                log-log scatter of tool x vs tool y, one point per formula,
                                green = verified (T), red = violated (F), black = unknown (U),
                                diagonal y = x (as create_log_figure)
"""
import argparse
import csv
import re
import sys
from collections import defaultdict
from itertools import combinations
from pathlib import Path

ZERO = 1e-6          # floor for log scale (DSS-Checker times go down to ~1e-5 s)
TIMEOUT = 10 * 60    # 10 minutes: axis upper bound and value for unfinished runs

DSS = "DSS-Checker"
VERDICT = {"verifiee": "T", "violee": "F"}
PROPERTY_COLORS = {"T": "green", "F": "red", "U": "black"}
TOOL_COLORS = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
MODULE_MARKERS = ["o", "s", "^", "D", "v", "P"]
MODULE_RE = re.compile(r"(.*?)[-_ ]m(\d+)")
LTSMIN_TABLE_LIMIT = 2 ** 24   # pnml2lts-seq / etf2lts-seq --state=table capacity
TRUNCATION_MARGIN = 1024       # runs within this many states of the limit are truncated
LOWER_BOUND_LABEL = "LTSmin lower bound\n(state table full,\n2$^{24}$ states)"


# ----------------------------------------------------------------- data

MODE_NAMES = {"volee": "onthefly", "complet": "full"}


def display(tool):
    """Tool name as shown on figures (LTSmin-onthefly -> LTSmin onthefly)."""
    if tool.startswith("LTSmin-"):
        mode = tool[len("LTSmin-"):]
        return f"LTSmin {MODE_NAMES.get(mode, mode)}"
    return tool


def to_float(s):
    try:
        return float(s)
    except (TypeError, ValueError):
        return None


def load(csv_files):
    """Returns {model: {formula: row}} where row = {
         'times': {tool: seconds or None}, 'verdicts': {tool: 'T'/'F'/'U'},
         'property': ground truth 'T'/'F'/'U', 'explored_states': int or None,
         'formula_text': str}"""
    data = defaultdict(dict)
    for f in csv_files:
        with open(f, newline="") as fh:
            for r in csv.DictReader(fh):
                m = re.fullmatch(r"\s*(.*?)\s*\(LTSmin\s+(\w+)\)\s*", r["cas"])
                mode = "onthefly" if m and m.group(2) == "volee" else (m.group(2) if m else "")
                model, lts_tool = (m.group(1), f"LTSmin-{mode}") if m else (r["cas"], "LTSmin")
                formula = int(r["propriete"])
                row = data[model].setdefault(formula, dict(
                    times={}, verdicts={}, property="U", explored_states=None,
                    formula_text=r["formule"], module=None, local_formula=formula,
                    truncated=set()))
                row["times"][DSS] = to_float(r["dss_temps_s"])
                row["verdicts"][DSS] = VERDICT.get(r["dss_verdict"], "U")
                row["times"][lts_tool] = to_float(r["ltsmin_temps_s"])
                row["verdicts"][lts_tool] = VERDICT.get(r["ltsmin_verdict"], "U")
                states = to_float(r["ltsmin_etats"])
                if states is not None:
                    row["explored_states"] = int(states)
                    if states >= LTSMIN_TABLE_LIMIT - TRUNCATION_MARGIN:
                        row["truncated"].add(lts_tool)
    # ground truth: LTSmin verdict (as pnml2lts-mc in plot-results.py), else DSS-Checker
    for formulas in data.values():
        for row in formulas.values():
            lts = [v for t, v in row["verdicts"].items() if t != DSS and v != "U"]
            row["property"] = lts[0] if lts else row["verdicts"].get(DSS, "U")
    return data


def merge_modules(data):
    """{base-all: formulas} for every base having at least two module models
    <base>-m<k>; formulas are renumbered consecutively in module order."""
    groups = defaultdict(dict)
    for model in data:
        if m := MODULE_RE.fullmatch(model):
            groups[m.group(1)][int(m.group(2))] = model
    merged = {}
    for base, modules in groups.items():
        if len(modules) < 2:
            continue
        formulas, offset = {}, 0
        for k in sorted(modules):
            src = data[modules[k]]
            for i in sorted(src):
                formulas[offset + i] = dict(src[i], module=k, local_formula=i)
            offset += max(src)
        merged[f"{base}-all"] = formulas
    return merged


def module_bounds(formulas):
    """[(module, first, last)] in formula order, empty if formulas are not merged."""
    bounds = []
    for x in sorted(formulas):
        k = formulas[x]["module"]
        if k is None:
            return []
        if bounds and bounds[-1][0] == k:
            bounds[-1][2] = x
        else:
            bounds.append([k, x, x])
    return bounds if len(bounds) > 1 else []


def draw_module_separators(ax, formulas):
    """Dashed vertical lines between modules and the module name above each block."""
    bounds = module_bounds(formulas)
    for i, (k, first, last) in enumerate(bounds):
        if i:
            ax.axvline(first - 0.5, color="gray", linestyle="--", linewidth=1)
        ax.annotate(f"module {k}", xy=((first + last) / 2, 1.0), xycoords=("data", "axes fraction"),
                    xytext=(0, -4), textcoords="offset points",
                    ha="center", va="top", fontsize=9, color="gray")


def set_formula_ticks(ax, formulas):
    xs = sorted(formulas)
    if len(xs) <= 25:
        ax.set_xticks(xs)
    else:
        bounds = module_bounds(formulas)
        ticks = sorted({b[1] for b in bounds} | {bounds[-1][2]}) if bounds \
            else [x for x in xs if x % 5 == 0 or x == xs[0]]
        ax.set_xticks(ticks)
    ax.tick_params(axis="x", labelsize=8)


def tools_of(formulas):
    tools = {t for row in formulas.values() for t in row["times"]}
    return sorted(tools, key=lambda t: (t != DSS, t))


def plot_time(t):
    """Unfinished -> TIMEOUT, 0 -> ZERO (log(0) cannot be plotted)."""
    if t is None:
        return TIMEOUT
    return max(t, ZERO)


# ----------------------------------------------------------------- figures

def lower_bound_arrow(ax, x, y, dx, dy, color):
    """Arrow starting at the point (x, y), pointing towards (dx, dy) in screen space."""
    ax.annotate("", xy=(x, y), xytext=(dx * 16, dy * 16), textcoords="offset points",
                arrowprops=dict(arrowstyle="<-", color=color, linewidth=1.2, shrinkA=0, shrinkB=4))


def lower_bound_handle(arrow=r"$\uparrow$"):
    from matplotlib.lines import Line2D
    return Line2D([], [], linestyle="", marker=arrow, markersize=10, color="black",
                  label=LOWER_BOUND_LABEL)


def simple_white(ax):
    """Approximates the plotly 'simple_white' template with mirrored axes."""
    for s in ax.spines.values():
        s.set_visible(True)
        s.set_color("black")
        s.set_linewidth(1)
    ax.tick_params(direction="out", top=True, right=True, labeltop=False, labelright=False)
    ax.set_facecolor("white")


def save(fig, path):
    path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(path.with_suffix(".pdf"), bbox_inches="tight")
    fig.savefig(path.with_suffix(".png"), dpi=150, bbox_inches="tight")


def figure_width(formulas):
    return 6 if len(formulas) <= 25 else 10


def create_figure(plt, model, formulas):
    """Formula vs time, one colour per tool (log y: DSS-Checker and LTSmin
    differ by several orders of magnitude)."""
    fig, ax = plt.subplots(figsize=(figure_width(formulas), 5))
    simple_white(ax)
    xs = sorted(formulas)
    truncated = False
    for i, tool in enumerate(tools_of(formulas)):
        color = TOOL_COLORS[i % len(TOOL_COLORS)]
        ax.scatter(xs, [plot_time(formulas[x]["times"].get(tool)) for x in xs],
                   marker="x", color=color, label=display(tool))
        for x in xs:
            if tool in formulas[x]["truncated"]:
                lower_bound_arrow(ax, x, plot_time(formulas[x]["times"].get(tool)), 0, 1, color)
                truncated = True
    ax.set_yscale("log")
    lo, hi = ax.get_ylim()
    ax.set_ylim(lo, hi * 3)   # room for the module labels
    ax.set_xlim(xs[0] - 1, xs[-1] + 1)
    set_formula_ticks(ax, formulas)
    draw_module_separators(ax, formulas)
    ax.set_xlabel("formula")
    ax.set_ylabel("time (s)")
    ax.set_title(model)
    handles, _ = ax.get_legend_handles_labels()
    if truncated:
        handles.append(lower_bound_handle())
    ax.legend(handles=handles, title="tool", frameon=False, loc="upper left", bbox_to_anchor=(1.01, 1))
    fig.tight_layout()
    return fig


def create_figure_explored_states(plt, model, formulas):
    """LTSmin explored states for each formula, with the mean per verdict."""
    fig, ax = plt.subplots(figsize=(figure_width(formulas), 5))
    simple_white(ax)
    plotted = truncated_any = False
    for i, p in enumerate(("T", "F")):
        pts = [(x, r["explored_states"]) for x, r in sorted(formulas.items())
               if r["property"] == p and r["explored_states"] is not None]
        if not pts:
            continue
        plotted = True
        xs, ys = zip(*pts)
        mean = sum(ys) / len(ys)
        color = PROPERTY_COLORS[p]
        ax.scatter(xs, ys, marker="o", color=color, label=p)
        truncated = [x for x in xs if formulas[x]["truncated"]]
        for x in truncated:
            lower_bound_arrow(ax, x, formulas[x]["explored_states"], 0, 1, color)
        truncated_any |= bool(truncated)
        ax.plot([min(xs), max(xs)], [mean, mean], color=color, linewidth=1.5)
        ax.annotate(f"mean {'≥' if truncated else '='} {mean:.2E}", xy=(1.01, mean), xycoords=("axes fraction", "data"),
                    color=color, va="center", ha="left", fontsize=9)
    if not plotted:
        plt.close(fig)
        return None
    ax.set_xlim(0, max(formulas) + 1)
    lo, hi = ax.get_ylim()
    ax.set_ylim(lo, hi + 0.08 * (hi - lo))   # room for the module labels
    set_formula_ticks(ax, formulas)
    draw_module_separators(ax, formulas)
    ax.set_xlabel("formula")
    ax.set_ylabel("# explored states (LTSmin)")
    ax.set_title(model)
    handles, _ = ax.get_legend_handles_labels()
    if truncated_any:
        handles.append(lower_bound_handle())
    ax.legend(handles=handles, title="property", frameon=False, loc="upper left", bbox_to_anchor=(1.01, 0.9))
    fig.tight_layout()
    fig.subplots_adjust(right=(0.75 if len(formulas) <= 25 else 0.83) - (0.08 if truncated_any else 0))
    return fig


def create_log_figure(plt, model, formulas, tool_x, tool_y):
    """Log-log scatter tool_x vs tool_y; below the diagonal, tool_y is faster."""
    fig, ax = plt.subplots(figsize=(6, 6))
    simple_white(ax)
    modules = sorted({r["module"] for r in formulas.values()}, key=lambda k: (k is None, k))
    by_module = len(module_bounds(formulas)) > 1
    for p, color in PROPERTY_COLORS.items():
        for j, k in enumerate(modules):
            pts = [(plot_time(r["times"].get(tool_x)), plot_time(r["times"].get(tool_y)))
                   for r in formulas.values() if r["property"] == p and r["module"] == k]
            if pts:
                xs, ys = zip(*pts)
                ax.scatter(xs, ys, marker=MODULE_MARKERS[j % len(MODULE_MARKERS)] if by_module else "o",
                           facecolors="none", edgecolors=color)
    truncated = False
    for r in formulas.values():
        dx, dy = int(tool_x in r["truncated"]), int(tool_y in r["truncated"])
        if dx or dy:
            truncated = True
            lower_bound_arrow(ax, plot_time(r["times"].get(tool_x)), plot_time(r["times"].get(tool_y)),
                              dx, dy, PROPERTY_COLORS[r["property"]])
    from matplotlib.lines import Line2D
    present = {r["property"] for r in formulas.values()}
    handles = [Line2D([], [], linestyle="", marker="o", markerfacecolor="none", markeredgecolor=c,
                      label=p) for p, c in PROPERTY_COLORS.items() if p in present]
    if by_module:
        handles += [Line2D([], [], linestyle="", marker=MODULE_MARKERS[j % len(MODULE_MARKERS)],
                           markerfacecolor="none", markeredgecolor="black", label=f"module {k}")
                    for j, k in enumerate(modules)]
    if truncated:
        handles.append(lower_bound_handle(r"$\nearrow$" if tool_x != DSS else r"$\uparrow$"))
    ax.plot([ZERO, TIMEOUT], [ZERO, TIMEOUT], color="black", linewidth=1)
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlim(ZERO, TIMEOUT)
    ax.set_ylim(ZERO, TIMEOUT)
    ax.set_aspect("equal")
    ax.set_xlabel(f"{display(tool_x)} (s)")
    ax.set_ylabel(f"{display(tool_y)} (s)")
    ax.set_title(model)
    ax.legend(handles=handles, title="property", frameon=False, loc="upper left", bbox_to_anchor=(1.01, 1))
    return fig   # no tight_layout: it shrinks the square axes; save() crops to the legend


# ----------------------------------------------------------------- main

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("csv", nargs="+", help="resultats.csv written by compare_times.py")
    ap.add_argument("-o", "--outdir", default="courbes/figures", help="output folder (default: courbes/figures)")
    ap.add_argument("--no-merge", action="store_true",
                    help="do not merge the modules <base>-m<k> into <base>-all")
    args = ap.parse_args()

    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        sys.exit("matplotlib missing: sudo apt install python3-matplotlib  (or pip install matplotlib)")

    for f in args.csv:
        if not Path(f).is_file():
            sys.exit(f"file not found: {f}")

    out = Path(args.outdir).resolve()
    data = load(args.csv)
    if not args.no_merge:
        data.update(merge_modules(data))
    safe = lambda s: re.sub(r"[^\w.-]+", "_", s)

    for model, formulas in data.items():
        m = safe(model)
        written = []

        fig = create_figure(plt, model, formulas)
        save(fig, out / "time-plots" / m)
        plt.close(fig)
        written.append(out / "time-plots" / m)

        fig = create_figure_explored_states(plt, model, formulas)
        if fig is not None:
            save(fig, out / "explored-states" / m)
            plt.close(fig)
            written.append(out / "explored-states" / m)

        for tx, ty in combinations(tools_of(formulas), 2):
            fig = create_log_figure(plt, model, formulas, tx, ty)
            path = out / "compare_tools" / m / f"{m}-{safe(tx)}-VS-{safe(ty)}-log"
            save(fig, path)
            plt.close(fig)
            written.append(path)

        print(f"[{model}] {len(formulas)} formulas, tools: {', '.join(map(display, tools_of(formulas)))}")
        for p in written:
            print(f"   {p}.pdf / .png")


if __name__ == "__main__":
    main()
