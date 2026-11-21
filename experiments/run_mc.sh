#!/usr/bin/env bash
# Runs LTL verification with ./distributedstatespace and displays a summary:
# DSS construction duration, DSS statistics (meta-states, SCCs, sync arcs,
# total and stored states), then the module, result, and verification
# duration for each property (values computed by the program itself).
#
# Usage: ./run_mc.sh <model.pn> <properties.ltl> [additional options...]
# Example: ./run_mc.sh ERK10.pn erk_all.ltl
#          ./run_mc.sh ERK10.pn erk_all.ltl --fairness --no-reduction
#
# The full program output is saved in a log file.

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

LOG="mc_$(basename "${MODEL%.*}")_$(basename "${PROPS%.*}")_$(date +%Y%m%d_%H%M%S).log"

echo "Model      : $MODEL"
echo "Properties : $PROPS"
[[ $# -gt 0 ]] && echo "Options    : $*"
echo "Log        : $LOG"
echo "Running..."

"$BIN" --file "$MODEL" --mc "$PROPS" "$@" > "$LOG" 2>&1
STATUS=$?

awk -v status="$STATUS" '
    /^DSS built in/ {
        sub(/^DSS built in */, ""); build = $0
    }
    # DSS statistics table: copied as is (header, per-module rows and total)
    /^DSS statistics/ { in_stats = 1; next }
    in_stats && /^ *$/ { in_stats = 0 }
    in_stats { stats[++ns] = $0 }
    /^Property #/ {
        sub(/^Property #/, ""); n++; id[n] = $0
    }
    /^  Formula *:/ {
        sub(/^  Formula *: */, ""); formula[n] = $0
    }
    /^  Module *:/ {
        sub(/^  Module *: */, ""); module[n] = $0
    }
    /^  Result *: *VERIFIED/ { res[n] = "VERIFIED"; nok++ }
    /^  Result *: *VIOLATED/ { res[n] = "VIOLATED"; nko++ }
    /^  Result *: *SKIPPED/  { res[n] = "SKIPPED";  nsk++; module[n] = "-" }
    /^  Verification time *:/ {
        sub(/^  Verification time *: */, ""); sub(/ *s$/, ""); dur[n] = $0; total += $0
    }
    END {
        rule = "------------------------------------------------------------------------------"
        print ""
        print "=============================================================================="
        printf "DSS construction : %s\n", (build != "" ? build : "n/a")
        print "=============================================================================="
        if (ns > 0) {
            print "DSS statistics"
            for (i = 1; i <= ns; i++) print stats[i]
            print "=============================================================================="
        }
        printf "%-5s %-7s %-10s %-14s %s\n", "#", "Module", "Result", "Time (s)", "Formula"
        print rule
        for (i = 1; i <= n; i++) {
            f = formula[i]; if (length(f) > 60) f = substr(f, 1, 57) "..."
            printf "%-5s %-7s %-10s %-14s %s\n", id[i], module[i],
                   (res[i] != "" ? res[i] : "??"), (dur[i] != "" ? dur[i] : "n/a"), f
        }
        print rule
        printf "Properties: %d  |  verified: %d  |  violated: %d  |  skipped: %d\n", n, nok, nko, nsk
        printf "Total verification time: %g s\n", total
        if (status != 0)
            printf "WARNING: the program exited with code %d (see the log)\n", status
    }
' "$LOG"

exit $STATUS
