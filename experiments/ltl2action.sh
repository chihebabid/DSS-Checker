#!/bin/bash
# Converts the properties of an .ltl file into the `action == "name"` format
# and writes one property per file (prop1.ltl, prop2.ltl, ...).
# Every variable (identifier) is converted, except LTL keywords
# (U, R, W, M, X, F, G, V, true, false) and strings already in quotes.
#
# Usage: ./ltl2action.sh file.ltl [output_dir] [model.pnml]
#   output_dir : defaults to <filename>_props
#   model.pnml : optional; reports variables that do not correspond to
#                any transition in the model
# Next step: ./verif_all.sh MODEL.pnml output_dir

if [ $# -lt 1 ] || [ ! -f "$1" ]; then
    echo "Usage : $0 fichier.ltl [repertoire_sortie] [modele.pnml]" >&2
    exit 1
fi

IN="$1"
OUT="${2:-$(basename "$IN" .ltl)_props}"
MODEL="$3"

if [ -d "$OUT" ] && ls "$OUT"/prop*.ltl >/dev/null 2>&1; then
    echo "Le répertoire $OUT contient déjà des propriétés." >&2
    read -r -p "Les remplacer ? [o/N] " rep
    [ "$rep" = "o" ] || [ "$rep" = "O" ] || exit 1
    rm -f "$OUT"/prop*.ltl
fi
mkdir -p "$OUT"

KEYWORDS='U|R|W|M|X|F|G|V|true|false|action'
CONVERT='s/("[^"]*")|\b([A-Za-z_][A-Za-z0-9_]*)\b/do { my ($q, $w) = ($1, $2); defined $q ? $q : ($w =~ m{^(?:'"$KEYWORDS"')$} ? $w : "(action == \"$w\")") }/ge'

i=1
while IFS= read -r line || [ -n "$line" ]; do
    line="${line%$'\r'}"
    [ -z "${line//[[:space:]]/}" ] && continue        # ignore empty lines
    printf '%s\n' "$line" | perl -pe "$CONVERT" > "$OUT/prop$i.ltl"
    i=$((i+1))
done < "$IN"
echo "$((i-1)) propriété(s) écrite(s) dans $OUT/"

# Optional check: variables missing from the model transitions
if [ -n "$MODEL" ]; then
    if [ ! -f "$MODEL" ]; then
        echo "Attention : modèle introuvable ($MODEL), contrôle ignoré." >&2
    else
        trans=$(grep -oE '<transition id="[^"]*"' "$MODEL" | sed -E 's/.*id="([^"]*)"/\1/' | sort -u)
        vars=$(grep -ohE 'action == "[^"]*"' "$OUT"/prop*.ltl | sed -E 's/.*"(.*)"/\1/' | sort -u)
        bad=$(comm -23 <(echo "$vars") <(echo "$trans"))
        if [ -n "$bad" ]; then
            echo "Attention : variables qui ne sont pas des transitions de $MODEL :" >&2
            echo "$bad" | sed 's/^/   /' >&2
        else
            echo "Toutes les variables sont des transitions de $MODEL."
        fi
    fi
fi
echo "Vérification : ./verif_all.sh MODELE.pnml $OUT"
