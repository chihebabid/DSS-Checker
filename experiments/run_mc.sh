#!/usr/bin/env bash
# Runs LTL verification with ./distributedstatespace and displays a summary:
# DSS construction duration, followed by the module, result, and verification
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
    echo "Usage : $0 <modele.pn> <proprietes.ltl> [options...]" >&2
    exit 1
fi

MODEL="$1"; PROPS="$2"; shift 2

[[ -x "$BIN" ]]   || { echo "Exécutable introuvable : $BIN" >&2; exit 1; }
[[ -f "$MODEL" ]] || { echo "Modèle introuvable : $MODEL" >&2; exit 1; }
[[ -f "$PROPS" ]] || { echo "Fichier LTL introuvable : $PROPS" >&2; exit 1; }

LOG="mc_$(basename "${MODEL%.*}")_$(basename "${PROPS%.*}")_$(date +%Y%m%d_%H%M%S).log"

echo "Modèle     : $MODEL"
echo "Propriétés : $PROPS"
[[ $# -gt 0 ]] && echo "Options    : $*"
echo "Log        : $LOG"
echo "Exécution en cours..."

"$BIN" --file "$MODEL" --mc "$PROPS" "$@" > "$LOG" 2>&1
STATUS=$?

awk -v status="$STATUS" '
    /DSS building duration:/ {
        sub(/.*DSS building duration: */, ""); build = $0
    }
    /\*\*\*\*\* Property#/ {
        sub(/.*Property#/, ""); n++; id[n] = $0
    }
    /Checking formula:/ {
        sub(/.*Checking formula: */, ""); formula[n] = $0
    }
    /Property will be checked against module #/ {
        sub(/.*module #/, ""); module[n] = $0
    }
    /Property is verified/ { res[n] = "VERIFIEE"; nok++ }
    /Property is violated/ { res[n] = "VIOLEE";   nko++ }
    /Verification duration:/ {
        sub(/.*Verification duration: */, ""); dur[n] = $0; total += $0
    }
    END {
        print ""
        print "================================================================"
        printf "Construction du DSS : %s\n", (build != "" ? build : "n/a")
        print "================================================================"
        printf "%-5s %-7s %-10s %-14s %s\n", "#", "Module", "Résultat", "Durée vérif(s)", "Formule"
        print "----------------------------------------------------------------"
        for (i = 1; i <= n; i++) {
            f = formula[i]; if (length(f) > 60) f = substr(f, 1, 57) "..."
            printf "%-5s %-7s %-10s %-14s %s\n", id[i], module[i],
                   (res[i] != "" ? res[i] : "??"), (dur[i] != "" ? dur[i] : "n/a"), f
        }
        print "----------------------------------------------------------------"
        printf "Propriétés : %d  |  vérifiées : %d  |  violées : %d\n", n, nok, nko
        printf "Durée totale de vérification : %g s\n", total
        if (status != 0)
            printf "ATTENTION : le programme s est terminé avec le code %d (voir le log)\n", status
    }
' "$LOG"

exit $STATUS
