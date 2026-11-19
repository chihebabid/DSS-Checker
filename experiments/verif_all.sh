#!/bin/bash
# Verifies all LTL properties in a directory against a Petri net model.
# Usage: ./verif_all.sh [model.pnml] [props_dir] [mode]
#   mode = onthefly (default) : pnml2lts-seq, on-the-fly verification on the model
#                               ("volee" is accepted as an alias)
#   mode = complet           : 1) full state space construction
#                                 (pnml2lts-sym -> ETF file)
#                              2) verification of each property on the
#                                 constructed state space (etf2lts-seq)
# For each property: time, explored states and transitions (product with
# the Büchi automaton), and peak memory usage (RSS) of the process.
# Results are saved in resultats/<model>_<props_dir>_<mode>/
MODEL="${1:-ERK1.pnml}"
DIR="${2:-erk_props}"
MODE="${3:-onthefly}"
[ "$MODE" = "volee" ] && MODE=onthefly   # former name

if [ ! -f "$MODEL" ]; then echo "Model not found: $MODEL" >&2; exit 1; fi
if ! ls "$DIR"/prop*.ltl >/dev/null 2>&1; then echo "No prop*.ltl in $DIR" >&2; exit 1; fi
if [ "$MODE" != "onthefly" ] && [ "$MODE" != "complet" ]; then
    echo "Unknown mode: $MODE (onthefly or complet)" >&2; exit 1
fi
if [ ! -x /usr/bin/time ]; then echo "/usr/bin/time not found (package 'time')" >&2; exit 1; fi

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
out "Model: %s | Properties: %s | Mode: %s | %s\n\n" "$MODEL" "$DIR" "$MODE" "$(date '+%F %T')"

if [ "$MODE" = "complet" ]; then
    ETF="$OUT/$(basename "$MODEL" .pnml).etf"
    t0=$(now)
    run_mem "$OUT/logs/construction.mem" pnml2lts-sym "$MODEL" "$ETF" > "$OUT/logs/construction.log" 2>&1
    t1=$(now)
    if [ ! -s "$ETF" ]; then
        out "ERROR: state space construction failed (see %s)\n" "$OUT/logs/construction.log"
        exit 1
    fi
    # count states and transitions of the full state space (by traversing the ETF file)
    etf2lts-seq --state=table "$ETF" > "$OUT/logs/comptage.log" 2>&1
    read -r ns na <<< "$(stats "$OUT/logs/comptage.log")"
    out "Full construction: %s states, %s arcs, %s s, %s MB\n\n" \
        "$ns" "$na" "$(elapsed $t0 $t1)" "$(mo "$OUT/logs/construction.mem")"
    out "(States / arcs below: part of the product explored during verification)\n\n"
fi

hdr="%-12s | %9s | %11s | %11s | %9s | %s\n"
out "%-12s | %9s | %11s | %11s | %9s | %s\n" "Property" "Time (s)" "States" "Arcs" "Mem. (MB)" "Result"
out -- "-------------+-----------+-------------+-------------+-----------+------------------------------\n"

ok=0; ko=0; err=0
for f in $(ls "$DIR"/prop*.ltl | sort -V); do
    name=$(basename "$f" .ltl)
    log="$OUT/logs/$name.log"
    memf="$OUT/logs/$name.mem"
    ce="$OUT/contre_exemples/ce_$name.gcf"
    t0=$(now)
    if [ "$MODE" = "complet" ]; then
        run_mem "$memf" etf2lts-seq -v --ltl="$f" --strategy=scc --state=table --trace="$ce" "$ETF" > "$log" 2>&1
    else
        run_mem "$memf" pnml2lts-seq -v --ltl="$f" --strategy=scc --state=table --trace="$ce" "$MODEL" > "$log" 2>&1
    fi
    t1=$(now)
    if grep -qi "accepting cycle found" "$log"; then
        res="NOT VERIFIED (counterexample: $ce)"; ko=$((ko+1))
    elif grep -qiE "empty product|no accepting cycle" "$log"; then
        res="VERIFIED"; ok=$((ok+1)); rm -f "$ce"
    elif grep -qi "property is TRUE" "$log"; then
        # empty Büchi automaton for the negation: formula is always true,
        # LTSmin concludes without exploring the model
        res="VERIFIED (tautology, no exploration)"; ok=$((ok+1)); rm -f "$ce"
    elif grep -qi "property is FALSE" "$log"; then
        res="NOT VERIFIED (unsatisfiable formula, no exploration)"; ko=$((ko+1))
    else
        res="ERROR (see $log)"; err=$((err+1))
    fi
    read -r ns na <<< "$(stats "$log")"
    out "$hdr" "$name" "$(elapsed $t0 $t1)" "$ns" "$na" "$(mo "$memf")" "$res"
done

out "\nSummary: %d verified, %d not verified, %d error(s).\n" $ok $ko $err
out "Total time: %s s\n" "$(elapsed $t_total0 $(now))"
echo "Results in $OUT/"
