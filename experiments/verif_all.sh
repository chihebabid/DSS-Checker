#!/bin/bash
# Verifies all LTL properties in a directory against a Petri net model.
# Usage: ./verif_all.sh [model.pnml] [props_dir] [mode]
#   mode = volee   (default) : pnml2lts-seq, on-the-fly verification on the model
#   mode = complet           : 1) full state space construction
#                                 (pnml2lts-sym -> ETF file)
#                              2) verification of each property on the
#                                 constructed state space (etf2lts-seq)
# For each property: time, explored states and transitions (product with
# the Büchi automaton), and peak memory usage (RSS) of the process.
# Results are saved in resultats/<model>_<props_dir>_<mode>/
MODEL="${1:-ERK1.pnml}"
DIR="${2:-erk_props}"
MODE="${3:-volee}"

if [ ! -f "$MODEL" ]; then echo "Modèle introuvable : $MODEL" >&2; exit 1; fi
if ! ls "$DIR"/prop*.ltl >/dev/null 2>&1; then echo "Aucun prop*.ltl dans $DIR" >&2; exit 1; fi
if [ "$MODE" != "volee" ] && [ "$MODE" != "complet" ]; then
    echo "Mode inconnu : $MODE (volee ou complet)" >&2; exit 1
fi
if [ ! -x /usr/bin/time ]; then echo "/usr/bin/time introuvable (paquet 'time')" >&2; exit 1; fi

OUT="resultats/$(basename "$MODEL" .pnml)_$(basename "${DIR%/}")_$MODE"
rm -rf "$OUT"
mkdir -p "$OUT/logs" "$OUT/contre_exemples"
SUMMARY="$OUT/resume.txt"

out() { printf "$@" | tee -a "$SUMMARY"; }
now() { date +%s.%N; }
elapsed() { awk -v a="$1" -v b="$2" 'BEGIN{printf "%.3f", b-a}'; }
# runs a command while measuring peak memory (KB) into file $1
run_mem() { local memf="$1"; shift; /usr/bin/time -f "%M" -o "$memf" "$@"; }
mo() { awk -v k="$(tail -1 "$1" 2>/dev/null)" 'BEGIN{ if (k=="") print "?"; else printf "%.1f", k/1024 }'; }
# number of explored states / transitions, parsed from a verbose (-v) LTSmin log
stats() {
    local l
    l=$(grep -E 'state space .* states .* transitions' "$1" | tail -1)
    [ -z "$l" ] && l=$(grep -E 'explored [0-9]+ states, [0-9]+ transitions' "$1" | tail -1)
    if [ -z "$l" ] && grep -qiE "property is (TRUE|FALSE)" "$1"; then echo "0 0"; return; fi
    local s t
    s=$(echo "$l" | grep -oE '[0-9]+ states' | grep -oE '[0-9]+')
    t=$(echo "$l" | grep -oE '[0-9]+ transitions' | grep -oE '[0-9]+')
    echo "${s:-?} ${t:-?}"
}

t_total0=$(now)
out "Modèle : %s | Propriétés : %s | Mode : %s | %s\n\n" "$MODEL" "$DIR" "$MODE" "$(date '+%F %T')"

if [ "$MODE" = "complet" ]; then
    ETF="$OUT/$(basename "$MODEL" .pnml).etf"
    t0=$(now)
    run_mem "$OUT/logs/construction.mem" ./pnml2lts-sym "$MODEL" "$ETF" > "$OUT/logs/construction.log" 2>&1
    t1=$(now)
    if [ ! -s "$ETF" ]; then
        out "ERREUR : construction de l'espace d'états échouée (voir %s)\n" "$OUT/logs/construction.log"
        exit 1
    fi
    # count states and transitions of the full state space (by traversing the ETF file)
    ./etf2lts-seq --state=table "$ETF" > "$OUT/logs/comptage.log" 2>&1
    read -r ns na <<< "$(stats "$OUT/logs/comptage.log")"
    out "Construction complète : %s états, %s arcs, %s s, %s Mo\n\n" \
        "$ns" "$na" "$(elapsed $t0 $t1)" "$(mo "$OUT/logs/construction.mem")"
    out "(États / arcs ci-dessous : partie du produit explorée pendant la vérification)\n\n"
fi

hdr="%-12s | %9s | %11s | %11s | %9s | %s\n"
out "%-14s | %9s | %11s | %11s | %10s | %s\n" "Propriété" "Temps (s)" "États" "Arcs" "Mém. (Mo)" "Résultat"
out -- "-------------+-----------+-------------+-------------+-----------+------------------------------\n"

ok=0; ko=0; err=0
for f in $(ls "$DIR"/prop*.ltl | sort -V); do
    name=$(basename "$f" .ltl)
    log="$OUT/logs/$name.log"
    memf="$OUT/logs/$name.mem"
    ce="$OUT/contre_exemples/ce_$name.gcf"
    t0=$(now)
    if [ "$MODE" = "complet" ]; then
        run_mem "$memf" ./etf2lts-seq -v --ltl="$f" --strategy=scc --state=table --trace="$ce" "$ETF" > "$log" 2>&1
    else
        run_mem "$memf" ./pnml2lts-seq -v --ltl="$f" --strategy=scc --state=table --trace="$ce" "$MODEL" > "$log" 2>&1
    fi
    t1=$(now)
    if grep -qi "accepting cycle found" "$log"; then
        res="NON VÉRIFIÉE (contre-exemple : $ce)"; ko=$((ko+1))
    elif grep -qiE "empty product|no accepting cycle" "$log"; then
        res="VÉRIFIÉE"; ok=$((ok+1)); rm -f "$ce"
    elif grep -qi "property is TRUE" "$log"; then
        # empty Büchi automaton for the negation: formula is always true,
        # LTSmin concludes without exploring the model
        res="VÉRIFIÉE (tautologie, aucune exploration)"; ok=$((ok+1)); rm -f "$ce"
    elif grep -qi "property is FALSE" "$log"; then
        res="NON VÉRIFIÉE (formule insatisfiable, aucune exploration)"; ko=$((ko+1))
    else
        res="ERREUR (voir $log)"; err=$((err+1))
    fi
    read -r ns na <<< "$(stats "$log")"
    out "$hdr" "$name" "$(elapsed $t0 $t1)" "$ns" "$na" "$(mo "$memf")" "$res"
done

out "\nBilan : %d vérifiée(s), %d non vérifiée(s), %d erreur(s).\n" $ok $ko $err
out "Temps total : %s s\n" "$(elapsed $t_total0 $(now))"
echo "Résultats dans $OUT/"
