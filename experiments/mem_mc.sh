#!/usr/bin/env bash
# Measures the peak memory (RSS) of DSS-Checker, as verif_all.sh does for LTSmin:
#   - one run that only builds the DSS (no --mc): memory of the construction;
#   - one run per formula of the .ltl file (DSS construction + checking of that formula only),
#     so that each formula gets its own peak, like each LTSmin process in verif_all.sh.
#
# Usage: ./mem_mc.sh <model.pn> <properties.ltl> [additional options...]
#   Options are forwarded to distributedstatespace (e.g. --fairness).
#   Environment variable BIN: path of distributedstatespace (default: ./distributedstatespace)
#
# Results in mem_<model>_<properties>/:
#   mem.csv : propriete,verdict,dss_mem_mo,dss_temps_s  (propriete 0 = construction only, verdict "-")
#   logs/   : output and memory file of each run

set -o pipefail
BIN="${BIN:-./distributedstatespace}"

if [[ $# -lt 2 ]]; then
    echo "Usage: $0 <model.pn> <properties.ltl> [options...]" >&2
    exit 1
fi
MODEL="$1"; PROPS="$2"; shift 2
[[ -x "$BIN" ]]   || { echo "Executable not found: $BIN" >&2; exit 1; }
[[ -f "$MODEL" ]] || { echo "Model not found: $MODEL" >&2; exit 1; }
[[ -f "$PROPS" ]] || { echo "LTL file not found: $PROPS" >&2; exit 1; }
[[ -x /usr/bin/time ]] || { echo "/usr/bin/time not found (package 'time')" >&2; exit 1; }

OUT="mem_$(basename "${MODEL%.*}")_$(basename "${PROPS%.*}")"
rm -rf "$OUT"
mkdir -p "$OUT/logs"
CSV="$OUT/mem.csv"
echo "propriete,verdict,dss_mem_mo,dss_temps_s" > "$CSV"

mo() { awk -v k="$(tail -1 "$1" 2>/dev/null)" 'BEGIN{ if (k=="") print ""; else printf "%.1f", k/1024 }'; }

# Construction only
/usr/bin/time -f "%M" -o "$OUT/logs/construction.mem" "$BIN" --file "$MODEL" "$@" > "$OUT/logs/construction.log" 2>&1
build_ms=$(grep -oE '^DSS built in *[0-9]+' "$OUT/logs/construction.log" | grep -oE '[0-9]+$')
echo "0,-,$(mo "$OUT/logs/construction.mem"),$(awk -v m="$build_ms" 'BEGIN{printf "%.3f", m/1000}')" >> "$CSV"
echo "Construction only: $(mo "$OUT/logs/construction.mem") MB"

# One run per formula (non-empty lines, numbered as ltl2action.sh / compare_times.py do)
i=0
while IFS= read -r line || [[ -n "$line" ]]; do
    line="${line%$'\r'}"
    [[ -z "${line//[[:space:]]/}" ]] && continue
    i=$((i+1))
    printf '%s\n' "$line" > "$OUT/logs/prop$i.ltl"
    /usr/bin/time -f "%M" -o "$OUT/logs/prop$i.mem" \
        "$BIN" --file "$MODEL" --mc "$OUT/logs/prop$i.ltl" "$@" > "$OUT/logs/prop$i.log" 2>&1
    if grep -qE '^  Result *: *VERIFIED' "$OUT/logs/prop$i.log"; then verdict=verifiee
    elif grep -qE '^  Result *: *VIOLATED' "$OUT/logs/prop$i.log"; then verdict=violee
    else verdict=erreur; fi
    t=$(grep -oE '^  Verification time *: *[0-9.eE+-]+' "$OUT/logs/prop$i.log" | awk '{print $NF}')
    echo "$i,$verdict,$(mo "$OUT/logs/prop$i.mem"),$t" >> "$CSV"
    printf 'prop%-3s %-9s %8s MB\n' "$i" "$verdict" "$(mo "$OUT/logs/prop$i.mem")"
done < "$PROPS"
echo "Results in $OUT/"
