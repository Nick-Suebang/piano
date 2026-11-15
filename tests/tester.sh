#!/usr/bin/env bash
# Tests de bout en bout du programme console.
# Usage : tests/tester.sh [chemin du programme]   (par défaut : ./piano)
set -uo pipefail
cd "$(dirname "$0")/.."

PIANO="${1:-./piano}"
TEMP="$(mktemp -d)"
trap 'rm -rf "$TEMP"' EXIT
echecs=0

echec() { echo "ÉCHEC : $1"; echecs=$((echecs + 1)); }

# 1. Les exemples produisent un WAV valide, dont l'en-tête annonce la bonne taille.
for partition in partitions/*.txt; do
    wav="$TEMP/$(basename "$partition" .txt).wav"
    if ! "$PIANO" "$partition" "$wav" > /dev/null; then
        echec "$partition n'a pas été convertie"
        continue
    fi
    taille=$(wc -c < "$wav")
    riff=$(head -c 4 "$wav")
    tailleRiff=$(od -An -tu4 -j4 -N4 "$wav" | tr -d ' ')
    tailleDonnees=$(od -An -tu4 -j40 -N4 "$wav" | tr -d ' ')
    [ "$riff" = "RIFF" ] || echec "$partition : en-tête RIFF absent"
    [ "$tailleRiff" -eq $((taille - 8)) ] || echec "$partition : taille RIFF $tailleRiff au lieu de $((taille - 8))"
    [ "$tailleDonnees" -eq $((taille - 44)) ] || echec "$partition : taille des données $tailleDonnees au lieu de $((taille - 44))"
done

# 2. Une partition invalide est refusée avec le code 2.
refuser() {
    printf "$2" > "$TEMP/invalide.txt"
    "$PIANO" "$TEMP/invalide.txt" "$TEMP/invalide.wav" > /dev/null 2>&1
    [ $? -eq 2 ] || echec "partition invalide acceptée : $1"
}
refuser "tempo non numérique" 'abc\n3 DO - 1\n'
refuser "tempo non autorisé" '90\n3 DO - 1\n'
refuser "note inconnue" '120\n3 do - 1\n'
refuser "altération inconnue" '120\n3 DO x 1\n'
refuser "champ manquant" '120\n3 DO -\n'
refuser "durée nulle" '120\n3 DO - 0\n'
refuser "aucune note" '120\n\n'

# 3. Un fichier introuvable donne le code 2, de mauvais arguments le code 1.
"$PIANO" "$TEMP/introuvable.txt" > /dev/null 2>&1
[ $? -eq 2 ] || echec "fichier introuvable : code attendu 2"
"$PIANO" > /dev/null 2>&1
[ $? -eq 1 ] || echec "sans argument : code attendu 1"

if [ "$echecs" -eq 0 ]; then
    echo "Tous les tests passent."
else
    echo "$echecs test(s) en échec."
    exit 1
fi
