#!/usr/bin/env python3
"""
Compare les temps d'exécution DSS-Checker (run_mc.sh) et LTSmin (verif_all.sh)
à partir des sorties de ces scripts (le script ne lance aucune vérification).

Usage :
  ./compare_times.py --case LABEL SORTIE_RUN_MC SORTIE_VERIF_ALL [--case ...] [-o courbes]

  SORTIE_RUN_MC    : au choix
                     - le tableau affiché par run_mc.sh, enregistré dans un fichier
                       (ex. ./run_mc.sh M.pn P.ltl | tee dss_A2_m1.txt)
                     - ou le log complet mc_<modèle>_<props>_<date>.log écrit par run_mc.sh
  SORTIE_VERIF_ALL : resume.txt écrit par verif_all.sh (ou son dossier resultats/<...>/),
                     ou l'affichage de verif_all.sh enregistré avec tee.

Exemple :
  ./compare_times.py \
      --case A2-m1 dss_A2_m1.txt resultats/SharedMemory5_A2_p_m1_volee/resume.txt \
      --case A3-m1 mc_SharedMemory5_A3_3M_sharedmemory5_3M_m1_20261117_180000.log \
                   resultats/SharedMemory5_A3_p_m1_volee \
      -o courbes

Sorties (dans le dossier -o, défaut : courbes) :
  - <cas>_par_propriete.png : temps de vérification de chaque propriété, deux outils,
    échelle log. Vert = propriété vérifiée, rouge = violée ; cercle noir = verdicts
    différents entre les deux outils.
  - <cas>_cumul.png : temps cumulé (construction du DSS incluse ; construction de
    l'espace d'états LTSmin incluse en mode « complet »).
  - comparaison_cas.png (au moins deux cas) : temps total par cas.
  - resultats.csv : toutes les mesures.

La i-ème propriété de DSS-Checker (#i-1) est appariée à prop<i> de LTSmin
(ltl2action.sh numérote dans l'ordre des lignes non vides du fichier .ltl).
"""
import argparse
import csv
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent

# Couleurs : statut vert/rouge (verdict), encre neutre pour le reste.
GREEN = "#0ca30c"      # propriété vérifiée
RED = "#d03b3b"        # propriété violée
GRAY = "#8a8984"       # verdict inconnu / erreur
INK = "#0b0b0b"
INK_2 = "#52514e"
GRID = "#e1e0d9"
AXIS = "#c3c2b7"
SURFACE = "#fcfcfb"

# Identité de l'outil : forme du marqueur + style de trait (la couleur porte le verdict).
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
    dss_ok: bool | None = None      # True = vérifiée, False = violée, None = inconnu
    lts_time: float | None = None
    lts_ok: bool | None = None
    lts_states: str = ""
    lts_arcs: str = ""
    lts_mem: str = ""


@dataclass
class Case:
    label: str
    props: list[Prop] = field(default_factory=list)
    dss_build: float | None = None  # s
    lts_build: float | None = None  # s (mode complet uniquement)


# ----------------------------------------------------------------- parsing

def parse_dss(path):
    """Sortie de run_mc.sh : tableau résumé affiché, ou log complet de distributedstatespace."""
    text = Path(path).read_text(errors="replace")
    if "***** Property#" in text:
        return parse_dss_full_log(text)
    return parse_dss_summary(text)


def parse_dss_full_log(text):
    props, build, cur = [], None, None
    for line in text.splitlines():
        if m := re.search(r"DSS building duration:\s*([\d.]+)\s*ms", line):
            build = float(m.group(1)) / 1000.0
        elif m := re.search(r"\*\*\*\*\* Property#(\d+)", line):
            cur = Prop(index=int(m.group(1)) + 1)
            props.append(cur)
        elif cur is None:
            continue
        elif m := re.search(r"Checking formula:\s*(.*)", line):
            cur.formula = m.group(1).strip()
        elif m := re.search(r"checked against module #(\d+)", line):
            cur.module = m.group(1)
        elif "Property is verified" in line:
            cur.dss_ok = True
        elif "Property is violated" in line:
            cur.dss_ok = False
        elif m := re.search(r"Verification duration:\s*([\d.eE+-]+)", line):
            cur.dss_time = float(m.group(1))
    return props, build


def parse_dss_summary(text):
    """Tableau affiché par run_mc.sh :
         Construction du DSS : 684 ms
         #     Module  Résultat   Durée vérif(s) Formule
         0     0       VERIFIEE   0.000223       !End_Own_Acc_5_5 W ...
    (la formule y est tronquée à 60 caractères)."""
    props, build = [], None
    for line in text.splitlines():
        if m := re.search(r"Construction du DSS\s*:\s*([\d.]+)\s*ms", line):
            build = float(m.group(1)) / 1000.0
            continue
        m = re.match(r"\s*(\d+)\s+(\S+)\s+(VERIFIEE|VIOLEE|\?\?)\s+(\S+)\s*(.*)$", line)
        if not m:
            continue
        p = Prop(index=int(m.group(1)) + 1, module=m.group(2), formula=m.group(5).strip())
        p.dss_ok = {"VERIFIEE": True, "VIOLEE": False}.get(m.group(3))
        try:
            p.dss_time = float(m.group(4))
        except ValueError:
            p.dss_time = None
        props.append(p)
    if not props:
        sys.exit(f"Aucune propriété trouvée dans la sortie DSS-Checker ({len(text)} caractères lus)")
    return props, build


def parse_ltsmin_resume(path):
    """resume.txt écrit par verif_all.sh."""
    path = Path(path)
    if path.is_dir():
        path = path / "resume.txt"
    rows, build = {}, None
    for line in path.read_text(errors="replace").splitlines():
        if m := re.search(r"Construction complète\s*:.*?,\s*([\d.]+)\s*s", line):
            build = float(m.group(1))
            continue
        cols = [c.strip() for c in line.split("|")]
        if len(cols) < 6 or not (m := re.fullmatch(r"prop(\d+)", cols[0])):
            continue
        res = cols[5].upper()
        ok = None if res.startswith("ERREUR") else (not res.startswith("NON"))
        try:
            t = float(cols[1])
        except ValueError:
            t = None
        rows[int(m.group(1))] = dict(time=t, ok=ok, states=cols[2], arcs=cols[3], mem=cols[4])
    if not rows:
        sys.exit(f"Aucune propriété trouvée dans la sortie LTSmin {path}")
    return rows, build


def build_case(label, dss_log, lts_resume):
    props, dss_build = parse_dss(dss_log)
    rows, lts_build = parse_ltsmin_resume(lts_resume)
    by_idx = {p.index: p for p in props}
    for i, r in rows.items():
        p = by_idx.setdefault(i, Prop(index=i))
        p.lts_time, p.lts_ok = r["time"], r["ok"]
        p.lts_states, p.lts_arcs, p.lts_mem = r["states"], r["arcs"], r["mem"]
    case = Case(label, sorted(by_idx.values(), key=lambda p: p.index), dss_build, lts_build)
    if len(props) != len(rows):
        print(f"[{label}] attention : {len(props)} propriétés DSS-Checker, {len(rows)} LTSmin", file=sys.stderr)
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
                 markeredgecolor="white", label="Propriété vérifiée"),
          Line2D([], [], linestyle="", marker="o", markersize=8, markerfacecolor=RED,
                 markeredgecolor="white", label="Propriété violée")]
    if with_mismatch:
        h.append(Line2D([], [], linestyle="", marker="o", markersize=13, markerfacecolor="none",
                        markeredgecolor=INK, markeredgewidth=1.5, label="Verdicts différents"))
    return h


def plot_series(ax, xs, ys, oks, tool):
    t = TOOLS[tool]
    pts = [(x, y, ok) for x, y, ok in zip(xs, ys, oks) if y is not None]
    if not pts:
        return
    px, py, pok = zip(*pts)
    ax.plot(px, py, color=t["line"], linestyle=t["linestyle"], linewidth=2, zorder=2)
    ax.scatter(px, py, marker=t["marker"], s=64, c=[verdict_color(o) for o in pok],
               edgecolors="white", linewidths=1.5, zorder=3)   # anneau clair sur chevauchement


def floor_times(values):
    """Échelle log : remplace 0 par un plancher visible."""
    pos = [v for v in values if v]
    floor = (min(pos) / 2) if pos else 1e-6
    return [None if v is None else max(v, floor) for v in values]


def plot_case(plt, case, outdir):
    xs = [p.index for p in case.props]
    dss_t = floor_times([p.dss_time for p in case.props])
    lts_t = floor_times([p.lts_time for p in case.props])
    mism = [p for p in case.props if p.dss_ok is not None and p.lts_ok is not None and p.dss_ok != p.lts_ok]

    # --- temps par propriété
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
    ax.set_xlabel("Propriété", color=INK_2)
    ax.set_ylabel("Temps de vérification (s, échelle log)", color=INK_2)
    title = f"{case.label} : temps de vérification par propriété"
    if case.dss_build is not None:
        title += f"\nconstruction du DSS : {case.dss_build:.3f} s (non incluse)"
    ax.set_title(title, color=INK, fontsize=12, loc="left")
    ax.legend(handles=legend_handles(plt, bool(mism)), frameon=False, fontsize=9,
              loc="upper left", bbox_to_anchor=(1.01, 1))
    fig.tight_layout()
    f1 = outdir / f"{safe(case.label)}_par_propriete.png"
    fig.savefig(f1, dpi=150)
    plt.close(fig)

    # --- temps cumulé
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
    ax.set_xlabel("Nombre de propriétés vérifiées", color=INK_2)
    ax.set_ylabel("Temps cumulé (s)", color=INK_2)
    note = "DSS-Checker : construction du DSS incluse"
    if case.lts_build is not None:
        note += " ; LTSmin : construction de l'espace d'états incluse"
    ax.set_title(f"{case.label} : temps cumulé\n{note}", color=INK, fontsize=12, loc="left")
    ax.legend(handles=legend_handles(plt, False), frameon=False, fontsize=9,
              loc="upper left", bbox_to_anchor=(1.01, 1))
    fig.tight_layout()
    f2 = outdir / f"{safe(case.label)}_cumul.png"
    fig.savefig(f2, dpi=150)
    plt.close(fig)
    return f1, f2, mism


def plot_cases(plt, cases, outdir):
    labels = [c.label for c in cases]
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
    ax.set_ylabel("Temps total (s, échelle log)", color=INK_2)
    ax.set_title("Temps total par cas (construction + toutes les propriétés)", color=INK, fontsize=12, loc="left")
    ax.legend(frameon=False, fontsize=9, loc="upper left", bbox_to_anchor=(1.01, 1))
    fig.tight_layout()
    f = outdir / "comparaison_cas.png"
    fig.savefig(f, dpi=150)
    plt.close(fig)
    return f


def safe(s):
    return re.sub(r"[^\w.-]+", "_", s)


def write_csv(cases, path):
    with open(path, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["cas", "propriete", "module", "formule", "dss_temps_s", "dss_verdict",
                    "ltsmin_temps_s", "ltsmin_verdict", "ltsmin_etats", "ltsmin_arcs",
                    "ltsmin_mem_mo", "dss_construction_s", "ltsmin_construction_s"])
        v = lambda ok: "" if ok is None else ("verifiee" if ok else "violee")
        for c in cases:
            for p in c.props:
                w.writerow([c.label, p.index, p.module, p.formula, p.dss_time, v(p.dss_ok),
                            p.lts_time, v(p.lts_ok), p.lts_states, p.lts_arcs, p.lts_mem,
                            c.dss_build, c.lts_build])


# ----------------------------------------------------------------- main

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--case", nargs=3, action="append", required=True,
                    metavar=("LABEL", "SORTIE_RUN_MC", "SORTIE_VERIF_ALL"),
                    help="un cas à comparer (répéter l'option pour plusieurs cas)")
    ap.add_argument("-o", "--outdir", default="courbes", help="dossier de sortie (défaut : courbes)")
    args = ap.parse_args()

    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        sys.exit("matplotlib manquant : sudo apt install python3-matplotlib  (ou pip install matplotlib)")

    for label, dss_out, lts_out in args.case:
        for f in (dss_out, lts_out):
            if not Path(f).exists():
                sys.exit(f"[{label}] fichier introuvable : {f}")

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
              f"désaccords : {[p.index for p in mism] or 'aucun'}")
        print(f"   {f1}\n   {f2}")
    if len(cases) >= 2:
        print(f"   {plot_cases(plt, cases, outdir)}")
    write_csv(cases, outdir / "resultats.csv")
    print(f"   {outdir / 'resultats.csv'}")


if __name__ == "__main__":
    main()
